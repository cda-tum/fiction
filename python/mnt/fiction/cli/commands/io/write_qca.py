# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The write_qca command."""

from __future__ import annotations

import argparse
from typing import TYPE_CHECKING

from mnt.fiction.cli.registry import Category, command
from mnt.pyfiction.qca import qca_layout
from mnt.pyfiction.qca.io import write_qca_layout, write_qca_layout_params

from ._write import output_argument, output_path, require_cell_type, written

if TYPE_CHECKING:
    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


def _write_qca_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    output_argument(parser)
    parser.add_argument(
        "--via-layers", action=argparse.BooleanOptionalAction, default=True, help="add inter-layer via cells"
    )


@command(
    "write_qca",
    Category.IO,
    _write_qca_arguments,
    inputs="Active cell-level layout.",
    example="generate mux -b 1; ortho; cell; write_qca output.qca",
    progress=True,
)
def write_qca_command(session: Session, args: argparse.Namespace) -> Result:
    """Write the active QCA layout as a QCADesigner file.

    Without a filename, use the active element's name and ``.qca``.
    """
    entry = session.cell_layouts.current()
    element = require_cell_type(entry.layout, (qca_layout,), ".qca")
    path = output_path(element, args.file, ".qca")
    params = write_qca_layout_params()
    params.on_progress = session.report_progress
    params.create_inter_layer_via_cells = args.via_layers
    write_qca_layout(element, str(path), params)
    return written(session, path)
