# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The session of the ``fiction`` shell: the stores, the command dispatcher, and the statistics log."""

from __future__ import annotations

import contextlib
import datetime
import json
import math
import os
import shutil
import tempfile
import time
from enum import Enum
from pathlib import Path
from typing import TYPE_CHECKING

from rich.console import Console
from rich.markup import escape
from rich.text import Text

from mnt.pyfiction import convert_network, technology_network

from .errors import CommandError, HelpRequested
from .registry import REGISTRY
from .stores import CellEntry, GateLayout, Network, Store, describe, element_name, one_line

if TYPE_CHECKING:
    from mnt.pyfiction import dynamic_truth_table

    from .registry import Result

MAX_SCRIPT_DEPTH = 16
"""How deeply scripts may ``source`` one another before the session refuses, so a script that sources itself stops."""


# Keep quote, word, and command state together so execution and completion share one lexer.
def tokenize(line: str, *, incomplete: bool = False) -> list[list[str]]:  # ruff: ignore[complex-structure]
    """Split commands, preserving quoted separators and literal Windows backslashes.

    Args:
        line: Commands separated by unquoted semicolons; unquoted # starts a comment.
        incomplete: Permit an open quote and retain the last empty word for completion.

    Returns:
        Commands containing decoded words.

    Raises:
        ValueError: An execution line contains an unclosed quote.
    """
    commands: list[list[str]] = []
    words: list[str] = []
    word = ""
    quote = ""
    started = False
    for char in line:
        if quote:
            if char == quote:
                quote = ""
            else:
                word += char
        elif char in "\"'":
            quote = char
            started = True
        elif char == "#":
            break
        elif char == ";" or char.isspace():
            if started:
                words.append(word)
                word, started = "", False
            if char == ";":
                if words:
                    commands.append(words)
                words = []
        else:
            word += char
            started = True
    if quote and not incomplete:
        msg = "unclosed quote"
        raise ValueError(msg)
    if started or incomplete:
        words.append(word)
    if words:
        commands.append(words)
    return commands


def stats_to_dict(stats: object) -> dict[str, object]:
    """Convert a statistics object of the bindings into JSON-ready values.

    Every public, non-callable attribute becomes a key. Durations become float seconds under the
    attribute name with an ``_s`` suffix, enumerators become their names, and nested statistics
    objects recurse.

    Args:
        stats: A ``*_stats`` object of ``mnt.pyfiction``.

    Returns:
        The attributes as a dictionary; members whose C++ type has no caster are named under
        ``_unsupported`` instead of being dropped silently.
    """
    result: dict[str, object] = {}
    unsupported: list[str] = []
    for name in dir(stats):
        if name.startswith("_"):
            continue
        try:
            value = getattr(stats, name)
        except TypeError:
            # a member whose C++ type has no caster; naming it here makes the missing one visible
            unsupported.append(name)
            continue
        if callable(value):
            continue
        if isinstance(value, datetime.timedelta):
            result[f"{name}_s"] = value.total_seconds()
        else:
            if name == "report" and isinstance(value, str):
                with contextlib.suppress(ValueError):
                    value = json.loads(value)
            result[name] = json_value(value)
    if unsupported:
        result["_unsupported"] = unsupported
    return result


def json_value(value: object) -> object:
    """Convert one binding value into a JSON-ready value.

    Args:
        value: A scalar, an enumerator, a list, a path, or a statistics object.

    Returns:
        The value as JSON accepts it.
    """
    if isinstance(value, dict):
        return {str(key): json_value(item) for key, item in value.items()}
    if isinstance(value, float) and not math.isfinite(value):
        return None
    if isinstance(value, bool | int | float | str) or value is None:
        return value
    if isinstance(value, Enum):
        return value.name
    if isinstance(value, datetime.timedelta):
        return value.total_seconds()
    if isinstance(value, Path):
        return str(value)
    if isinstance(value, list | tuple):
        return [json_value(item) for item in value]
    if type(value).__module__.startswith("mnt.pyfiction"):
        return stats_to_dict(value)
    return str(value)


