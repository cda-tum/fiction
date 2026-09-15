# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The write_fqca command."""

from __future__ import annotations

import argparse
from typing import TYPE_CHECKING

from mnt.fiction.cli.registry import Category, command
from mnt.pyfiction import qca_layout, stacked_qca_layout, write_fqca_layout, write_fqca_layout_params

from ._write import output_argument, output_path, require_cell_type, written

if TYPE_CHECKING:
    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


def _write_fqca_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    output_argument(parser)
    parser.add_argument(
        "--via-layers", action=argparse.BooleanOptionalAction, default=False, help="add inter-layer via cells"
    )


@command(
    "write_fqca",
    Category.IO,
    _write_fqca_arguments,
    inputs="Active cell-level layout.",
    example="generate mux -b 1; ortho; cell; write_fqca output.fqca",
)
def write_fqca_command(session: Session, args: argparse.Namespace) -> Result:
    """Write the active QCA layout as a QCA-STACK file.

    Without a filename, use the active element's name and ``.fqca``.
    """
    entry = session.cell_layouts.current()
    element = entry.layout
    require_cell_type(element, (qca_layout, stacked_qca_layout), ".fqca")
    path = output_path(element, args.file, ".fqca")
    params = write_fqca_layout_params()
    params.create_inter_layer_via_cells = args.via_layers
    write_fqca_layout(element, str(path), params)
    return written(session, path)
