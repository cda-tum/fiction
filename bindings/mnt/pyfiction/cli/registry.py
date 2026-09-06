# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The command registry of the ``fiction`` command-line interface.

A command is a function that takes the :class:`~mnt.pyfiction.cli.session.Session` and its parsed
arguments and returns the statistics to log. The :func:`command` decorator builds the command's
argument parser and registers it under its name.
"""

from __future__ import annotations

import argparse
import inspect
from dataclasses import dataclass
from enum import Enum
from typing import TYPE_CHECKING, Any, NoReturn

from .errors import CommandError, HelpRequested

if TYPE_CHECKING:
    from collections.abc import Callable

    from _typeshed import SupportsWrite

    Result = dict[str, object] | None
    # the first argument is the session; it is not named here so that the session module can import the
    # registry without creating an import cycle
    Runner = Callable[[Any, argparse.Namespace], Result]


class Category(Enum):
    """The groups the ``help`` command lists commands under."""

    GENERAL = "General"
    IO = "Input and output"
    LOGIC = "Logic"
    PHYSICAL_DESIGN = "Physical design"
    TECHNOLOGY = "Technology"
    SIMULATION = "Simulation"
    VERIFICATION = "Verification"


class Parser(argparse.ArgumentParser):
    """An argument parser that reports through exceptions instead of exiting the process."""

    def __init__(self, name: str, description: str) -> None:
        """Create the parser of one command.

        Args:
            name: The command name, shown as the program name in usage lines.
            description: The command's docstring, shown by ``-h``.
        """
        super().__init__(
            prog=name,
            description=description,
            allow_abbrev=False,
            formatter_class=argparse.RawDescriptionHelpFormatter,
        )

    def error(self, message: str) -> NoReturn:
        """Turn a usage error into a :class:`CommandError` that carries the usage line.

        Args:
            message: The error message ``argparse`` produced.

        Raises:
            CommandError: Always.
        """
        msg = f"{message}\n{self.format_usage().rstrip()}"
        raise CommandError(msg)

    # an override of argparse's method, which does not need the parser either
    def exit(self, status: int = 0, message: str | None = None) -> NoReturn:  # ruff: ignore[no-self-use]
        """Never exit the process.

        Args:
            status: The exit status ``argparse`` intended.
            message: The message ``argparse`` intended to print.

        Raises:
            CommandError: When the status signals an error.
            HelpRequested: Otherwise.
        """
        if status:
            raise CommandError(message or "invalid arguments")
        raise HelpRequested(message or "")

    def print_help(self, file: SupportsWrite[str] | None = None) -> None:
        """Hand the help text to the session instead of writing it to a stream.

        Args:
            file: Ignored; kept for the ``argparse`` signature.

        Raises:
            HelpRequested: Always, carrying the formatted help.
        """
        del file
        raise HelpRequested(self.format_help())


@dataclass(frozen=True)
class Command:
    """One registered command."""

    name: str
    category: Category
    summary: str
    run: Runner
    parser: Parser
    options: tuple[str, ...]
    """The option strings, for tab completion."""


REGISTRY: dict[str, Command] = {}
"""Every registered command by name, in registration order."""


def command(
    name: str,
    category: Category,
    arguments: Callable[[Parser], None] | None = None,
) -> Callable[[Runner], Runner]:
    """Register a function as a shell command.

    The function's docstring becomes the help text and its first line the one-line summary.

    Args:
        name: The command name typed at the prompt.
        category: The group ``help`` lists the command under.
        arguments: Adds the command's options to its parser.

    Returns:
        The decorator, which returns the function unchanged.
    """

    def register(run: Runner) -> Runner:
        doc = inspect.getdoc(run) or name
        parser = Parser(name, description=doc)
        if arguments is not None:
            arguments(parser)
        # argparse offers no public enumeration of the registered options
        options = tuple(option for action in parser._actions for option in action.option_strings)  # ruff: ignore[private-member-access]
        REGISTRY[name] = Command(
            name=name,
            category=category,
            summary=doc.splitlines()[0],
            run=run,
            parser=parser,
            options=options,
        )
        return run

    return register


STORE_FLAGS: dict[str, tuple[str, str, str]] = {
    "truth_table": ("-t", "--truth-table", "truth tables"),
    "network": ("-n", "--network", "logic networks"),
    "gate_layout": ("-g", "--gate-layout", "gate-level layouts"),
    "cell_layout": ("-c", "--cell-layout", "cell-level layouts"),
}
"""The short flag, long flag, and help label of every store."""


def store_flags(parser: Parser, *stores: str) -> None:
    """Add the store selection flags ``-t``, ``-n``, ``-g``, and ``-c`` to a parser.

    Args:
        parser: The command's parser.
        stores: The stores to add flags for, as keys of :data:`STORE_FLAGS`; all four when empty.
    """
    for store in stores or tuple(STORE_FLAGS):
        short, long, label = STORE_FLAGS[store]
        parser.add_argument(short, long, action="store_true", help=f"the {label} store")


def selected_stores(args: argparse.Namespace) -> list[str]:
    """Return the store flags set on parsed arguments, in the order of :data:`STORE_FLAGS`.

    Args:
        args: The parsed arguments of a command created with :func:`store_flags`.

    Returns:
        The selected store names.
    """
    return [store for store in STORE_FLAGS if getattr(args, store, False)]


def one_store(args: argparse.Namespace, *stores: str) -> str:
    """Return the single store flag set on parsed arguments.

    Args:
        args: The parsed arguments of a command created with :func:`store_flags`.
        stores: The stores the command accepts; all four when empty.

    Returns:
        The selected store name.

    Raises:
        CommandError: When no flag or more than one flag is set.
    """
    accepted = stores or tuple(STORE_FLAGS)
    selected = [store for store in selected_stores(args) if store in accepted]
    if len(selected) != 1:
        flags = ", ".join(STORE_FLAGS[store][0] for store in accepted)
        msg = f"select exactly one store: {flags}"
        raise CommandError(msg)
    return selected[0]
