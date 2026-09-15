# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The read command."""

from __future__ import annotations

from pathlib import Path
from typing import TYPE_CHECKING

from mnt.fiction.cli.registry import Category, command

from ._common import READ_SUFFIXES, _topology_argument, _type_argument, read_file

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


def _read_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument(
        "-F",
        "--format",
        choices=sorted(suffix[1:] for suffix in READ_SUFFIXES),
        help="select the reader independently of the filename",
    )
    parser.add_argument("path", type=Path, help="the file to read")
    _type_argument(parser)
    _topology_argument(parser)


@command("read", Category.IO, _read_arguments, inputs="One file.", example='read "circuit.v" --type tec')
def read(session: Session, args: argparse.Namespace) -> Result:
    """Read a logic network or a layout from a file, chosen by its suffix.

    Networks: .v (Verilog), .aig and .aag (AIGER), .blif, and .pla; the type defaults to a
    technology network. Gate-level layouts: .fgl. Cell-level layouts: .sqd (SiDB) and .fqca (QCA).
    The format-specific commands, such as 'read_verilog', take the same file without guessing.
    """
    return read_file(session, args.path, file_format=args.format, network_type=args.type, topology=args.topology)
