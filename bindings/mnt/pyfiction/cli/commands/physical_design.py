# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Physical design commands: placement and routing, hexagonalization, and post-layout optimization."""

from __future__ import annotations

import os
from typing import TYPE_CHECKING

from mnt import pyfiction
from mnt.pyfiction import (
    cartesian_gate_layout,
    exact_params,
    exact_stats,
    gold_cost_objective,
    gold_effort_mode,
    graph_oriented_layout_design,
    graph_oriented_layout_design_params,
    graph_oriented_layout_design_stats,
    hexagonalization,
    hexagonalization_io_pin_extension_mode,
    hexagonalization_params,
    hexagonalization_stats,
    orthogonal,
    orthogonal_params,
    orthogonal_stats,
    post_layout_optimization,
    post_layout_optimization_params,
    post_layout_optimization_stats,
    technology_constraints,
    wiring_reduction,
    wiring_reduction_params,
    wiring_reduction_stats,
)
from mnt.pyfiction.cli.registry import Category, CommandError, command
from mnt.pyfiction.cli.session import stats_to_dict
from mnt.pyfiction.cli.stores import TOPOLOGIES, describe

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.registry import Parser, Result
    from mnt.pyfiction.cli.session import Session
    from mnt.pyfiction.cli.stores import GateLayout

CLOCKING_SCHEMES = (
    "OPEN",
    "OPEN3",
    "OPEN4",
    "COLUMNAR",
    "COLUMNAR3",
    "COLUMNAR4",
    "ROW",
    "ROW3",
    "ROW4",
    "2DDWAVE",
    "2DDWAVE3",
    "2DDWAVE4",
    "2DDWAVEHEX",
    "2DDWAVEHEX3",
    "2DDWAVEHEX4",
    "USE",
    "RES",
    "ESR",
    "CFE",
    "RIPPLE",
    "SRS",
    "BANCS",
)
"""The clocking scheme names ``exact`` accepts; a bare name uses four clock phases."""

MILLISECONDS = 1000
"""The bindings take timeouts in milliseconds; the commands take seconds."""


def _seconds_to_ms(seconds: float | None) -> int | None:
    if seconds is None:
        return None
    if seconds <= 0:
        msg = "the timeout must be positive"
        raise CommandError(msg)
    return int(seconds * MILLISECONDS)


def _exact_arguments(parser: Parser) -> None:
    parser.add_argument("-s", "--scheme", default="2DDWave", help="clocking scheme (default: 2DDWave)")
    parser.add_argument(
        "--topology",
        choices=list(TOPOLOGIES.values()),
        default="cartesian",
        help="layout topology; hexagonal is even-row, shifted_cartesian is odd-column (default: cartesian)",
    )
    parser.add_argument("--upper-x", type=int, metavar="N", help="maximum layout width in tiles")
    parser.add_argument("--upper-y", type=int, metavar="N", help="maximum layout height in tiles")
    parser.add_argument("--upper-area", type=int, metavar="N", help="maximum layout area in tiles")
    parser.add_argument("-f", "--fixed-size", type=int, metavar="N", help="only try layouts of exactly N tiles")
    parser.add_argument("-t", "--timeout", type=float, metavar="SECONDS", help="give up after this long")
    parser.add_argument("-a", "--threads", type=int, metavar="N", help="solve N aspect ratios in parallel")
    parser.add_argument("--async-max", action="store_true", help="use every processor core for -a")
    parser.add_argument("-x", "--crossings", action="store_true", help="allow wire crossings")
    parser.add_argument("-b", "--border-io", action="store_true", help="route all I/Os to the layout border")
    parser.add_argument("-n", "--straight-inverters", action="store_true", help="forbid bent inverters")
    parser.add_argument("-d", "--desynchronize", action="store_true", help="drop global synchronization")
    parser.add_argument("-w", "--minimize-wires", action="store_true", help="also minimize the wire count")
    parser.add_argument("-c", "--minimize-crossings", action="store_true", help="also minimize the crossing count")
    parser.add_argument(
        "--topolinano",
        action="store_true",
        help="apply ToPoliNano's iNML constraints; implies --topology shifted_cartesian",
    )


