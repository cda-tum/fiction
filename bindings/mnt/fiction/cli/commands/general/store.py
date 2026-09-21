# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The store command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from rich.markup import escape
from rich.table import Table

from mnt.fiction.cli.errors import CommandError
from mnt.fiction.cli.registry import STORE_FLAGS, Category, command, selected_stores, store_flags
from mnt.fiction.cli.render import display_value
from mnt.fiction.cli.stores import Store, describe, one_line

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session

MAX_NAME_WIDTH = 32
"""The widest a name column gets before it is shortened; long names must not push the counts away."""


def _size_of(description: dict[str, object]) -> str:
    """Return the size of an element: its extent for a layout, its node count for a network.

    Args:
        description: A description from :func:`~mnt.fiction.cli.stores.describe`.

    Returns:
        The size as one short string.
    """
    size = description.get("size")
    if isinstance(size, dict):
        extent = " x ".join(str(size[axis]) for axis in ("x", "y", "z") if axis in size)
        if extent:
            return extent
    return ", ".join(f"{description[key]} {key}" for key in ("gates", "cells", "dots", "vars") if key in description)


def _store_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    store_flags(parser)
    parser.add_argument("--pop", action="store_true", help="remove the active element of the selected stores")


@command(
    "store",
    Category.GENERAL,
    _store_arguments,
    inputs="Store elements selected by the flags below.",
    example="store -n -g",
)
def store(session: Session, args: argparse.Namespace) -> Result:
    """List the elements of the selected stores, or of all stores.

    The active element, the one commands work on, is marked with '*'. With --pop, the active
    element of every selected store is removed instead and the one before it becomes active.
    --pop needs the stores spelled out, so that it cannot empty one the user did not mean.
    """
    stores = session.stores
    if args.pop:
        return _pop(session, stores, selected_stores(args))

    listed: dict[str, object] = {}
    for name in selected_stores(args) or list(stores):
        current = stores[name]
        descriptions = [describe(element) for element in current]
        empty = " (empty)" if not descriptions else ""
        session.console.print(f"[bold]{STORE_FLAGS[name][2]}[/]{empty}")
        table = Table(box=None, padding=(0, 1))
        table.add_column("", width=1)
        table.add_column("Index", justify="right", no_wrap=True)
        table.add_column("Name", overflow="ellipsis", max_width=MAX_NAME_WIDTH, no_wrap=True)
        table.add_column("Type", no_wrap=True)
        table.add_column("I/O", no_wrap=True)
        table.add_column("Size", no_wrap=True)
        for index, description in enumerate(descriptions):
            marker = "*" if index == current.active else ""
            label = " / ".join(
                display_value(key, description[key])
                for key in ("type", "technology", "topology", "lattice")
                if description.get(key)
            )
            if description.get("simulation"):
                label += " / simulated"
            io = f"{description['inputs']}/{description['outputs']}" if "inputs" in description else ""
            table.add_row(
                marker,
                str(index + 1),
                escape(str(description.get("name", "—"))),
                escape(label),
                io,
                escape(_size_of(description)),
            )
        if descriptions:
            session.console.print(table)
        listed[name] = descriptions
    return listed


def _pop(session: Session, stores: dict[str, Store], names: list[str]) -> Result:  # type: ignore[type-arg]
    """Remove the active element of every named store.

    Every store is checked before any is touched, so a command that cannot run leaves the session
    exactly as it found it.

    Args:
        session: The session, for the message.
        stores: The stores by their flag name.
        names: The stores the flags selected.

    Returns:
        The removed elements by store.

    Raises:
        CommandError: When no store was selected, or one of them is empty.
    """
    if not names:
        flags = ", ".join(short for short, _, _ in STORE_FLAGS.values())
        msg = f"select the stores to remove from: {flags}"
        raise CommandError(msg)
    empty = [stores[name].kind for name in names if stores[name].active is None]
    if empty:
        msg = f"no {' and no '.join(empty)} in store"
        raise CommandError(msg)
    removed: dict[str, object] = {}
    for name in names:
        description = describe(stores[name].pop())
        removed[name] = description
        session.output(f"removed {one_line(description)}")
    return removed
