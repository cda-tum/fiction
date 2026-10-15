# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Atomic staging for CLI output files."""

from __future__ import annotations

import contextlib
import json
import subprocess  # ruff: ignore[suspicious-subprocess-import] -- platform viewers use argument lists
import sys
import tempfile
import threading
from collections.abc import Callable, Generator
from contextlib import contextmanager
from pathlib import Path
from typing import Any

from rich.console import Console, RenderableType
from rich.markup import escape
from rich.progress import (
    BarColumn,
    Progress,
    ProgressColumn,
    SpinnerColumn,
    Task,
    TextColumn,
    TimeElapsedColumn,
    TimeRemainingColumn,
)
from rich.table import Column
from rich.text import Text

from .errors import CommandError
from .parsing import tokenize
from .statistics import json_value


@contextmanager
def atomic_output(path: Path) -> Generator[Path, None, None]:
    """Stage an output in a sibling temporary file, then replace the destination.

    The destination changes only when the body completes. Existing symbolic links retain their link
    and receive the output through their target.

    Args:
        path: The destination.

    Yields:
        The temporary path to write.
    """
    destination = path.resolve(strict=True) if path.is_symlink() else path
    with tempfile.TemporaryDirectory(prefix=".fiction-", dir=destination.parent) as directory:
        temporary = Path(directory) / destination.name
        yield temporary  # ruff: ignore[fallible-context-manager] -- a failed body skips the replacement
        if destination.exists():
            temporary.chmod(destination.stat().st_mode)
        temporary.replace(destination)


STYLES = {
    "result": "dim cyan",
    "note": "dim",
}
"""The colors the shell sets command output apart from the prompt with; muted, so a long run of
command output does not read as a wall of color."""

ProgressCallback = Callable[[str, int, int], None]
"""What an algorithm's ``on_progress`` parameter accepts: the task name, the completed count, and the total."""

WorkerProgressCallback = Callable[[int, int, str, int, int, bool], None]
"""A worker ID, worker count, description, completed work, total, and active state."""


def ignore_worker_progress(worker: int, workers: int, description: str, done: int, total: int, active: bool) -> None:  # ruff: ignore[boolean-type-hint-positional-argument] -- native callbacks pass positional arguments
    """Discard worker activity when the session has no progress display."""


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


class CommandProgress:
    """Render aggregate counts and candidate status on one command row."""

    def __init__(self, display: Progress, label: str) -> None:
        """Start the command with an indeterminate spinner."""
        self.display = display
        self.label = label
        self.bar = display.add_task(label, total=None, spinner=True)
        self.phase = ""
        self.completed = 0
        self.total = 0
        self.candidates: dict[int, str] = {}
        self.best_solution = ""
        self.best_cost = float("inf")
        self.lock = threading.Lock()

    def description(self) -> str:
        """Return the current phase and the latest active candidate or best accepted solution."""
        if self.label == "exact":
            candidate = next(reversed(self.candidates.values()), "")
            return f"exact: {candidate}" if candidate else "exact"
        if self.label == "gold":
            return "gold: expansions" + self.best_solution
        return self.phase or self.label

    def report(self, phase: str, done: int, total: int) -> None:
        """Replace phase counts with aggregate algorithm reports, never worker-local counts."""
        with self.lock:
            if phase != self.phase or done < self.completed or (total == 0 and self.total != 0):
                # Rich preserves the old total when reset receives None; replace the single row instead.
                self.display.remove_task(self.bar)
                self.bar = self.display.add_task(self.label, total=total or None)
            self.phase = phase if done or total else ""
            self.completed, self.total = done, total
            self.display.update(
                self.bar,
                description=escape(self.description()),
                completed=done,
                total=total or None,
                spinner=not (done or total),
                refresh=True,
            )

    def report_worker(
        self,
        worker: int,
        _workers: int,
        description: str,
        _done: int,
        _total: int,
        active: bool,  # ruff: ignore[boolean-type-hint-positional-argument] -- native callback arguments are positional
    ) -> None:
        """Use worker reports only for search status; counts come from the aggregate callback."""
        with self.lock:
            if self.label == "exact":
                self.candidates.pop(worker, None)
                if active:
                    self.candidates[worker] = description.partition(": ")[2]
            elif self.label == "gold":
                _, _, best = description.partition("; best ")
                if best:
                    cost = int(best.rsplit(", cost ", 1)[1])
                    if cost < self.best_cost:
                        self.best_cost = cost
                        self.best_solution = "; best " + best
            else:
                return
            self.display.update(self.bar, description=escape(self.description()), spinner=False, refresh=True)


