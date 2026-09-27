# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The temp command."""

from __future__ import annotations

import math
from typing import TYPE_CHECKING

from mnt.fiction.cli.errors import CommandError
from mnt.fiction.cli.parsing import positive_float, probability
from mnt.fiction.cli.registry import Category, command
from mnt.pyfiction.sidb.analysis import (
    CriticalTemperatureParams,
    critical_temperature_gate_based,
    critical_temperature_non_gate_based,
)

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session
from ._common import ENGINES, _active_sidb_layout, _apply_physical, _engine_argument, _physical_arguments


def _temp_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument("-c", "--confidence", type=probability, default=0.99, help="confidence level")
    parser.add_argument(
        "-t", "--max-temperature", type=positive_float, default=400.0, help="highest temperature in K to try"
    )
    parser.add_argument(
        "-g", "--gate-based", action="store_true", help="judge stability against the active truth table"
    )
    _physical_arguments(parser, base=True)
    _engine_argument(parser)


@command(
    "temp",
    Category.SIMULATION,
    _temp_arguments,
    inputs="Active SiDB layout; gate checks also use the active truth table.",
    example='read and.sqd; tt -e "(ab)"; temp --gate-based',
    progress=True,
)
def temp(session: Session, args: argparse.Namespace) -> Result:
    """Compute the critical temperature of the active SiDB layout.

    The critical temperature is where the ground state's population probability drops below the
    confidence level. With -g, the layout is a gate implementing the active truth table, and
    the erroneous states are those with a wrong output.
    """
    layout = _active_sidb_layout(session)
    params = CriticalTemperatureParams()
    params.on_progress = session.report_progress
    params.on_worker_progress = session.report_worker_progress
    params.confidence_level = args.confidence
    params.max_temperature = args.max_temperature
    params.operational_params.sim_engine = ENGINES[args.engine]
    parameters = _apply_physical(params.operational_params.simulation_parameters, args)
    if args.gate_based:
        if layout.num_pis() == 0 or layout.num_pos() == 0:
            msg = "gate-based simulation needs a layout with input and output dots"
            raise CommandError(msg)
        spec = [session.truth_tables.current()]
        result = critical_temperature_gate_based(layout, spec, params=params)
    else:
        result = critical_temperature_non_gate_based(layout, params=params)
    temperature = result.temperature
    stats = result.stats

    # the statistics leave the energy gap at infinity when no erroneous state exists, and JSON has no infinity
    energy_gap = stats.energy_between_ground_state_and_first_erroneous
    gap = energy_gap if math.isfinite(energy_gap) else None

    if stats.num_valid_lyt == 0:
        session.info(f"the ground state of '{layout.name}' could not be determined")
    else:
        bound = "> " if temperature >= args.max_temperature else ""
        session.info(f"critical temperature of '{layout.name}': {bound}{temperature:.2f} K")
        if stats.num_valid_lyt > 1 and gap is not None:
            session.info(f"energy between the ground state and the first erroneous state: {gap:.2f} meV")
    return {
        "critical_temperature_k": temperature,
        "stable_states": stats.num_valid_lyt,
        "energy_gap_mev": gap,
        "engine": stats.algorithm_name,
        "gate_based": args.gate_based,
        "parameters": parameters,
    }
