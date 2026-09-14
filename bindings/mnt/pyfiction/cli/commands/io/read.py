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

from mnt.pyfiction import (
    read_fqca_layout,
    read_sqd_layout,
    read_stacked_fqca_layout,
)
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import Category, command
from mnt.pyfiction.cli.stores import CellEntry, describe

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.registry import Parser, Result
    from mnt.pyfiction.cli.session import Session
from ._common import AIGVERSE_NETWORK_SUFFIXES, FGL_READERS, FORMATS, _read_network, _topology_argument, _type_argument

NATIVE_NETWORK_SUFFIXES = {suffix for suffix, spec in FORMATS.items() if spec.reader == "native"}
"""The native network reader's suffixes."""


NETWORK_SUFFIXES = NATIVE_NETWORK_SUFFIXES | AIGVERSE_NETWORK_SUFFIXES
"""Every file suffix ``read`` recognizes as a logic network."""


def _read_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument(
        "-F",
        "--format",
        choices=sorted(suffix[1:] for suffix, spec in FORMATS.items() if spec.reader),
        help="select the reader independently of the filename",
    )
    parser.add_argument("path", type=Path, help="the file to read")
    _type_argument(parser)
    _topology_argument(parser)


@command("read", Category.IO, _read_arguments)
def read(session: Session, args: argparse.Namespace) -> Result:
    """Read a logic network or a layout from a file, chosen by its suffix.

    Networks: .v (Verilog), .aig and .aag (AIGER), .blif, and .pla; the type defaults to a
    technology network. Gate-level layouts: .fgl. Cell-level layouts: .sqd (SiDB) and .fqca (QCA).
    The format-specific commands, such as 'read_verilog', take the same file without guessing.
    """
    path: Path = args.path
    if path.is_dir():
        msg = f"'{path}' is a directory; give one file to read"
        raise CommandError(msg)
    if not path.is_file():
        msg = f"no such file: '{path}'"
        raise CommandError(msg)

    suffix = f".{args.format}" if args.format else path.suffix.lower()
    if suffix in NETWORK_SUFFIXES:
        network = _read_network(session, path, args.type, args.format)
        session.networks.add(network)
        return {"network": describe(network)}
    if suffix == ".fgl":
        layout = FGL_READERS[args.topology](str(path), path.stem)
        session.gate_layouts.add(layout)
        return {"gate_layout": describe(layout)}
    if suffix == ".sqd":
        entry = CellEntry(read_sqd_layout(str(path), path.stem))
        session.cell_layouts.add(entry)
        return {"cell_layout": describe(entry)}
    if suffix == ".fqca":
        stacked = read_stacked_fqca_layout(str(path), path.stem)
        # The SVG renderer supports unsigned coordinates, whose layer index is one bit.
        entry = CellEntry(read_fqca_layout(str(path), path.stem) if stacked.z() <= 1 else stacked)
        session.cell_layouts.add(entry)
        return {"cell_layout": describe(entry)}
    msg = f"cannot read '{path.suffix}' files"
    raise CommandError(msg)
