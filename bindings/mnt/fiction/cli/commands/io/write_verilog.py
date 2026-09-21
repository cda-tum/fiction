# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The write_verilog command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.registry import Category, command
from mnt.pyfiction import write_verilog

from ._write import output_argument, output_path, written

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


@command(
    "write_verilog",
    Category.IO,
    output_argument,
    inputs="Active network.",
    example="generate mux -b 1; write_verilog output.v",
    progress=True,
)
def write_verilog_command(session: Session, args: argparse.Namespace) -> Result:
    """Write the active network as Verilog.

    Without a filename, use the active element's name and ``.v``.
    """
    element = session.networks.current()
    path = output_path(element, args.file, ".v")
    write_verilog(element, str(path))
    return written(session, path)
