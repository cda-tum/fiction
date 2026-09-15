# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The ps command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.registry import STORE_FLAGS, Category, command, one_store, selected_stores, store_flags
from mnt.fiction.cli.render import summary as render_summary
from mnt.fiction.cli.stores import describe

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


def _ps_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    store_flags(parser)
    parser.add_argument("--all", action="store_true", help="describe every element, not only the active one")


@command("ps", Category.GENERAL, _ps_arguments, inputs="Store elements selected by the flags below.", example="ps -n")
def ps(session: Session, args: argparse.Namespace) -> Result:
    """Print the statistics of the active element of a store."""
    stores = session.stores
    if not args.all:
        name = one_store(args)
        description = describe(stores[name].current())
        session.console.print(render_summary(description))
        return {name: description}

    listed: dict[str, object] = {}
    for name in selected_stores(args) or list(stores):
        current_store = stores[name]
        descriptions = [describe(element) for element in current_store]
        for index, description in enumerate(descriptions):
            marker = "*" if index == current_store.active else " "
            session.console.print(f"[bold]{STORE_FLAGS[name][2]}[/] {marker}{index + 1}")
            session.console.print(render_summary(description))
        listed[name] = descriptions
    return listed
