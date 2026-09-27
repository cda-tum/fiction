# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The ortho command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.parsing import integer
from mnt.fiction.cli.registry import Category, command
from mnt.fiction.cli.topologies import GATE_LAYOUTS
from mnt.pyfiction.physical_design import ClockPhases, OrthogonalParams, orthogonal

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session
from ._common import _added

THREE_CLOCK_PHASES = 3
"""``ortho -n 3`` asks for the three-phase clocking the library also supports."""


def _ortho_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument(
        "--topology",
        choices=["cartesian", "hexagonal", "even_row_hex", "odd_row_hex", "odd_column_hex", "even_column_hex"],
        metavar="TOPOLOGY",
        default="cartesian",
        help="direct output topology; choices: %(choices)s",
    )
    parser.add_argument(
        "-n",
        "--clock-phases",
        type=integer,
        choices=[3, 4],
        default=4,
        help="the number of clock phases of the result",
    )
    parser.add_argument("-v", "--verbose", action="store_true", help="print the statistics")


@command(
    "ortho",
    Category.PHYSICAL_DESIGN,
    _ortho_arguments,
    inputs="Active network.",
    example="generate mux -b 1; ortho",
    progress=True,
)
def ortho(session: Session, args: argparse.Namespace) -> Result:
    """Place and route the active network with the scalable orthogonal graph drawing heuristic.

    The result uses 2DDWave clocking and the selected Cartesian or hexagonal topology. The network
    must not have gates with more than two inputs.
    """
    network = session.as_technology_network(session.networks.current())
    params = OrthogonalParams()
    params.on_progress = session.report_progress
    params.number_of_clock_phases = ClockPhases.THREE if args.clock_phases == THREE_CLOCK_PHASES else ClockPhases.FOUR
    result = orthogonal(network, params=params, layout_type=GATE_LAYOUTS[args.topology])
    layout, stats = result.layout, result.stats
    session.gate_layouts.add(layout)
    return _added(session, layout, stats, verbose=args.verbose)
