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
import os
import shutil
import tempfile
import time
from collections.abc import Callable
from pathlib import Path
from typing import TYPE_CHECKING, Any

from rich.cells import cell_len
from rich.console import Console, RenderableType
from rich.markup import escape
from rich.progress import (
    BarColumn,
    Progress,
    ProgressColumn,
    SpinnerColumn,
    Task,
    TaskID,
    TextColumn,
    TimeElapsedColumn,
    TimeRemainingColumn,
)
from rich.text import Text

from mnt.pyfiction import convert_network, technology_network

from .errors import CommandError, HelpRequested
from .parsing import tokenize
from .registry import REGISTRY
from .statistics import json_value
from .stores import CellEntry, GateLayout, Network, Store, describe, element_name, one_line

if TYPE_CHECKING:
    from collections.abc import Iterator

    from mnt.pyfiction import dynamic_truth_table

    from .registry import Result

STYLES = {
    "result": "dim cyan",
    "note": "dim",
}
"""The colors the shell sets command output apart from the prompt with; muted, so a long run of
command output does not read as a wall of color."""


ProgressCallback = Callable[[str, int, int], None]
"""What an algorithm's ``on_progress`` parameter accepts: the task name, the completed count, and the total."""


def ignore_progress(task: str, done: int, total: int) -> None:
    """Discard a progress report; the callback in effect while no command runs.

    Args:
        task: The name of the task the algorithm works on.
        done: The number of completed work items.
        total: The number of work items, or ``0`` if unknown.
    """


class TaskBarColumn(BarColumn):
    """Render the bar of a task, and nothing for a spinner."""

    def __call__(self, task: Task) -> RenderableType:
        """Render the bar of a task.

        Args:
            task: The task to render.

        Returns:
            The bar, or an empty text for the spinner.
        """
        if task.fields.get("spinner"):
            return Text("")
        return super().__call__(task)


class CountColumn(ProgressColumn):
    """Render ``done/total`` of a task, only ``done`` while the total is unknown, and nothing for a spinner."""

    def render(self, task: Task) -> Text:  # ruff: ignore[no-self-use] -- rich calls the column's render method
        """Render the count of a task.

        Args:
            task: The task to render.

        Returns:
            The count, right-aligned.
        """
        if task.fields.get("spinner"):
            return Text("")
        completed = int(task.completed)
        if task.total is None:
            return Text(f"{completed}", style="progress.download")
        return Text(f"{completed}/{int(task.total)}", style="progress.download")


