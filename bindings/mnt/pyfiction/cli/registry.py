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

import inspect
from dataclasses import dataclass
from enum import Enum
from typing import TYPE_CHECKING, Any

from .errors import CommandError
from .parsing import Parser

if TYPE_CHECKING:
    import argparse
    from collections.abc import Callable

    Result = dict[str, object] | None
    # the first argument is the session; it is not named here so that the session module can import the
    # registry without creating an import cycle
    Runner = Callable[[Any, argparse.Namespace], Result]


class Category(Enum):
    """The groups the ``help`` command lists commands under."""

    IO = "Input and output"
    LOGIC = "Logic"
    PHYSICAL_DESIGN = "Physical design"
    TECHNOLOGY = "Technology"
    SIMULATION = "Simulation"
    VERIFICATION = "Verification"
    GENERAL = "General"


@dataclass(frozen=True)
class Command:
    """One registered command."""

    name: str
    category: Category
    summary: str
    run: Runner
    parser: Parser
    unavailable: str | None = None
    """The missing capability, or None when the command can run."""

    @property
    def options(self) -> tuple[str, ...]:
        """The command's option strings, for tab completion.

        Returns:
            Every option string of the command's parser.
        """
        return tuple(option for action in self.parser.actions for option in action.option_strings)


REGISTRY: dict[str, Command] = {}
"""Every registered command by name, in registration order."""


def command(
    name: str,
    category: Category,
    arguments: Callable[[Parser], None] | None = None,
    *,
    inputs: str = "No store input.",
    example: str | None = None,
    unavailable: str | None = None,
) -> Callable[[Runner], Runner]:
    """Register a function as a shell command.

    The function's docstring becomes the help text and its first line the one-line summary.

    Args:
        name: The command name typed at the prompt.
        category: The group ``help`` lists the command under.
        arguments: Adds the command's options to its parser.
        inputs: The input requirements shown in help.
        example: A shell invocation, or the command name when omitted.
        unavailable: A missing capability reported by help and execution.

    Returns:
        The decorator, which returns the function unchanged.
    """

    def register(run: Runner) -> Runner:
        doc = inspect.getdoc(run) or name
        parser = Parser(name, description=doc, inputs=inputs, example=example or name, unavailable=unavailable)
        if arguments is not None:
            arguments(parser)
        cmd = Command(
            name=name,
            category=category,
            summary=doc.splitlines()[0],
            run=run,
            parser=parser,
            unavailable=unavailable,
        )
        REGISTRY[name] = cmd
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
