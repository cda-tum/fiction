# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The write_sqd command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.registry import Category, command
from mnt.pyfiction import (
    sidb_layout,
    write_sqd_layout,
)

from ._write import output_argument, output_path, require_cell_type, written

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


def _write_sqd_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    output_argument(parser)


@command(
    "write_sqd",
    Category.IO,
    _write_sqd_arguments,
    inputs="Active cell-level layout.",
    example="read_sqd layout.sqd; write_sqd output.sqd",
)
def write_sqd_command(session: Session, args: argparse.Namespace) -> Result:
    """Write the active SiDB layout as a SiQAD SQD file.

    Without a filename, use the active element's name and ``.sqd``.
    """
    entry = session.cell_layouts.current()
    element = entry.layout
    require_cell_type(element, (sidb_layout,), ".sqd")
    path = output_path(element, args.file, ".sqd")
    write_sqd_layout(element, str(path))
    return written(session, path)
