# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The write_fgl command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.registry import Category, command
from mnt.pyfiction import (
    write_fgl_layout,
)

from ._write import output_argument, output_path, written

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


def _write_fgl_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    output_argument(parser)


@command(
    "write_fgl",
    Category.IO,
    _write_fgl_arguments,
    inputs="Active gate-level layout.",
    example="generate mux -b 1; ortho; write_fgl output.fgl",
)
def write_fgl_command(session: Session, args: argparse.Namespace) -> Result:
    """Write the active gate-level layout as FGL.

    Without a filename, use the active element's name and ``.fgl``.
    """
    element = session.gate_layouts.current()
    path = output_path(element, args.file, ".fgl")
    write_fgl_layout(element, str(path))
    return written(session, path)
