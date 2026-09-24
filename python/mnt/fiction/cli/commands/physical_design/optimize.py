# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The optimize command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.parsing import integer, positive_float
from mnt.fiction.cli.registry import Category, command
from mnt.pyfiction import (
    post_layout_optimization,
    post_layout_optimization_params,
    post_layout_optimization_stats,
    wiring_reduction,
    wiring_reduction_params,
    wiring_reduction_stats,
)

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session
from ._common import _added, _cartesian_2ddwave, _seconds_to_ms


def _optimize_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument("-w", "--wiring-only", action="store_true", help="only reduce wiring, do not move gates")
    parser.add_argument("-m", "--max-relocations", type=integer, metavar="N", help="move at most N gates")
    parser.add_argument("-p", "--planar", action="store_true", help="only move gates when no crossing results")
    parser.add_argument("-t", "--timeout", type=positive_float, metavar="SECONDS", help="stop after this long")
    parser.add_argument("-v", "--verbose", action="store_true", help="print the statistics")


@command(
    "optimize",
    Category.PHYSICAL_DESIGN,
    _optimize_arguments,
    inputs="Active gate-level layout.",
    example="generate mux -b 1; ortho; optimize",
    progress=True,
)
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
        wiring_params.on_progress = session.report_progress
        if timeout is not None:
            wiring_params.timeout = timeout
        stats = wiring_reduction_stats()
        wiring_reduction(layout, wiring_params, stats)
    else:
        params = post_layout_optimization_params()
        params.on_progress = session.report_progress
        params.planar_optimization = args.planar
        if args.max_relocations is not None:
            params.max_gate_relocations = args.max_relocations
        if timeout is not None:
            params.timeout = timeout
        stats = post_layout_optimization_stats()
        post_layout_optimization(layout, params, stats)
    session.gate_layouts.add(layout)
    return _added(session, layout, stats, verbose=args.verbose)
