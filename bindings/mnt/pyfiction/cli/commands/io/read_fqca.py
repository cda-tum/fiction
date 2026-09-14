# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The read_fqca command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction import (
    read_fqca_layout,
    read_stacked_fqca_layout,
)
from mnt.pyfiction.cli.registry import Category, command
from mnt.pyfiction.cli.stores import CellEntry, describe

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.registry import Result
    from mnt.pyfiction.cli.session import Session
from ._common import _existing_file, _read_path_argument


@command("read_fqca", Category.IO, _read_path_argument)
def read_fqca(session: Session, args: argparse.Namespace) -> Result:
    """Read a QCA cell-level layout from a QCA-STACK file.

    A file of more than two layers is kept stacked, which the SVG renderer cannot draw.
    """
    path = _existing_file(args.path, (".fqca",))
    stacked = read_stacked_fqca_layout(str(path), path.stem)
    # The SVG renderer supports unsigned coordinates, whose layer index is one bit.
    entry = CellEntry(read_fqca_layout(str(path), path.stem) if stacked.z() <= 1 else stacked)
    session.cell_layouts.add(entry)
    return {"cell_layout": describe(entry)}
