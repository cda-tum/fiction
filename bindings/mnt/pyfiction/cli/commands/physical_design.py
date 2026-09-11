# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Physical design commands: placement and routing, hexagonalization, and post-layout optimization."""

from __future__ import annotations

import math
import os
from typing import TYPE_CHECKING

from mnt import pyfiction
from mnt.pyfiction import (
    cartesian_gate_layout,
    clocked_cartesian_layout,
    clocked_hexagonal_layout,
    clocked_shifted_cartesian_layout,
    gold_cost_objective,
    gold_effort_mode,
    graph_oriented_layout_design,
    graph_oriented_layout_design_params,
    graph_oriented_layout_design_stats,
    hexagonalization,
    hexagonalization_io_pin_extension_mode,
    hexagonalization_params,
    hexagonalization_stats,
    num_clks,
    orthogonal,
    orthogonal_params,
    orthogonal_stats,
    post_layout_optimization,
    post_layout_optimization_params,
    post_layout_optimization_stats,
    wiring_reduction,
    wiring_reduction_params,
    wiring_reduction_stats,
)
from mnt.pyfiction.cli.commands.files import FGL_READERS
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import Category, command
from mnt.pyfiction.cli.render import table as render_table
from mnt.pyfiction.cli.session import stats_to_dict
from mnt.pyfiction.cli.stores import TOPOLOGIES, describe

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction import exact_params
    from mnt.pyfiction.cli.registry import Parser, Result
    from mnt.pyfiction.cli.session import Session
    from mnt.pyfiction.cli.stores import GateLayout

CLOCKED_LAYOUTS = {
    "cartesian": clocked_cartesian_layout,
    "odd_column_cartesian": clocked_shifted_cartesian_layout,
    "even_row_hex": clocked_hexagonal_layout,
    "odd_row_cartesian": pyfiction.clocked_odd_row_cartesian_layout,
    "even_row_cartesian": pyfiction.clocked_even_row_cartesian_layout,
    "even_column_cartesian": pyfiction.clocked_even_column_cartesian_layout,
    "odd_row_hex": pyfiction.clocked_odd_row_hex_layout,
    "odd_column_hex": pyfiction.clocked_odd_column_hex_layout,
    "even_column_hex": pyfiction.clocked_even_column_hex_layout,
    "shifted_cartesian": clocked_shifted_cartesian_layout,
    "hexagonal": clocked_hexagonal_layout,
}
"""The smallest clocked layout of each topology, which is what validates a clocking scheme name."""

MILLISECONDS = 1000
"""The bindings take timeouts in milliseconds; the commands take seconds."""

THREE_CLOCK_PHASES = 3
"""``ortho -n 3`` asks for the three-phase clocking the library also supports."""


def _added(session: Session, layout: GateLayout, stats: object, *, verbose: bool) -> Result:
    """Describe a layout a command just added, printing its statistics as the shared table when asked.

    Args:
        session: The session, for the console.
        layout: The layout that was added to the store.
        stats: The algorithm's statistics object.
        verbose: Print the statistics.

    Returns:
        The log entry of the command.
    """
    statistics = stats_to_dict(stats)
    if verbose:
        session.console.print(render_table(statistics))
    return {"gate_layout": describe(layout), "stats": statistics}


def _clocking_scheme(name: str, topology: str) -> str:
    """Return a clocking scheme name the library knows for a topology.

    The library owns the list of schemes, so it is asked rather than a table here that would drift
    away from it: a one-tile clocked layout of the topology accepts exactly the supported names.

    Args:
        name: The name the user typed.
        topology: The layout topology the scheme has to exist for.

    Returns:
        The name, upper-cased as the solver expects it.

    Raises:
        CommandError: When the library knows no such scheme for the topology.
    """
    scheme = name.upper()
    try:
        CLOCKED_LAYOUTS[topology]((0, 0), scheme)
    except RuntimeError as error:
        msg = f"'{name}' is not a clocking scheme for {topology} layouts; see the CLI documentation for the list"
        raise CommandError(msg) from error
    return scheme


def _seconds_to_ms(seconds: float | None) -> int | None:
    if seconds is None:
        return None
    if not math.isfinite(seconds) or seconds <= 0 or seconds * MILLISECONDS > 2**32 - 1:
        msg = "the timeout must be positive"
        raise CommandError(msg)
    return math.ceil(seconds * MILLISECONDS)


