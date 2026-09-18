# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The write_blif command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.registry import Category, command
from mnt.pyfiction import write_blif

from ._write import output_argument, output_path, written

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


@command(
    "write_blif",
    Category.IO,
    output_argument,
    inputs="Active network.",
    example="generate mux -b 1; write_blif output.blif",
)
def write_blif_command(session: Session, args: argparse.Namespace) -> Result:
    """Write the active network as BLIF.

    Without a filename, use the active element's name and ``.blif``.
    """
    element = session.networks.current()
    path = output_path(element, args.file, ".blif")
    write_blif(element, str(path))
    return written(session, path)
