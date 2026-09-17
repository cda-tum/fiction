# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The read_blif command."""

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
from ._common import _type_argument


def _arguments_read_blif(parser: Parser) -> None:
    """Configure the read_blif arguments.

    Args:
        parser: The command parser.
    """
    parser.add_argument("path", type=Path, help="the file to read")
    _type_argument(parser)


@command("read_blif", Category.IO, _arguments_read_blif, inputs="One file.", example='read_blif "circuit.blif"')
def read_blif(session: Session, args: argparse.Namespace) -> Result:
    """Read a logic network from a BLIF file.

    BLIF is read as a technology network only; --type tec is the only accepted value.
    """
    return read_file(session, args.path, network_type=args.type, suffixes=(".blif",))
