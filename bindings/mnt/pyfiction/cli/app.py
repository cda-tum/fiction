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
from prompt_toolkit.history import FileHistory

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
        if not words or (len(words) == 1 and not segment.endswith(" ")):
            for name in sorted(REGISTRY):
                if name.startswith(word):
                    yield Completion(name, start_position=-len(word))
            return
        cmd = REGISTRY.get(words[0])
        if cmd is not None and word.startswith("-"):
            for option in cmd.options:
                if option.startswith(word):
                    yield Completion(option, start_position=-len(word))
            return
        yield from self._paths.get_completions(Document(word, len(word)), complete_event)


def repl(session: Session) -> None:
    """Run the interactive shell until ``quit`` or end of input.

    Args:
        session: The session to run commands in.
    """
    prompt: PromptSession[str] = PromptSession(
        history=FileHistory(str(HISTORY_FILE)),
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
        "interactive shell starts; type 'help' there for the list of commands.",
    )
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("-c", "--commands", metavar="COMMANDS", help="run the ';'-separated commands and exit")
    mode.add_argument("-f", "--file", type=Path, metavar="FILE", help="run the commands in FILE and exit")
    parser.add_argument(
        "-l", "--log", type=Path, metavar="FILE", help="write the statistics of every command as JSON to FILE"
    )
    parser.add_argument("--version", action="version", version=__version__)
    args = parser.parse_args(argv)

    session = Session(log_path=args.log)
    try:
        if args.commands is not None:
            return 0 if session.execute(args.commands) else EXIT_FAILURE
        if args.file is not None:
            try:
                return 0 if session.run_script(args.file) else EXIT_FAILURE
            except CommandError as error:
                session.error(str(error))
                return EXIT_USAGE
        repl(session)
    finally:
        session.close()
    return 0
