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
from pathlib import Path
from typing import TYPE_CHECKING

from prompt_toolkit import PromptSession
from prompt_toolkit.completion import Completer, Completion, PathCompleter
from prompt_toolkit.document import Document
from prompt_toolkit.history import FileHistory, History

from mnt.pyfiction import __version__

from .errors import CommandError
from .registry import REGISTRY
from .session import Session

if TYPE_CHECKING:
    from collections.abc import Iterable, Sequence

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
        segment = document.text_before_cursor.rsplit(";", 1)[-1].lstrip()
        words = segment.split()
        word = document.get_word_before_cursor(WORD=True)
        candidates = self._candidates(segment, words, word)
        if candidates is None:
            yield from self._paths.get_completions(Document(word, len(word)), complete_event)
            return
        for candidate in candidates:
            if candidate.startswith(word):
                yield Completion(candidate, start_position=-len(word))

    @staticmethod
    def _candidates(segment: str, words: list[str], word: str) -> Iterable[str] | None:
        """Return what may follow the cursor, or ``None`` when a file path may.

        Args:
            segment: The command being typed, without the ones before it on the line.
            words: The words of that command.
            word: The word before the cursor.

        Returns:
            The command names, option strings, or option values that fit, or ``None``.
        """
        if not words or (len(words) == 1 and not segment.endswith(" ")):
            return sorted(REGISTRY)
        cmd = REGISTRY.get(words[0])
        if cmd is None:
            return None
        if word.startswith("-"):
            return cmd.parser.completions
        # the word before the cursor may be the option whose values are the only ones that fit
        previous = words[-1] if segment.endswith(" ") else (words[-2] if len(words) > 1 else "")
        return cmd.parser.completions.get(previous) or None


def repl(session: Session) -> None:
    """Run the interactive shell until ``quit`` or end of input.

    Args:
        session: The session to run commands in.
    """
    history: History | None
    try:
        history = FileHistory(str(HISTORY_FILE))
    except OSError as error:  # an unwritable home costs the history, not the shell
        session.error(f"cannot use the command history '{HISTORY_FILE}': {error.strerror}")
        history = None
    prompt: PromptSession[str] = PromptSession(
        history=history,
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
            session.quiet = False
            repl(session)
    finally:
        session.close()
    return 0