class Session:
    """The state of one shell session.

    Attributes:
        console: Where informational output goes.
        errors: Where error messages go.
        quiet: Set by ``--quiet``; :meth:`info` stays silent while it holds.
        truth_tables: The truth table store.
        networks: The logic network store.
        gate_layouts: The gate-level layout store.
        cell_layouts: The cell-level layout store.
        running: Cleared by ``quit``; the interactive loop stops when it is ``False``.
    """

    def __init__(
        self,
        console: Console | None = None,
        log_path: Path | None = None,
        errors: Console | None = None,
    ) -> None:
        """Create an empty session.

        Args:
            console: The console informational output goes to; defaults to standard output.
            log_path: Where :meth:`close` writes the JSON statistics log; ``None`` disables the log.
            errors: The console error messages go to; defaults to standard error, so that
                ``fiction -c '...' > out`` still shows what went wrong.
        """
        self.console = console if console is not None else Console(highlight=False)
        self.errors = errors if errors is not None else Console(highlight=False, stderr=True)
        self.quiet = False
        self.truth_tables: Store[dynamic_truth_table] = Store("truth table")
        self.networks: Store[Network] = Store("network")
        self.gate_layouts: Store[GateLayout] = Store("gate-level layout")
        self.cell_layouts: Store[CellEntry] = Store("cell-level layout")
        self.running = True
        self.log_path = log_path
        self.log: list[dict[str, object]] = []
        self._script_depth = 0
        self._source: tuple[str, int] | None = None
        self.close_failed = False
        self._temp_dir: Path | None = None
        self._temp_index = 0
        self._to_delete: list[Path] = []

    def execute(self, line: str) -> bool:
        """Run every command on a line, stopping at the first failure or at ``quit``.

        Args:
            line: The input line.

        Returns:
            ``True`` when every command succeeded.
        """
        try:
            commands = tokenize(line)
        except ValueError as error:
            self.error(str(error))
            if self.log_path is not None:
                self.log.append({"command": line, "status": "error", "error": str(error), "source": self._source})
            return False
        for argv in commands:
            if not self.running:  # 'quit' earlier on the line ends it
                return True
            if not self.run_command(argv):
                return False
        return True

    def run_command(self, argv: list[str]) -> bool:
        """Run one command and record it in the log.

        Args:
            argv: The command name followed by its arguments.

        Returns:
            ``True`` when the command succeeded or only printed its help.
        """
        name, *arguments = argv
        started = datetime.datetime.now(tz=datetime.timezone.utc)
        clock = time.perf_counter()
        entry: dict[str, object] = {
            "command": name,
            "argv": arguments,
            "args": {},
            "started": started.isoformat(),
            "source": self._source,
        }
        if self.log_path is not None:
            self.log.append(entry)
        stores = (self.truth_tables, self.networks, self.gate_layouts, self.cell_layouts)
        before = [(len(store), store.active) for store in stores]
        long_operation = name in {
            "exact",
            "ortho",
            "gold",
            "map",
            "abc",
            "aig",
            "optimize",
            "quickexact",
            "quicksim",
            "clustercomplete",
            "temp",
            "opdom",
        }
        try:
            result = self._invoke(argv, entry, long_operation=long_operation)
        except HelpRequested as help_request:
            self.console.print(help_request.text, markup=False, highlight=False)
            entry["status"] = "help"
            return True
        except (Exception, KeyboardInterrupt) as error:  # ruff: ignore[blind-except] -- native binding errors must not end the shell
            interrupted = isinstance(error, KeyboardInterrupt)
            message = "interrupted" if interrupted else str(error)
            self.error(f"{name}: {message}")
            entry.update(status="interrupted" if interrupted else "error", error=message)
            if statistics := getattr(error, "stats", None):
                entry["stats"] = json_value(statistics)
            return False
        finally:
            entry["runtime_s"] = time.perf_counter() - clock
        entry["status"] = "partial" if result and result.get("status") == "partial" else "ok"
        if result is not None and self.log_path is not None:
            entry["result"] = json_value(result)
        for store, previous in zip(stores, before, strict=False):
            if store.active is not None and (len(store), store.active) != previous:
                self.info(f"{store.kind} [{store.active}]: {one_line(describe(store.current()))}")
        if long_operation:
            self.info(f"{name}: completed in {time.perf_counter() - clock:.3f} s")
        return True

    def _invoke(self, argv: list[str], entry: dict[str, object], *, long_operation: bool) -> Result:
        """Validate and invoke one registered command.

        Args:
            argv: Command name and arguments.
            entry: Log entry that receives parsed options.
            long_operation: Print a start notice before native work.

        Returns:
            The command result.

        Raises:
            CommandError: The command or its arguments are invalid.
        """
        name, *arguments = argv
        cmd = REGISTRY.get(name)
        if cmd is None:
            msg = f"unknown command '{name}'; type 'help' for the list of commands"
            raise CommandError(msg)
        args = cmd.parser.parse_args(arguments)
        cmd.parser.validate(args)
        entry["args"] = {key: json_value(value) for key, value in vars(args).items()}
        if long_operation:
            self.info(f"{name}: running…")
        return cmd.run(self, args)

    def run_script(self, path: Path) -> bool:
        """Run the commands in a file, one line at a time, stopping at the first failure or at ``quit``.

        Args:
            path: The script file.

        Returns:
            ``True`` when every command succeeded.

        Raises:
            CommandError: When the file cannot be read or scripts nest deeper than
                :data:`MAX_SCRIPT_DEPTH`.
        """
        if self._script_depth >= MAX_SCRIPT_DEPTH:
            msg = f"scripts nest deeper than {MAX_SCRIPT_DEPTH} levels; does '{path}' source itself?"
            raise CommandError(msg)
        try:
            lines = path.read_text(encoding="utf-8").splitlines()
        except (OSError, UnicodeError) as error:
            if isinstance(error, UnicodeDecodeError):
                line_number = error.object[: error.start].count(b"\n") + 1
                msg_0 = f"{path}:{line_number}: script must be UTF-8"
                raise CommandError(msg_0) from error
            msg_0 = f"cannot read script '{path}': {error}"
            raise CommandError(msg_0) from error

        self._script_depth += 1
        previous_source = self._source
        try:
            for number, line in enumerate(lines, 1):
                self._source = (str(path), number)
                if not self.running:  # 'quit' in the script ends it
                    return True
                if not self.execute(line):
                    return False
            return True
        finally:
            self._script_depth -= 1
            self._source = previous_source

    def close(self) -> None:
        """Write the statistics log and remove the session's temporary files."""
        try:
            if self.log_path is not None:
                with self.log_path.open("w", encoding="utf-8") as file:
                    json.dump(json_value(self.log), file, indent=2, allow_nan=False)
                    file.write("\n")
        except OSError as error:
            self.close_failed = True
            self.error(f"cannot write log '{self.log_path}': {error}")
        finally:
            for path in self._to_delete:
                self._remove_viewer_file(path)
            self._to_delete.clear()
            if self._temp_dir is not None:
                shutil.rmtree(self._temp_dir, ignore_errors=True)
                self._temp_dir = None

    def _remove_viewer_file(self, path: Path) -> None:
        """Attempt one cleanup without preventing cleanup of other files.

        Args:
            path: A viewer file explicitly marked for deletion.
        """
        try:
            path.unlink(missing_ok=True)
        except OSError as error:
            self.error(f"cannot remove temporary file '{path}': {error}")

    def temp_file(self, suffix: str) -> Path:
        """Return a fresh path in the session's temporary directory.

        Args:
            suffix: The file suffix, including the dot.

        Returns:
            A path that does not exist yet and is removed by :meth:`close`.
        """
        if self._temp_dir is None:
            self._temp_dir = Path(tempfile.mkdtemp(prefix="fiction-"))
        self._temp_index += 1
        return self._temp_dir / f"{self._temp_index}{suffix}"

    def viewer_file(self, suffix: str, *, delete: bool) -> Path:
        """Return a fresh path for a file that is handed to an external viewer.

        Viewers return immediately and read the file afterwards, so the file must outlive the
        command that wrote it. It remains on disk unless ``delete`` requests removal when the session closes.

        Args:
            suffix: The file suffix, including the dot.
            delete: Remove the file when the session ends, as ``show --delete`` asks.

        Returns:
            An empty file that exists.
        """
        handle, name = tempfile.mkstemp(prefix="fiction-", suffix=suffix)
        os.close(handle)
        path = Path(name)
        if delete:
            self._to_delete.append(path)
        return path

    def error(self, message: str) -> None:
        """Print an error message.

        Args:
            message: The message; printed verbatim.
        """
        context = f"{self._source[0]}:{self._source[1]}: " if self._source else ""
        self.errors.print(f"[red]error:[/] {escape(context + message)}", highlight=False)

    def info(self, message: str) -> None:
        """Print an informational message, unless the session is quiet.

        Args:
            message: The message; printed verbatim.
        """
        if not self.quiet:
            self.output(message)

    def output(self, message: str) -> None:
        """Print an explicitly requested result, including in quiet mode.

        Args:
            message: Plain text or a native ANSI drawing.
        """
        self.console.print(Text.from_ansi(message), highlight=False)

    def status_line(self) -> str:
        """Return compact store counts and active indices within the terminal width.

        Returns:
            Store identifiers and counts, with names only when space permits.
        """
        stores = (self.truth_tables, self.networks, self.gate_layouts, self.cell_layouts)
        parts = [
            f"{label}[{store.active if store.active is not None else '-'}]/{len(store)}"
            for label, store in zip(("tt", "net", "gate", "cell"), stores, strict=False)
        ]
        width = self.console.width
        remaining = max(0, width - len(" · ".join(parts)))
        for index, store in enumerate(stores):
            if store.active is not None and remaining >= len(" name"):
                name = element_name(store.current())
                budget = min(20, remaining - 1)
                if name:
                    text = name if len(name) <= budget else name[: budget - 1] + "…"
                    parts[index] += " " + text
                    remaining -= len(text) + 1
        return " · ".join(parts)

    @staticmethod
    def as_technology_network(network: Network) -> technology_network:
        """Return a network as a technology network, converting any other type.

        Args:
            network: Any network of the network store.

        Returns:
            The network itself when it is a technology network, otherwise an equivalent one.
        """
        if isinstance(network, technology_network):
            return network
        return convert_network(network)
