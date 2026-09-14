# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The help command."""

from __future__ import annotations

import sys
from contextlib import nullcontext
from typing import TYPE_CHECKING

from rich.markup import escape
from rich.table import Table

from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import (
    REGISTRY,
    Category,
    command,
    unavailable_reason,
)

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.registry import Parser, Result
    from mnt.pyfiction.cli.session import Session


def _help_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument("--all", action="store_true", help="list command descriptions; page in interactive terminals")
    parser.add_argument("name", nargs="?", help="a command to show the full help of")


@command("help", Category.GENERAL, _help_arguments)
def help_command(session: Session, args: argparse.Namespace) -> Result:
    """List the commands, or show one command's options.

    Every command accepts -h for its full help.
    """
    if args.name is not None:
        cmd = REGISTRY.get(args.name)
        if cmd is None:
            msg = f"unknown command '{args.name}'"
            raise CommandError(msg)
        session.console.print(cmd.parser.format_help(), markup=False, highlight=False)
        return None

    pager = (
        session.console.pager() if args.all and session.console.is_terminal and sys.stdin.isatty() else nullcontext()
    )
    with pager:
        session.console.print(_command_grid(described=args.all))
        session.output("\nExample: read circuit.v; ortho; check; cell; write circuit.qca", style="note")
        session.output("Use help COMMAND for options and restrictions; help --all for descriptions.", style="note")
        for name in ("exact", "clustercomplete"):
            if reason := unavailable_reason(name):
                session.output(f"{name}: {reason}", style="note")
    return None


NAME_COLUMNS = 4
"""How many command names one row of the ``help`` grid holds."""


def _command_grid(*, described: bool) -> Table:
    """Lay every category out in one grid, so the columns align across the whole listing.

    Args:
        described: List each command with its summary instead of packing names into columns.

    Returns:
        The grid, ready to print on a console.
    """
    grid = Table(box=None, show_header=False, padding=(0, 1), pad_edge=False)
    grid.add_column("", no_wrap=True)
    if described:
        grid.add_column("", overflow="fold")
    else:
        for _ in range(NAME_COLUMNS - 1):
            grid.add_column("", no_wrap=True)
    blanks = [""] * (len(grid.columns) - 1)
    for category in Category:
        # a command registered under an alias appears in the registry twice; list it once
        commands = [cmd for name, cmd in REGISTRY.items() if cmd.category is category and cmd.name == name]
        if not commands:
            continue
        if grid.row_count:
            grid.add_row("", *blanks)
        grid.add_row(f"[bold]{category.value}[/]", *blanks)
        if described:
            for cmd in commands:
                grid.add_row(f"  {cmd.name}", escape(cmd.summary))
        else:
            _add_name_rows(grid, [name for cmd in commands for name in (cmd.name, *cmd.aliases)])
    return grid


def _add_name_rows(grid: Table, names: list[str]) -> None:
    """Append the command names of one category, wrapped across the grid's columns.

    Args:
        grid: The grid being built.
        names: The command names, aliases included.
    """
    for start in range(0, len(names), NAME_COLUMNS):
        row = names[start : start + NAME_COLUMNS]
        cells = [*row, *[""] * (NAME_COLUMNS - len(row))]
        cells[0] = f"  {cells[0]}"
        grid.add_row(*cells)