@command("exact", Category.PHYSICAL_DESIGN, _exact_arguments)
def exact(session: Session, args: argparse.Namespace) -> Result:
    """Place and route the active network exactly, with an SMT solver, into a minimal layout.

    Every clocking scheme is supported (-s 2ddwave, use, res, esr, ...). Enabling crossings (-x),
    desynchronization (-d), and a fixed scheme finds solutions fastest; -b puts the I/Os on the
    border. Only small networks finish in reasonable time.
    """
    if not hasattr(pyfiction, "exact_cartesian"):
        msg = "this build of pyfiction has no Z3 solver, which 'exact' needs"
        raise CommandError(msg)
    scheme = args.scheme.upper()
    if scheme not in CLOCKING_SCHEMES:
        msg = f"'{args.scheme}' is not a clocking scheme; choose from {', '.join(CLOCKING_SCHEMES)}"
        raise CommandError(msg)

    params = _exact_parameters(args, scheme)

    topology = "shifted_cartesian" if args.topolinano else args.topology
    design = getattr(pyfiction, f"exact_{topology}")

    network = session.as_technology_network(session.networks.current())
    stats = exact_stats()
    layout = design(network, params, stats)
    if layout is None:
        msg = f"impossible to place and route '{pyfiction.get_name(network)}' within the given parameters"
        raise CommandError(msg)
    session.gate_layouts.add(layout)
    return {"gate_layout": describe(layout), "stats": stats_to_dict(stats)}


def _exact_parameters(args: argparse.Namespace, scheme: str) -> exact_params:
    params = exact_params()
    params.scheme = scheme
    params.crossings = args.crossings
    params.border_io = args.border_io
    params.straight_inverters = args.straight_inverters
    params.desynchronize = args.desynchronize
    params.minimize_wires = args.minimize_wires
    params.minimize_crossings = args.minimize_crossings
    for name, value in (
        ("upper_bound_x", args.upper_x),
        ("upper_bound_y", args.upper_y),
        ("upper_bound_area", args.upper_area),
    ):
        if value is not None:
            setattr(params, name, value)
    if args.fixed_size is not None:
        params.fixed_size = True
        params.upper_bound_area = args.fixed_size
    timeout = _seconds_to_ms(args.timeout)
    if timeout is not None:
        params.timeout = timeout
    if args.async_max:
        params.num_threads = os.cpu_count() or 1
    elif args.threads is not None:
        params.num_threads = args.threads
    if args.topolinano:
        params.technology_specifics = technology_constraints.TOPOLINANO
    return params


def _ortho_arguments(parser: Parser) -> None:
    parser.add_argument("-v", "--verbose", action="store_true", help="print the statistics")


@command("ortho", Category.PHYSICAL_DESIGN, _ortho_arguments)
def ortho(session: Session, args: argparse.Namespace) -> Result:
    """Place and route the active network with the scalable orthogonal graph drawing heuristic.

    The result is a 2DDWave-clocked Cartesian layout; run 'hex' to turn it hexagonal. The network
    must not have gates with more than two inputs.
    """
    network = session.as_technology_network(session.networks.current())
    stats = orthogonal_stats()
    layout = orthogonal(network, orthogonal_params(), stats)
    session.gate_layouts.add(layout)
    if args.verbose:
        session.info(repr(stats))
    return {"gate_layout": describe(layout), "stats": stats_to_dict(stats)}


def _gold_arguments(parser: Parser) -> None:
    parser.add_argument("-t", "--timeout", type=float, metavar="SECONDS", help="give up after this long")
    parser.add_argument("-n", "--expansions", type=int, default=4, help="vertex expansions per search node")
    parser.add_argument(
        "-e",
        "--effort",
        choices=["high_efficiency", "high_effort", "highest_effort", "maximum_effort"],
        default="high_effort",
        help="how many search space graphs to explore (default: high_effort)",
    )
    parser.add_argument(
        "-c",
        "--cost",
        choices=["area", "wires", "crossings", "acp"],
        default="area",
        help="what to minimize; acp is the area-crossing product (default: area)",
    )
    parser.add_argument("-r", "--return-first", action="store_true", help="stop at the first valid layout")
    parser.add_argument("-p", "--planar", action="store_true", help="forbid crossings")
    parser.add_argument("-m", "--multithreading", action="store_true", help="explore the graphs in parallel")
    parser.add_argument("-s", "--seed", type=int, help="seed for the randomized strategies")
    parser.add_argument("-i", "--straight-inverters", action="store_true", help="forbid bent inverters")
    parser.add_argument("-g", "--skip-tiles", type=int, default=0, metavar="N", help="empty tiles kept after each PI")
    parser.add_argument("-j", "--randomize-skip-tiles", action="store_true", help="randomize the tiles kept after PIs")
    parser.add_argument("-v", "--verbose", action="store_true", help="print the statistics")


