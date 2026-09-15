# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The write_aiger command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.errors import CommandError
from mnt.fiction.cli.registry import Category, command
from mnt.pyfiction import (
    aig_network,
    write_aiger,
)

from ._write import output_argument, output_path, written

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


def _write_aiger_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    output_argument(parser)


@command(
    "write_aiger",
    Category.IO,
    _write_aiger_arguments,
    inputs="Active network.",
    example="read_verilog circuit.v --type aig; write_aiger output.aig",
)
def write_aiger_command(session: Session, args: argparse.Namespace) -> Result:
    """Write the active AIG as binary AIGER.

    Without a filename, use the active element's name and ``.aig``.
    """
    element = session.networks.current()
    if not isinstance(element, aig_network):
        msg = "only an AIG can be written as AIGER; read the network with --type aig"
        raise CommandError(msg)
    path = output_path(element, args.file, ".aig")
    write_aiger(element, str(path))
    return written(session, path)
