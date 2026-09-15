# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The read_fgl command."""

from __future__ import annotations

from pathlib import Path
from typing import TYPE_CHECKING

from mnt.fiction.cli.registry import Category, command

from ._common import read_file

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session
from ._common import _topology_argument


def _read_fgl_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument("path", type=Path, help="the file to read")
    _topology_argument(parser)


@command(
    "read_fgl",
    Category.IO,
    _read_fgl_arguments,
    inputs="One file.",
    example='read_fgl "layout.fgl" --topology cartesian',
)
def read_fgl(session: Session, args: argparse.Namespace) -> Result:
    """Read a gate-level layout from an FGL file.

    The file does not record its topology, so --topology selects the reader; it defaults to
    cartesian.
    """
    return read_file(session, args.path, topology=args.topology, suffixes=(".fgl",))
