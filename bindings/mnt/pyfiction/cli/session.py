# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The session of the ``fiction`` shell: the stores, the command dispatcher, and the statistics log."""

from __future__ import annotations

import datetime
import json
import shlex
import shutil
import tempfile
import time
from enum import Enum
from pathlib import Path
from typing import TYPE_CHECKING

from rich.console import Console
from rich.markup import escape

from mnt.pyfiction import convert_network, technology_network

from .registry import REGISTRY, CommandError, HelpRequested
from .stores import CellEntry, GateLayout, Network, Store

if TYPE_CHECKING:
    from mnt.pyfiction import dynamic_truth_table

MAX_SCRIPT_DEPTH = 16
"""How deeply scripts may ``source`` one another before the session refuses, so a script that sources itself stops."""


def tokenize(line: str) -> list[list[str]]:
    """Split an input line into commands.

    Commands are separated by ``;``, words by whitespace, and ``#`` starts a comment. Quotes group
    words and protect ``;`` and ``#`` inside them. Backslashes are ordinary characters, so Windows
    paths need no quoting.

    Args:
        line: The input line.

    Returns:
        The commands, each as its list of words.
    """
    lexer = shlex.shlex(line, posix=True, punctuation_chars=";")
    lexer.whitespace_split = True
    lexer.commenters = "#"
    # backslashes are path separators on Windows, not escape characters; quotes do the grouping
    lexer.escape = ""
    commands: list[list[str]] = []
    current: list[str] = []
    for word in lexer:
        if set(word) == {";"}:
            if current:
                commands.append(current)
            current = []
        else:
            current.append(word)
    if current:
        commands.append(current)
    return commands


def stats_to_dict(stats: object) -> dict[str, object]:
    """Convert a statistics object of the bindings into JSON-ready values.

    Every public, non-callable attribute becomes a key. Durations become float seconds under the
    attribute name with an ``_s`` suffix, enumerators become their names, and nested statistics
    objects recurse.

    Args:
        stats: A ``*_stats`` object of ``mnt.pyfiction``.

    Returns:
        The attributes as a dictionary.
    """
    result: dict[str, object] = {}
    for name in dir(stats):
        if name.startswith("_"):
            continue
        try:
            value = getattr(stats, name)
        except TypeError:
            continue  # a member whose C++ type has no Python binding
        if callable(value):
            continue
        if isinstance(value, datetime.timedelta):
            result[f"{name}_s"] = value.total_seconds()
        else:
            result[name] = json_value(value)
    return result


def json_value(value: object) -> object:
    """Convert one binding value into a JSON-ready value.

    Args:
        value: A scalar, an enumerator, a list, a path, or a statistics object.

    Returns:
        The value as JSON accepts it.
    """
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
        console: Where every message goes.
        truth_tables: The truth table store.
        networks: The logic network store.
        gate_layouts: The gate-level layout store.
        cell_layouts: The cell-level layout store.
        running: Cleared by ``quit``; the interactive loop stops when it is ``False``.
    """

    def __init__(self, console: Console | None = None, log_path: Path | None = None) -> None:
        """Create an empty session.

        Args:
            console: The console to print to; defaults to the terminal.
            log_path: Where :meth:`close` writes the JSON statistics log; ``None`` disables the log.
        """
        # soft wrapping keeps file paths and layout drawings on one line in narrow terminals
        self.console = console if console is not None else Console(soft_wrap=True)
        self.truth_tables: Store[dynamic_truth_table] = Store("truth table")
        self.networks: Store[Network] = Store("network")
        self.gate_layouts: Store[GateLayout] = Store("gate-level layout")
        self.cell_layouts: Store[CellEntry] = Store("cell-level layout")
        self.running = True
        self.log_path = log_path
        self.log: list[dict[str, object]] = []
        self._script_depth = 0
        self._temp_dir: Path | None = None

    def execute(self, line: str) -> bool:
        """Run every command on a line, stopping at the first failure.

        Args:
            line: The input line.

        Returns:
            ``True`` when every command succeeded.
        """
        try:
            commands = tokenize(line)
        except ValueError as error:
            self.error(str(error))
            return False
        return all(self.run_command(argv) for argv in commands)

    def run_command(self, argv: list[str]) -> bool:
        """Run one command and record it in the log.

        Args:
            argv: The command name followed by its arguments.

        Returns:
            ``True`` when the command succeeded or only printed its help.
        """
        name, *arguments = argv
        cmd = REGISTRY.get(name)
        if cmd is None:
            self.error(f"unknown command '{name}'; type 'help' for the list of commands")
            return False

        started = datetime.datetime.now(tz=datetime.timezone.utc)
        clock = time.perf_counter()
        entry: dict[str, object] = {"command": name, "args": {}, "started": started.isoformat()}
        try:
            args = cmd.parser.parse_args(arguments)
            entry["args"] = {key: json_value(value) for key, value in vars(args).items()}
            result = cmd.run(self, args)
        except HelpRequested as help_request:
            self.console.print(help_request.text, markup=False, highlight=False)
            return True
        except Exception as error:  # ruff: ignore[blind-except] -- a binding error must not end the shell
            self.error(f"{name}: {error}")
            entry.update(runtime_s=time.perf_counter() - clock, status="error", error=str(error))
            self.log.append(entry)
            return False

        entry.update(runtime_s=time.perf_counter() - clock, status="ok")
        if result is not None:
            entry["result"] = result
        self.log.append(entry)
        return True

    def run_script(self, path: Path) -> bool:
        """Run the commands in a file, one line at a time, stopping at the first failure.

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
        except OSError as error:
            msg_0 = f"cannot read script '{path}': {error.strerror}"
            raise CommandError(msg_0) from error

        self._script_depth += 1
        try:
            return all(self.execute(line) for line in lines)
        finally:
            self._script_depth -= 1

    def close(self) -> None:
        """Write the statistics log and remove the session's temporary files."""
        if self.log_path is not None:
            with self.log_path.open("w", encoding="utf-8") as file:
                json.dump(self.log, file, indent=2, default=str)
                file.write("\n")
        if self._temp_dir is not None:
            shutil.rmtree(self._temp_dir, ignore_errors=True)
            self._temp_dir = None

    def temp_file(self, suffix: str) -> Path:
        """Return a fresh path in the session's temporary directory.

        Args:
            suffix: The file suffix, including the dot.

        Returns:
            A path that does not exist yet and is removed by :meth:`close`.
        """
        if self._temp_dir is None:
            self._temp_dir = Path(tempfile.mkdtemp(prefix="fiction-"))
        index = len(list(self._temp_dir.iterdir()))
        return self._temp_dir / f"{index}{suffix}"

    def error(self, message: str) -> None:
        """Print an error message.

        Args:
            message: The message; printed verbatim.
        """
        self.console.print(f"[red]error:[/] {escape(message)}")

    def info(self, message: str) -> None:
        """Print an informational message.

        Args:
            message: The message; printed verbatim.
        """
        self.console.print(escape(message))

    def status_line(self) -> str:
        """Return the store summary shown below the prompt.

        Returns:
            One entry per store with its element count and the active element's name.
        """
        return " · ".join(
            store.summary() for store in (self.truth_tables, self.networks, self.gate_layouts, self.cell_layouts)
        )

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