def _exact_arguments(parser: Parser) -> None:
    parser.add_argument("-s", "--scheme", default="2DDWave", help="clocking scheme (default: 2DDWave)")
    parser.add_argument(
        "--topology",
        choices=list(FGL_READERS),
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
    parser.add_argument(
        "--synchronization-elements", action="store_true", help="allow Cartesian synchronization elements"
    )
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
    parser.add_argument("-v", "--verbose", action="store_true", help="print the statistics")


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
    topology = "shifted_cartesian" if args.topolinano else args.topology
    params = _exact_parameters(args, _clocking_scheme(args.scheme, topology))
    native_topology = {"odd_column_cartesian": "shifted_cartesian", "even_row_hex": "hexagonal"}.get(topology, topology)
    design = getattr(pyfiction, f"exact_{native_topology}")
    if args.synchronization_elements and topology != "cartesian":
        msg_0 = "synchronization elements require Cartesian topology"
        raise CommandError(msg_0)

    network = session.as_technology_network(session.networks.current())
    stats = pyfiction.exact_stats()
    layout = design(network, params, stats)
    if layout is None:
        msg = f"no layout found for '{pyfiction.get_name(network)}' within the search bounds or timeout"
        error = CommandError(msg)
        error.stats = stats_to_dict(stats)
        raise error
    session.gate_layouts.add(layout)
    return _added(session, layout, stats, verbose=args.verbose)


def _exact_parameters(args: argparse.Namespace, scheme: str) -> exact_params:
    params = pyfiction.exact_params()
    params.scheme = scheme
    params.synchronization_elements = args.synchronization_elements
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
        params.technology_specifics = pyfiction.pyfiction.technology_constraints.TOPOLINANO
    return params


def _ortho_arguments(parser: Parser) -> None:
    parser.add_argument(
        "--topology",
        choices=["cartesian", "hexagonal", "even_row_hex", "odd_row_hex", "odd_column_hex", "even_column_hex"],
        default="cartesian",
        help="direct output topology (default: cartesian)",
    )
    parser.add_argument(
        "-n",
        "--clock-phases",
        type=int,
        choices=[3, 4],
        default=4,
        help="the number of clock phases of the result (default: 4)",
    )
    parser.add_argument("-v", "--verbose", action="store_true", help="print the statistics")


@command("ortho", Category.PHYSICAL_DESIGN, _ortho_arguments)
def ortho(session: Session, args: argparse.Namespace) -> Result:
    """Place and route the active network with the scalable orthogonal graph drawing heuristic.

    The result uses 2DDWave clocking and the selected Cartesian or hexagonal topology. The network
    must not have gates with more than two inputs.
    """
    network = session.as_technology_network(session.networks.current())
    params = orthogonal_params()
    params.number_of_clock_phases = num_clks.THREE if args.clock_phases == THREE_CLOCK_PHASES else num_clks.FOUR
    stats = orthogonal_stats()
    topology = "hexagonal" if args.topology == "even_row_hex" else args.topology
    design = orthogonal if topology == "cartesian" else getattr(pyfiction, f"orthogonal_{topology}")
    layout = design(network, params, stats)
    session.gate_layouts.add(layout)
    return _added(session, layout, stats, verbose=args.verbose)


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
    parser.add_argument(
        "--progress",
        action="store_true",
        help="let the search write its own progress to the terminal, bypassing the shell",
    )


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
    params.verbose = args.progress
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
    return _added(session, hexagonal, stats, verbose=args.verbose)


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
    return _added(session, layout, stats, verbose=args.verbose)


def _cartesian_2ddwave(session: Session) -> GateLayout:
    layout = session.gate_layouts.current()
    if not isinstance(layout, cartesian_gate_layout):
        msg = f"the active layout is {TOPOLOGIES[type(layout)]}; a Cartesian layout is needed"
        raise CommandError(msg)
    if layout.get_clocking_scheme_name().upper() != "2DDWAVE":
        msg = f"the active layout is {layout.get_clocking_scheme_name()}-clocked; 2DDWave is needed"
        raise CommandError(msg)
    return layout
