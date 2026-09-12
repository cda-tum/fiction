# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The ``fiction`` entry point: one-shot commands, script files, and the interactive shell."""

from __future__ import annotations

import argparse
import importlib
import sys
from pathlib import Path
from typing import TYPE_CHECKING

from prompt_toolkit import PromptSession
from prompt_toolkit.completion import Completer, Completion, PathCompleter
from prompt_toolkit.document import Document
from prompt_toolkit.history import FileHistory, History

from mnt.pyfiction import __version__

from .errors import CommandError
from .registry import REGISTRY
from .session import Session, tokenize

if TYPE_CHECKING:
    from collections.abc import Callable, Iterable, Sequence

    from prompt_toolkit.completion import CompleteEvent

# importing the commands package registers every command with the registry
importlib.import_module(".commands", __package__)

HISTORY_FILE = Path.home() / ".fiction_history"
"""Where the interactive shell keeps the command history between sessions."""

EXIT_FAILURE = 1
"""Exit status when a command fails in one-shot or script mode."""

EXIT_USAGE = 2
"""Exit status when the script file cannot be read."""


class CommandCompleter(Completer):
    """Completes command names, the current command's options, and file paths."""

    def __init__(self) -> None:
        """Create the completer."""
        self._paths = PathCompleter(expanduser=True)

    def get_completions(self, document: Document, complete_event: CompleteEvent) -> Iterable[Completion]:
        """Yield the completions for the word before the cursor.

        Args:
            document: The current input.
            complete_event: Why completion was requested.

        Yields:
            The matching completions.
        """
        candidates: Iterable[str]
        words = tokenize(document.text_before_cursor, incomplete=True)[-1]
        word = words[-1]
        if len(words) == 1:
            candidates = sorted(REGISTRY)
        else:
            cmd = REGISTRY.get(words[0])
            if cmd is None:
                return
            if word.startswith("-"):
                candidates = cmd.options
            elif words[0] == "help":
                candidates = sorted(REGISTRY)
            else:
                yield from self._argument_completions(words, complete_event)
                return
        for candidate in candidates:
            if candidate.startswith(word):
                yield Completion(candidate, start_position=-len(word))

    def _argument_completions(self, words: list[str], event: CompleteEvent) -> Iterable[Completion]:
        """Complete an option value or positional argument from the registered parser.

        Args:
            words: Decoded words of the current command, including the incomplete word.
            event: Completion request passed to the path completer.

        Yields:
            Enum values or paths, restricted to the argument's declared type.
        """
        actions = REGISTRY[words[0]].parser.actions
        action = next((action for action in actions if words[-2] in action.option_strings), None)
        if action is None:
            action = next((action for action in actions if not action.option_strings), None)
        if action is None:
            return
        word = words[-1]
        if action.choices:
            for choice in action.choices:
                candidate = str(choice)
                if candidate.startswith(word):
                    yield Completion(candidate, start_position=-len(word))
        elif action.type is Path:
            yield from self._paths.get_completions(Document(word, len(word)), event)


class ForgivingFileHistory(FileHistory):
    """A file history that gives up on the file instead of ending the shell.

    ``prompt_toolkit`` logs a history it cannot read, but an append that fails leaves ``prompt``
    through an ``OSError``, which would end the session on the first command a user types. A home
    directory that is read-only or full costs the history alone here.

    Attributes:
        report: Called with the first failure; the shell prints it once.
    """

    def __init__(self, filename: str, report: Callable[[str], None]) -> None:
        """Create the history.

        Args:
            filename: The history file.
            report: Called with the message of the first failure.
        """
        super().__init__(filename)
        self.report = report
        self._path = filename
        self._broken = False

    def store_string(self, string: str) -> None:
        """Append one accepted command to the history file, once and if that works at all.

        Args:
            string: The command line to remember.
        """
        if self._broken:
            return
        try:
            super().store_string(string)
        except OSError as error:
            self._broken = True
            self.report(f"cannot write the command history '{self._path}': {error.strerror}")


def _history(session: Session) -> History | None:
    """Return the command history of the interactive shell.

    Args:
        session: The session, for the message when the file cannot be used.

    Returns:
        The history, or ``None`` when even opening the file is impossible.
    """
    try:
        return ForgivingFileHistory(str(HISTORY_FILE), session.error)
    except OSError as error:  # an unusable home costs the history, not the shell
        session.error(f"cannot use the command history '{HISTORY_FILE}': {error.strerror}")
        return None


def repl(session: Session) -> None:
    """Run the interactive shell until ``quit`` or end of input.

    Args:
        session: The session to run commands in.
    """
    prompt: PromptSession[str] = PromptSession(
        history=_history(session),
        completer=CommandCompleter(),
        complete_while_typing=False,
        bottom_toolbar=session.status_line,
    )
    session.console.print(f"[bold]{__version__}[/] · type [bold]help[/] for the list of commands")
    while session.running:
        try:
            line = prompt.prompt("fiction> ")
        except KeyboardInterrupt:
            continue
        except EOFError:
            break
        session.execute(line)


def main(argv: Sequence[str] | None = None) -> int:
    """Run the ``fiction`` command-line interface.

    Args:
        argv: The command-line arguments without the program name; ``sys.argv[1:]`` when ``None``.

    Returns:
        The exit status: ``0`` on success, ``1`` when a command failed, ``2`` when the script file
        cannot be read.
    """
    parser = argparse.ArgumentParser(
        prog="fiction",
        description="Design automation for field-coupled nanotechnologies. Without -c or -f, an "
        "interactive shell starts; -i continues into it afterwards. Type 'help' there for the "
        "list of commands.",
    )
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("-c", "--commands", metavar="COMMANDS", help="run the ';'-separated commands and exit")
    mode.add_argument("-f", "--file", type=Path, metavar="FILE", help="run the commands in FILE and exit")
    parser.add_argument(
        "-i", "--interactive", action="store_true", help="continue into the shell once -c or -f is done"
    )
    parser.add_argument(
        "-l", "--log", type=Path, metavar="FILE", help="write the statistics of every command as JSON to FILE"
    )
    parser.add_argument("-q", "--quiet", action="store_true", help="print only errors and what commands return")
    parser.add_argument("--version", action="version", version=__version__)
    args = parser.parse_args(argv)

    session = Session(log_path=args.log)
    session.quiet = args.quiet
    try:
        status = _run_input(session, args)
    except (UnicodeError, OSError) as error:
        session.error(f"input: {error}")
        status = EXIT_USAGE
    finally:
        session.close()
    return status or (EXIT_FAILURE if session.close_failed else 0)


def _run_input(session: Session, args: argparse.Namespace) -> int:
    """Execute the selected command source and optional interactive continuation.

    Args:
        session: The current stores and output streams.
        args: Parsed application options.

    Returns:
        An exit status for command or input failures.
    """
    if args.commands is not None and not session.execute(args.commands):
        return EXIT_FAILURE
    if args.file is not None:
        try:
            if not session.run_script(args.file):
                return EXIT_FAILURE
        except CommandError as error:
            session.error(str(error))
            return EXIT_USAGE
    if (args.commands is None and args.file is None) or (args.interactive and session.running):
        return _read_stdin(session)
    return 0


def _read_stdin(session: Session) -> int:
    """Read a terminal interactively or execute piped lines without a console session.

    Args:
        session: The session that receives each command.

    Returns:
        Zero on success, or a command failure status.
    """
    if sys.stdin.isatty():
        repl(session)
        return 0
    for line in sys.stdin:
        if not session.execute(line):
            return EXIT_FAILURE
        if not session.running:
            break
    return 0
