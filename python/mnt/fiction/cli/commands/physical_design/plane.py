# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The plane command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.parsing import integer
from mnt.fiction.cli.registry import Category, command
from mnt.fiction.cli.statistics import stats_to_dict
from mnt.pyfiction.physical_design import num_clks, plane_params, plane_stats
from mnt.pyfiction.physical_design import plane as plane_layout
from mnt.pyfiction.synthesis import (
    network_balancing,
    network_balancing_params,
    planarization,
    planarization_params,
    planarization_stats,
)

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session

from ._common import _added

THREE_CLOCK_PHASES = 3
"""``plane -n 3`` asks for the three-phase clocking the library also supports."""


def _plane_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument(
        "-n",
        "--clock-phases",
        type=integer,
        choices=[3, 4],
        default=4,
        help="the number of clock phases of the result",
    )
    parser.add_argument(
        "-x", "--xor-gates", action="store_true", help="build crossing gadgets from XOR gates instead of AND, OR, NOT"
    )
    parser.add_argument("-v", "--verbose", action="store_true", help="print the statistics")


@command(
    "plane",
    Category.PHYSICAL_DESIGN,
    _plane_arguments,
    inputs="Active network.",
    example="generate mux -b 1; plane",
    progress=True,
)
def plane(session: Session, args: argparse.Namespace) -> Result:
    """Planarize the active network and place and route it without wire crossings (PLANE).

    The network is balanced, planarized by node duplication and crossing gates, fanout-substituted,
    and rebalanced; the Planar Layout from Network Embedding (PLANE) algorithm then places every
    level on one diagonal of a 2DDWave-clocked Cartesian layout. Duplicated inputs appear as
    separate layout inputs. The network must not have gates with more than two inputs.
    """
    network = session.as_technology_network(session.networks.current())

    balancing = network_balancing_params()
    balancing.unify_outputs = True
    balanced = network_balancing(network, balancing)

    planarization_parameters = planarization_params()
    planarization_parameters.on_progress = session.report_progress
    planarization_parameters.duplication.xor_gates = args.xor_gates

    planarization_statistics = planarization_stats()
    planar, _original_inputs = planarization(balanced, planarization_parameters, planarization_statistics)

    params = plane_params()
    params.on_progress = session.report_progress
    params.number_of_clock_phases = num_clks.THREE if args.clock_phases == THREE_CLOCK_PHASES else num_clks.FOUR

    stats = plane_stats()
    layout = plane_layout(planar, params, stats)
    session.gate_layouts.add(layout)

    return {
        **(_added(session, layout, stats, verbose=args.verbose) or {}),
        "planarization": stats_to_dict(planarization_statistics),
    }
