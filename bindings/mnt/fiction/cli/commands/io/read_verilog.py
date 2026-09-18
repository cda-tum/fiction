# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The read_verilog command."""

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


def _arguments_read_verilog(parser: Parser) -> None:
    """Configure the read_verilog arguments.

    Args:
        parser: The command parser.
    """
    parser.add_argument("path", type=Path, help="the file to read")
    _type_argument(parser)


@command(
    "read_verilog",
    Category.IO,
    _arguments_read_verilog,
    inputs="One file.",
    example='read_verilog "circuit.v" --type tec',
)
def read_verilog(session: Session, args: argparse.Namespace) -> Result:
    """Read a logic network from a Verilog file.

    The network type follows --type and defaults to a technology network.
    """
    return read_file(session, args.path, network_type=args.type, suffixes=(".v",))
