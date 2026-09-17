# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The exact command."""

from __future__ import annotations

import os
from typing import TYPE_CHECKING

from mnt import pyfiction
from mnt.fiction.cli.errors import CommandError
from mnt.fiction.cli.parsing import positive_float, positive_int
from mnt.fiction.cli.registry import Category, command
from mnt.fiction.cli.statistics import stats_to_dict
from mnt.fiction.cli.topologies import CLOCKED_LAYOUTS, FGL_READERS

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session
    from mnt.pyfiction import exact_params
from ._common import _added, _seconds_to_ms


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


def _exact_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument("-s", "--scheme", default="2DDWave", help="clocking scheme")
    parser.add_argument(
        "--topology",
        choices=list(FGL_READERS),
        metavar="TOPOLOGY",
        default="cartesian",
        help="layout topology; choices: %(choices)s. hexagonal is even-row, shifted_cartesian is odd-column",
    )
    parser.add_argument("--upper-x", type=positive_int, metavar="N", help="maximum layout width in tiles")
    parser.add_argument("--upper-y", type=positive_int, metavar="N", help="maximum layout height in tiles")
    parser.add_argument("--upper-area", type=positive_int, metavar="N", help="maximum layout area in tiles")
    parser.add_argument(
        "-f", "--fixed-size", type=positive_int, metavar="N", help="only try layouts of exactly N tiles"
    )
    parser.add_argument("-t", "--timeout", type=positive_float, metavar="SECONDS", help="give up after this long")
    parser.add_argument("-a", "--threads", type=positive_int, metavar="N", help="solve N aspect ratios in parallel")
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


@command(
    "exact",
    Category.PHYSICAL_DESIGN,
    _exact_arguments,
    inputs="Active network.",
    example="generate mux -b 1; exact --timeout 10",
    unavailable=None
    if hasattr(pyfiction, "exact_cartesian")
    else "this build of pyfiction has no Z3 solver, which 'exact' needs",
)
def exact(session: Session, args: argparse.Namespace) -> Result:
    """Place and route the active network exactly, with an SMT solver, into a minimal layout.

    Every clocking scheme is supported (-s 2ddwave, use, res, esr, ...). Enabling crossings (-x),
    desynchronization (-d), and a fixed scheme finds solutions fastest; -b puts the I/Os on the
    border. Only small networks finish in reasonable time.
    """
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
    """Build exact placement parameters from validated command options."""
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
