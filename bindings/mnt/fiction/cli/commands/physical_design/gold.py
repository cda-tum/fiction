# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The gold command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt import pyfiction
from mnt.fiction.cli.errors import CommandError
from mnt.fiction.cli.parsing import integer, positive_float, positive_int, seed
from mnt.fiction.cli.registry import Category, command
from mnt.fiction.cli.statistics import stats_to_dict
from mnt.pyfiction import (
    gold_cost_objective,
    gold_effort_mode,
    graph_oriented_layout_design,
    graph_oriented_layout_design_params,
    graph_oriented_layout_design_stats,
)

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session
from ._common import _added, _seconds_to_ms


def _gold_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument("-t", "--timeout", type=positive_float, metavar="SECONDS", help="give up after this long")
    parser.add_argument("-n", "--expansions", type=positive_int, default=4, help="vertex expansions per search node")
    parser.add_argument(
        "-e",
        "--effort",
        choices=["high_efficiency", "high_effort", "highest_effort", "maximum_effort"],
        default="high_effort",
        help="how many search space graphs to explore",
    )
    parser.add_argument(
        "-c",
        "--cost",
        choices=["area", "wires", "crossings", "acp"],
        default="area",
        help="what to minimize; acp is the area-crossing product",
    )
    parser.add_argument("-r", "--return-first", action="store_true", help="stop at the first valid layout")
    parser.add_argument("-p", "--planar", action="store_true", help="forbid crossings")
    parser.add_argument("-m", "--multithreading", action="store_true", help="explore the graphs in parallel")
    parser.add_argument("-s", "--seed", type=seed, help="seed for the randomized strategies")
    parser.add_argument("-i", "--straight-inverters", action="store_true", help="forbid bent inverters")
    parser.add_argument(
        "-g", "--skip-tiles", type=integer, default=0, metavar="N", help="empty tiles kept after each PI"
    )
    parser.add_argument("-j", "--randomize-skip-tiles", action="store_true", help="randomize the tiles kept after PIs")
    parser.add_argument("-v", "--verbose", action="store_true", help="print the statistics")
    parser.add_argument(
        "--progress",
        action="store_true",
        help="show shell progress (enabled by default)",
    )


@command(
    "gold",
    Category.PHYSICAL_DESIGN,
    _gold_arguments,
    inputs="Active network.",
    example="generate mux -b 1; gold --timeout 10",
    progress=True,
)
def gold(session: Session, args: argparse.Namespace) -> Result:
    """Place and route the active network with graph-oriented layout design, an A* search.

    The result is a 2DDWave-clocked Cartesian layout. The network must not have gates with more
    than two inputs.
    """
    params = graph_oriented_layout_design_params()
    params.on_progress = session.report_progress
    params.on_worker_progress = session.report_worker_progress
    params.num_vertex_expansions = args.expansions
    params.mode = getattr(gold_effort_mode, args.effort.upper())
    params.cost = getattr(gold_cost_objective, args.cost.upper())
    params.return_first = args.return_first
    params.planar = args.planar
    params.enable_multithreading = args.multithreading
    params.straight_inverters = args.straight_inverters
    params.tiles_to_skip_between_pis = args.skip_tiles
    params.randomize_tiles_to_skip_between_pis = args.randomize_skip_tiles
    params.verbose = False
    if args.seed is not None:
        params.seed = args.seed
    timeout = _seconds_to_ms(args.timeout)
    if timeout is not None:
        params.timeout = timeout

    network = session.as_technology_network(session.networks.current())
    stats = graph_oriented_layout_design_stats()
    layout = graph_oriented_layout_design(network, params, stats)
    if layout is None:
        msg = f"no layout found for '{pyfiction.get_name(network)}' within the search bounds or timeout"
        error = CommandError(msg)
        error.stats = stats_to_dict(stats)
        raise error
    session.gate_layouts.add(layout)
    return _added(session, layout, stats, verbose=args.verbose)
