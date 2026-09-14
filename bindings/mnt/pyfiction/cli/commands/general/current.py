# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The current command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction.cli.parsing import integer
from mnt.pyfiction.cli.registry import Category, command, one_store, store_flags
from mnt.pyfiction.cli.stores import describe

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.parsing import Parser
    from mnt.pyfiction.cli.registry import Result
    from mnt.pyfiction.cli.session import Session


def _current_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    store_flags(parser)
    parser.add_argument("position", type=integer, help="the element's position, counting from 1 as 'store' lists it")


@command(
    "current",
    Category.GENERAL,
    _current_arguments,
    inputs="Store elements selected by the flags below.",
    example="current -n 1",
)
def current(session: Session, args: argparse.Namespace) -> Result:
    """Make an element of a store the active one."""
    name = one_store(args)
    selected = session.stores[name]
    selected.select(args.position)
    return {name: describe(selected.current())}
