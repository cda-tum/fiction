# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The read_sqd command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction import (
    read_sqd_layout,
)
from mnt.pyfiction.cli.registry import Category, command
from mnt.pyfiction.cli.stores import CellEntry, describe

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.registry import Result
    from mnt.pyfiction.cli.session import Session
from ._common import _existing_file, _read_path_argument


@command("read_sqd", Category.IO, _read_path_argument, inputs="One file.", example='read_sqd "layout.sqd"')
def read_sqd(session: Session, args: argparse.Namespace) -> Result:
    """Read an SiDB cell-level layout from a SiQAD file."""
    path = _existing_file(args.path, (".sqd",))
    entry = CellEntry(read_sqd_layout(str(path), path.stem))
    session.cell_layouts.add(entry)
    return {"cell_layout": describe(entry)}
