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

    IO = "Input and output"
    LOGIC = "Logic"
    PHYSICAL_DESIGN = "Physical design"
    TECHNOLOGY = "Technology"
    SIMULATION = "Simulation"
    VERIFICATION = "Verification"
    GENERAL = "General"


class Group:
    """An argument group that records the options it adds on the parser that owns it."""

    def __init__(self, parser: Parser, group: argparse._ArgumentGroup) -> None:
        """Wrap one ``argparse`` group.

        Args:
            parser: The parser that records the options.
            group: The group to add the arguments to.
        """
        self._parser = parser
        self._group = group

    def add_argument(self, *names: str, **kwargs: Any) -> argparse.Action:  # ruff: ignore[any-type]
        """Add an argument to the group and record it on the parser.

        Args:
            names: The option strings, or the name of a positional argument.
            kwargs: The keyword arguments of ``argparse.ArgumentParser.add_argument``.

        Returns:
            The created action.
        """
        action = self._group.add_argument(*names, **kwargs)
        self._parser.record(action)
        return action


class Parser(argparse.ArgumentParser):
    """An argument parser that reports through exceptions instead of exiting the process.

    Attributes:
        completions: Every option string of the command, mapped onto the values it accepts.
    """

    def __init__(self, name: str, description: str) -> None:
        """Create the parser of one command.

        Args:
            name: The command name, shown as the program name in usage lines.
            description: The command's docstring, shown by ``-h``.
        """
        self.completions: dict[str, tuple[str, ...]] = {}
        super().__init__(
            prog=name,
            description=description,
            allow_abbrev=False,
            formatter_class=argparse.RawDescriptionHelpFormatter,
        )

    def record(self, action: argparse.Action) -> None:
        """Remember an action's option strings and the values they accept, for tab completion.

        Args:
            action: The action ``add_argument`` created.
        """
        values = tuple(str(choice) for choice in action.choices) if action.choices else ()
        for option in action.option_strings:
            self.completions[option] = values

    def add_argument(self, *names: str, **kwargs: Any) -> argparse.Action:  # ruff: ignore[any-type]
        """Add an argument and record it for tab completion.

        Args:
            names: The option strings, or the name of a positional argument.
            kwargs: The keyword arguments of ``argparse.ArgumentParser.add_argument``.

        Returns:
            The created action.
        """
        action = super().add_argument(*names, **kwargs)
        self.record(action)
        return action

    def group(self, title: str) -> Group:
        """Create an argument group whose options this parser records.

        ``add_argument_group`` itself is left alone, because ``argparse`` builds its own groups with
        it while constructing and those must stay the plain ones.

        Args:
            title: The heading the help text shows the group under.

        Returns:
            The group.
        """
        return Group(self, self.add_argument_group(title))

    def exclusive_group(self, *, required: bool = False) -> Group:
        """Create a mutually exclusive group whose options this parser records.

        Args:
            required: Whether one of the group's options has to be given.

        Returns:
            The group.
        """
        return Group(self, self.add_mutually_exclusive_group(required=required))

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
    aliases: tuple[str, ...] = ()
    """Other names the command also answers to; ``help`` lists it under ``name`` alone."""

    @property
    def options(self) -> tuple[str, ...]:
        """The command's option strings, for tab completion.

        Returns:
            Every option string of the command's parser.
        """
        return tuple(self.parser.completions)


REGISTRY: dict[str, Command] = {}
"""Every registered command by name, in registration order."""


def command(
    name: str,
    category: Category,
    arguments: Callable[[Parser], None] | None = None,
    aliases: tuple[str, ...] = (),
) -> Callable[[Runner], Runner]:
    """Register a function as a shell command.

    The function's docstring becomes the help text and its first line the one-line summary.

    Args:
        name: The command name typed at the prompt.
        category: The group ``help`` lists the command under.
        arguments: Adds the command's options to its parser.
        aliases: Other names the command answers to; ``help`` lists only ``name``.

    Returns:
        The decorator, which returns the function unchanged.
    """

    def register(run: Runner) -> Runner:
        doc = inspect.getdoc(run) or name
        parser = Parser(name, description=doc)
        if arguments is not None:
            arguments(parser)
        cmd = Command(
            name=name,
            category=category,
            summary=doc.splitlines()[0],
            run=run,
            parser=parser,
            aliases=aliases,
        )
        REGISTRY[name] = cmd
        for alias in aliases:
            REGISTRY[alias] = cmd
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