class RemainingColumn(TimeRemainingColumn):
    """Render the time remaining of a task, and nothing while its total is unknown."""

    def render(self, task: Task) -> Text:
        """Render the time remaining of a task.

        Args:
            task: The task to render.

        Returns:
            The estimate, or an empty text without a total.
        """
        if task.total is None:
            return Text("")
        return super().render(task)


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
        report_progress: The callback that displays algorithm progress while a command runs.
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
        self.report_progress: ProgressCallback = ignore_progress
        self.log_path = log_path
        self.log: list[dict[str, object]] = []
        self._source: tuple[str, int] | None = None
        self.close_failed = False
        self._temp_dir: Path | None = None
        self._temp_index = 0
        self._to_delete: list[Path] = []

    @property
    def stores(self) -> dict[str, Store[Any]]:
        """The stores keyed by their command-line flag names."""
        return {
            "truth_table": self.truth_tables,
            "network": self.networks,
            "gate_layout": self.gate_layouts,
            "cell_layout": self.cell_layouts,
        }

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
        stores = self.stores
        before = {name: (len(store), store.active) for name, store in stores.items()}
        try:
            result = self._invoke(argv, entry)
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
        entry["status"] = "ok"
        if result is not None and self.log_path is not None:
            entry["result"] = json_value(result)
        for name, store in stores.items():
            previous = before[name]
            if store.active is not None and (len(store), store.active) != previous:
                description = result.get(name) if result and len(store) >= previous[0] else None
                if not isinstance(description, dict):
                    description = describe(store.current())
                self.info(f"{store.kind} {store.position}: {one_line(description)}", style="result")
        return True

    def _invoke(self, argv: list[str], entry: dict[str, object]) -> Result:
        """Validate and invoke one registered command.

        Args:
            argv: Command name and arguments.
            entry: Log entry that receives parsed options.

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
        entry["args"] = {key: json_value(value) for key, value in vars(args).items()}
        if cmd.unavailable is not None:
            raise CommandError(cmd.unavailable)
        with self.progress(name):
            return cmd.run(self, args)

    def run_script(self, path: Path) -> bool:
        """Run the commands in a file, one line at a time, stopping at the first failure or at ``quit``.

        Args:
            path: The script file.

        Returns:
            ``True`` when every command succeeded.

        Raises:
            CommandError: When the file cannot be read.
        """
        try:
            lines = path.read_text(encoding="utf-8").splitlines()
        except (OSError, UnicodeError) as error:
            if isinstance(error, UnicodeDecodeError):
                line_number = error.object[: error.start].count(b"\n") + 1
                msg_0 = f"{path}:{line_number}: script must be UTF-8"
                raise CommandError(msg_0) from error
            msg_0 = f"cannot read script '{path}': {error}"
            raise CommandError(msg_0) from error

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

    @contextlib.contextmanager
    def progress(self, label: str) -> Iterator[ProgressCallback]:
        """Show a spinner for a running command and a bar for every task its algorithms report.

        The display is transient. Quiet mode and nonterminal output disable the display and
        discard the reports. While the context is open,
        :attr:`report_progress` is the callback that feeds the bars, so commands hand it to the
        algorithms they run.

        Args:
            label: The name of the command, shown next to the spinner.

        Yields:
            The callback to hand to an ``on_progress`` parameter.
        """
        if self.quiet or not self.console.is_terminal:
            # older rich versions print a newline when a disabled display stops, so start none at all
            yield ignore_progress
            return

        display = Progress(
            SpinnerColumn(),
            TextColumn("[progress.description]{task.description}"),
            TaskBarColumn(),
            CountColumn(),
            TimeElapsedColumn(),
            RemainingColumn(),
            console=self.console,
            transient=True,
        )
        bars: dict[str, TaskID] = {}
        counts: dict[str, int] = {}

        def report(task: str, done: int, total: int) -> None:
            bar = bars.get(task)
            if bar is None:
                bar = bars[task] = display.add_task(task, total=total or None)
            elif done < counts[task]:
                # the algorithm restarted the task, e.g., for another optimization pass
                display.reset(bar, total=total or None)
            counts[task] = done
            display.update(bar, completed=done, total=total or None, refresh=True)

        previous = self.report_progress
        self.report_progress = report
        try:
            with display:
                display.add_task(label, total=None, spinner=True)
                yield report
        finally:
            self.report_progress = previous

    def error(self, message: str) -> None:
        """Print an error message.

        Args:
            message: The message; printed verbatim.
        """
        context = f"{self._source[0]}:{self._source[1]}: " if self._source else ""
        self.errors.print(f"[red]error:[/] {escape(context + message)}", highlight=False)

    def info(self, message: str, style: str | None = None) -> None:
        """Print an informational message, unless the session is quiet.

        Args:
            message: The message; printed verbatim.
            style: A key of :data:`STYLES`, to set the message apart from the prompt.
        """
        if not self.quiet:
            self.output(message, style=style)

    def output(self, message: str, style: str | None = None) -> None:
        """Print an explicitly requested result, including in quiet mode.

        A drawing that carries its own ANSI colors keeps them; ``style`` applies to plain text only,
        so that a colored layout picture is never repainted in one color.

        Args:
            message: Plain text or a native ANSI drawing.
            style: A key of :data:`STYLES`, or ``None`` for the terminal's default.
        """
        text = Text.from_ansi(message)
        if style is not None and "\x1b" not in message:
            self.console.print(text, highlight=False, style=STYLES[style])
        else:
            self.console.print(text, highlight=False)

    def status_line(self) -> str:
        """Return the store counts, the active element, and its name, within the terminal width.

        Positions count from 1, the way ``store`` lists them and ``current`` accepts them, so the
        position never looks like it disagrees with the count.

        Returns:
            One line naming each store that holds something, with names as space permits.
        """
        stores = tuple(self.stores.values())
        labels = ("truth tables", "networks", "gate layouts", "cell layouts")
        parts = [
            f"{label} {store.position} of {len(store)}"
            for label, store in zip(labels, stores, strict=False)
            if len(store)
        ]
        shown = [index for index, store in enumerate(stores) if len(store)]
        width = self.console.width
        remaining = width - cell_len("   ".join(parts))
        for slot, index in enumerate(shown):
            store = stores[index]
            if store.active is not None and remaining > len(" · "):
                name = Text(" ".join(element_name(store.current()).split()))
                name.truncate(min(20, remaining - 3), overflow="ellipsis")
                if name.plain:
                    parts[slot] += " · " + name.plain
                    remaining -= name.cell_len + 3
        text = Text("   ".join(parts) if parts else "no elements in store")
        text.truncate(width, overflow="ellipsis")
        return text.plain

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
