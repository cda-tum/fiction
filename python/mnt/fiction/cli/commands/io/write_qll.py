# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The write_qll command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.registry import Category, command
from mnt.pyfiction import inml, mol_qca, qca
from mnt.pyfiction.inml import INMLLayout
from mnt.pyfiction.mol_qca import MolecularQCALayout
from mnt.pyfiction.qca import QCALayout

from ._write import output_argument, output_path, require_cell_type, written

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


@command(
    "write_qll",
    Category.IO,
    output_argument,
    inputs="Active cell-level layout.",
    example="generate mux -b 1; ortho; cell; write_qll output.qll",
    progress=True,
)
def write_qll_command(session: Session, args: argparse.Namespace) -> Result:
    """Write the active QCA, molecular QCA, or iNML layout as QLL.

    Without a filename, use the active element's name and ``.qll``.
    """
    entry = session.cell_layouts.current()
    element = require_cell_type(entry.layout, (QCALayout, MolecularQCALayout, INMLLayout), ".qll")
    path = output_path(element, args.file, ".qll")
    if isinstance(element, QCALayout):
        qca.io.write_qll_layout(element, path, on_progress=session.report_progress)
    elif isinstance(element, MolecularQCALayout):
        mol_qca.io.write_qll_layout(element, path, on_progress=session.report_progress)
    else:
        inml.io.write_qll_layout(element, path, on_progress=session.report_progress)
    return written(session, path)
