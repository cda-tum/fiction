# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The write_qcc command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.registry import Category, command
from mnt.pyfiction import inml_layout, write_qcc_layout, write_qcc_layout_params

from ._write import output_argument, output_path, require_cell_type, written

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


def _write_qcc_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    output_argument(parser)
    parser.add_argument("--component-name", action="store_true", help="use the file name as the component identifier")


@command(
    "write_qcc",
    Category.IO,
    _write_qcc_arguments,
    inputs="Active cell-level layout.",
    example="read_fgl layout.fgl; cell --library topolinano; write_qcc output.qcc",
    progress=True,
)
def write_qcc_command(session: Session, args: argparse.Namespace) -> Result:
    """Write the active iNML layout as a ToPoliNano QCC file.

    Without a filename, use the active element's name and ``.qcc``.
    """
    entry = session.cell_layouts.current()
    element = entry.layout
    require_cell_type(element, (inml_layout,), ".qcc")
    path = output_path(element, args.file, ".qcc")
    params = write_qcc_layout_params()
    params.on_progress = session.report_progress
    params.use_filename_as_component_name = args.component_name
    write_qcc_layout(element, str(path), params)
    return written(session, path)
