# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The write_svg command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.drawing import validate_drawing_options, write_svg
from mnt.fiction.cli.registry import Category, command
from mnt.pyfiction import mol_qca_layout, qca_layout, sidb_layout

from ._write import output_argument, output_path, require_cell_type, written

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


def _write_svg_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    output_argument(parser)
    parser.add_argument("--simple", action="store_true", help="draw QCA cells without dots and clock numbers")


@command(
    "write_svg",
    Category.IO,
    _write_svg_arguments,
    inputs="Active cell-level layout.",
    example="generate mux -b 1; ortho; cell; write_svg output.svg",
)
def write_svg_command(session: Session, args: argparse.Namespace) -> Result:
    """Write the active QCA, molecular QCA, or SiDB layout as SVG.

    Without a filename, use the active element's name and ``.svg``.
    """
    entry = session.cell_layouts.current()
    element = entry.layout
    require_cell_type(element, (qca_layout, mol_qca_layout, sidb_layout), ".svg")
    validate_drawing_options(
        args, dot=False, gate_layout=False, qca_svg=isinstance(element, qca_layout | mol_qca_layout)
    )
    path = output_path(element, args.file, ".svg")
    write_svg(entry, path, simple=args.simple)
    return written(session, path)
