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
from mnt.pyfiction import physical_design
from mnt.pyfiction.physical_design import num_clks, orthogonal, orthogonal_params, orthogonal_stats

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
    params = orthogonal_params()
    params.on_progress = session.report_progress
    params.number_of_clock_phases = num_clks.THREE if args.clock_phases == THREE_CLOCK_PHASES else num_clks.FOUR
    stats = orthogonal_stats()
    topology = "hexagonal" if args.topology == "even_row_hex" else args.topology
    design = orthogonal if topology == "cartesian" else getattr(physical_design, f"orthogonal_{topology}")
    layout = design(network, params, stats)
    session.gate_layouts.add(layout)
    return _added(session, layout, stats, verbose=args.verbose)
