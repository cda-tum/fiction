# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The read_aiger command."""

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


def _arguments_read_aiger(parser: Parser) -> None:
    """Configure the read_aiger arguments.

    Args:
        parser: The command parser.
    """
    parser.add_argument("path", type=Path, help="the file to read")
    _type_argument(parser)


@command("read_aiger", Category.IO, _arguments_read_aiger, inputs="One file.", example='read_aiger "circuit.aig"')
def read_aiger(session: Session, args: argparse.Namespace) -> Result:
    """Read a logic network from a binary (.aig) or ASCII (.aag) AIGER file.

    ASCII AIGER is read through aigverse as an AIG and converted to --type from there.
    """
    return read_file(session, args.path, network_type=args.type, suffixes=(".aig", ".aag"))