@command("gold", Category.PHYSICAL_DESIGN, _gold_arguments)
def gold(session: Session, args: argparse.Namespace) -> Result:
    """Place and route the active network with graph-oriented layout design, an A* search.

    The result is a 2DDWave-clocked Cartesian layout. The network must not have gates with more
    than two inputs.
    """
    if args.expansions < 1:
        msg = "the number of vertex expansions must be at least 1"
        raise CommandError(msg)
    params = graph_oriented_layout_design_params()
    params.num_vertex_expansions = args.expansions
    params.mode = getattr(gold_effort_mode, args.effort.upper())
    params.cost = getattr(gold_cost_objective, args.cost.upper())
    params.return_first = args.return_first
    params.planar = args.planar
    params.enable_multithreading = args.multithreading
    params.straight_inverters = args.straight_inverters
    params.tiles_to_skip_between_pis = args.skip_tiles
    params.randomize_tiles_to_skip_between_pis = args.randomize_skip_tiles
    params.verbose = args.verbose
    if args.seed is not None:
        params.seed = args.seed
    timeout = _seconds_to_ms(args.timeout)
    if timeout is not None:
        params.timeout = timeout

    network = session.as_technology_network(session.networks.current())
    stats = graph_oriented_layout_design_stats()
    layout = graph_oriented_layout_design(network, params, stats)
    if layout is None:
        msg = f"no layout found for '{pyfiction.get_name(network)}' within the given parameters"
        raise CommandError(msg)
    session.gate_layouts.add(layout)
    if args.verbose:
        session.info(repr(stats))
    return {"gate_layout": describe(layout), "stats": stats_to_dict(stats)}


def _hex_arguments(parser: Parser) -> None:
    parser.add_argument("-i", "--extend-inputs", action="store_true", help="extend the inputs to the top row")
    parser.add_argument("-o", "--extend-outputs", action="store_true", help="extend the outputs to the bottom row")
    parser.add_argument("-p", "--planar", action="store_true", help="reroute the extended pins without crossings")
    parser.add_argument("-v", "--verbose", action="store_true", help="print the statistics")


@command("hex", Category.PHYSICAL_DESIGN, _hex_arguments)
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
    if args.extend_inputs:
        params.input_pin_extension = mode
    if args.extend_outputs:
        params.output_pin_extension = mode
    stats = hexagonalization_stats()
    hexagonal = hexagonalization(layout, params, stats)
    session.gate_layouts.add(hexagonal)
    if args.verbose:
        session.info(repr(stats))
    return {"gate_layout": describe(hexagonal), "stats": stats_to_dict(stats)}


def _optimize_arguments(parser: Parser) -> None:
    parser.add_argument("-w", "--wiring-only", action="store_true", help="only reduce wiring, do not move gates")
    parser.add_argument("-m", "--max-relocations", type=int, metavar="N", help="move at most N gates")
    parser.add_argument("-p", "--planar", action="store_true", help="only move gates when no crossing results")
    parser.add_argument("-t", "--timeout", type=float, metavar="SECONDS", help="stop after this long")
    parser.add_argument("-v", "--verbose", action="store_true", help="print the statistics")


@command("optimize", Category.PHYSICAL_DESIGN, _optimize_arguments)
def optimize(session: Session, args: argparse.Namespace) -> Result:
    """Shrink the active 2DDWave-clocked Cartesian layout by moving gates and shortening wires.

    The optimized copy is added to the store; the original stays. Use -w for very large layouts
    and -m 1 for layouts beyond 100000 tiles.
    """
    layout = _cartesian_2ddwave(session).clone()
    timeout = _seconds_to_ms(args.timeout)
    stats: post_layout_optimization_stats | wiring_reduction_stats
    if args.wiring_only:
        wiring_params = wiring_reduction_params()
        if timeout is not None:
            wiring_params.timeout = timeout
        stats = wiring_reduction_stats()
        wiring_reduction(layout, wiring_params, stats)
    else:
        params = post_layout_optimization_params()
        params.planar_optimization = args.planar
        if args.max_relocations is not None:
            params.max_gate_relocations = args.max_relocations
        if timeout is not None:
            params.timeout = timeout
        stats = post_layout_optimization_stats()
        post_layout_optimization(layout, params, stats)
    session.gate_layouts.add(layout)
    if args.verbose:
        session.info(repr(stats))
    return {"gate_layout": describe(layout), "stats": stats_to_dict(stats)}


def _cartesian_2ddwave(session: Session) -> GateLayout:
    layout = session.gate_layouts.current()
    if not isinstance(layout, cartesian_gate_layout):
        msg = f"the active layout is {TOPOLOGIES[type(layout)]}; a Cartesian layout is needed"
        raise CommandError(msg)
    if layout.get_clocking_scheme_name().upper() != "2DDWAVE":
        msg = f"the active layout is {layout.get_clocking_scheme_name()}-clocked; 2DDWave is needed"
        raise CommandError(msg)
    return layout
