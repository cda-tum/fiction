# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The hex command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.registry import Category, command
from mnt.pyfiction import (
    hexagonalization,
    hexagonalization_io_pin_extension_mode,
    hexagonalization_params,
    hexagonalization_stats,
)

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session
from ._common import _added, _cartesian_2ddwave


def _hex_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument("-i", "--extend-inputs", action="store_true", help="extend the inputs to the top row")
    parser.add_argument("-o", "--extend-outputs", action="store_true", help="extend the outputs to the bottom row")
    parser.add_argument("-p", "--planar", action="store_true", help="reroute the extended pins without crossings")
    parser.add_argument("-v", "--verbose", action="store_true", help="print the statistics")


@command(
    "hex",
    Category.PHYSICAL_DESIGN,
    _hex_arguments,
    inputs="Active gate-level layout.",
    example="generate mux -b 1; ortho; hex",
)
def hex_command(session: Session, args: argparse.Namespace) -> Result:
    """Turn the active 2DDWave-clocked Cartesian layout into a hexagonal, row-clocked one.

    Hexagonal layouts are what the Bestagon SiDB gate library needs.
    """
    layout = _cartesian_2ddwave(session)
    if args.planar and not (args.extend_inputs or args.extend_outputs):
        session.info("planar rerouting only applies to extended pins; add -i or -o")
    mode = (
        hexagonalization_io_pin_extension_mode.EXTEND_PLANAR
        if args.planar
        else hexagonalization_io_pin_extension_mode.EXTEND
    )
    params = hexagonalization_params()
    params.on_progress = session.report_progress
    if args.extend_inputs:
        params.input_pin_extension = mode
    if args.extend_outputs:
        params.output_pin_extension = mode
    stats = hexagonalization_stats()
    hexagonal = hexagonalization(layout, params, stats)
    session.gate_layouts.add(hexagonal)
    return _added(session, hexagonal, stats, verbose=args.verbose)
