# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The clustercomplete command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt import pyfiction
from mnt.pyfiction.cli.parsing import integer
from mnt.pyfiction.cli.registry import Category, command

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.parsing import Parser
    from mnt.pyfiction.cli.registry import Result
    from mnt.pyfiction.cli.session import Session
from ._common import _active_sidb_layout, _apply_physical, _physical_arguments, _store_result


def _clustercomplete_arguments(parser: Parser) -> None:
    # base 3 is what ClusterComplete is for, and what the C++ shell defaulted to
    """Add the command's arguments to the parser."""
    _physical_arguments(parser, base=True, base_default=3, potential=True)
    parser.add_argument("-w", "--witness-limit", type=integer, default=6, help="witness partitioning limit")
    parser.add_argument("-o", "--overlap-limit", type=integer, default=6, help="overlapping witnesses limit")
    parser.add_argument("-r", "--report-stats", action="store_true", help="report ground state space statistics")


@command(
    "clustercomplete",
    Category.SIMULATION,
    _clustercomplete_arguments,
    inputs="Active cell-level layout.",
    example="read layout.sqd; clustercomplete",
    unavailable=None
    if hasattr(pyfiction, "clustercomplete")
    else "this build of pyfiction has no ALGLIB, which 'clustercomplete' needs",
)
def clustercomplete_command(session: Session, args: argparse.Namespace) -> Result:
    """Simulate the active SiDB layout exactly with ClusterComplete, which scales to multi-gate layouts in base 3.

    The witness limits tune the first pruning stage; -r prints its statistics.
    """
    layout = _active_sidb_layout(session)
    params = pyfiction.clustercomplete_params()
    parameters = _apply_physical(params.simulation_parameters, args)
    params.global_potential = args.global_potential
    params.validity_witness_partitioning_max_cluster_size_gss = args.witness_limit
    params.num_overlapping_witnesses_limit_gss = args.overlap_limit
    if args.report_stats:
        params.report_gss_stats = pyfiction.ground_state_space_reporting.ON
    parameters.update(
        global_potential=args.global_potential,
        witness_partitioning_limit=args.witness_limit,
        overlapping_witnesses_limit=args.overlap_limit,
    )
    return _store_result(session, layout, pyfiction.clustercomplete(layout, params), parameters)