class CommandOutput:
    """Console output, progress, and statistics logging shared by command entry points."""

    def __init__(
        self, console: Console | None = None, log_path: Path | None = None, errors: Console | None = None
    ) -> None:
        """Set output streams and an optional JSON statistics destination."""
        self.console = console if console is not None else Console(highlight=False)
        self.errors = errors if errors is not None else Console(highlight=False, stderr=True)
        self.quiet = False
        self.report_progress: ProgressCallback = ignore_progress
        self.report_worker_progress: WorkerProgressCallback = ignore_worker_progress
        self.log_path = log_path
        self.log: list[dict[str, Any]] = []
        self.close_failed = False

    def close(self) -> None:
        """Write the statistics log and report failures."""
        try:
            if self.log_path is not None:
                with self.log_path.open("w", encoding="utf-8") as file:
                    json.dump(json_value(self.log), file, indent=2, allow_nan=False)
                    file.write("\n")
        except OSError as error:
            self.close_failed = True
            self.error(f"cannot write log '{self.log_path}': {error}")

    @contextlib.contextmanager
    def progress(self, label: str) -> Generator[ProgressCallback, None, None]:
        """Show one row for the command, with aggregate counts and search candidate status.

        The display is transient. Quiet mode and nonterminal output disable the display and
        discard the reports. While the context is open,
        :attr:`report_progress` is the callback that feeds the row, so commands hand it to the
        algorithms they run.

        Args:
            label: The name of the command, shown next to the spinner.

        Yields:
            The callback to hand to an ``on_progress`` parameter.
        """
        if self.quiet or not self.console.is_terminal:
            # older rich versions print a newline when a disabled display stops, so start none at all
            previous = self.report_progress
            previous_worker = self.report_worker_progress
            self.report_progress = ignore_progress
            self.report_worker_progress = ignore_worker_progress
            try:
                yield ignore_progress
            finally:
                self.report_progress = previous
                self.report_worker_progress = previous_worker
            return
        display = Progress(
            SpinnerColumn(),
            TextColumn(
                "[progress.description]{task.description}", table_column=Column(no_wrap=True, overflow="ellipsis")
            ),
            TaskBarColumn(),
            CountColumn(),
            TimeElapsedColumn(),
            RemainingColumn(),
            console=self.console,
            transient=True,
        )
        previous = self.report_progress
        previous_worker = self.report_worker_progress
        try:
            with display:
                reporter = CommandProgress(display, label)
                self.report_progress = reporter.report
                self.report_worker_progress = reporter.report_worker
                yield self.report_progress
        finally:
            self.report_progress = previous
            self.report_worker_progress = previous_worker

    def error(self, message: str) -> None:
        """Print an error message.

        Args:
            message: The message; printed verbatim.
        """
        self.errors.print(f"[red]error:[/] {escape(message)}", highlight=False)

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


def viewer_command(path: Path, program: str | None) -> list[str]:
    """Return the command that opens a file in a viewer.

    Args:
        path: The file to open.
        program: An explicit command; ``{}`` in it is replaced by the file, otherwise the file is
            appended. The platform's own opener is used when this is ``None``.

    Returns:
        The command and its arguments.

    Raises:
        CommandError: An explicit viewer contains multiple executable commands.
    """
    if program is not None:
        commands = tokenize(program)
        if len(commands) != 1:
            msg = "the viewer must be one executable command"
            raise CommandError(msg)
        words = commands[0]
        if any("{}" in word for word in words):
            return [word.replace("{}", str(path)) for word in words]
        return [*words, str(path)]
    if sys.platform == "darwin":
        return ["open", str(path)]
    return ["xdg-open", str(path)]


def open_viewer(path: Path, program: str | None = None) -> None:
    """Open a drawing in a viewer, without waiting for it.

    A Graphviz DOT file is not a web page, so the platform's file association decides what opens it,
    exactly as the C++ shell did.

    Args:
        path: The file to open.
        program: An explicit viewer command; the platform's own opener when ``None``.

    Raises:
        CommandError: When the viewer cannot be started.
    """
    if program is None and sys.platform == "win32":
        # Windows has no opener binary; the shell association is reached through the API
        import os  # ruff: ignore[import-outside-top-level] -- os.startfile exists on Windows alone

        os.startfile(path)  # ruff: ignore[start-process-with-no-shell]
        return
    command = viewer_command(path, program)
    try:
        subprocess.Popen(command, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)  # ruff: ignore[subprocess-without-shell-equals-true]
    except OSError as error:
        msg = f"cannot open '{path}' with '{command[0]}': {error.strerror}"
        raise CommandError(msg) from error
