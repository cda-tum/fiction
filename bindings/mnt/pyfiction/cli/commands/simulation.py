# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Physical simulation commands for SiDB layouts: ground states, critical temperature, operational domain."""

from __future__ import annotations

import math
from pathlib import Path
from typing import TYPE_CHECKING

from mnt import pyfiction
from mnt.pyfiction import (
    critical_temperature_gate_based,
    critical_temperature_non_gate_based,
    critical_temperature_params,
    critical_temperature_stats,
    operational_analysis_strategy,
    operational_condition,
    operational_domain_contour_tracing,
    operational_domain_flood_fill,
    operational_domain_grid_search,
    operational_domain_params,
    operational_domain_random_sampling,
    operational_domain_stats,
    operational_domain_value_range,
    quickexact,
    quickexact_params,
    quicksim,
    quicksim_params,
    sample_writing_mode,
    sidb_layout,
    sidb_simulation_engine,
    sweep_parameter,
    write_operational_domain,
    write_operational_domain_params,
)
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import Category, command
from mnt.pyfiction.cli.session import stats_to_dict
from mnt.pyfiction.cli.stores import TECHNOLOGIES, CellEntry, describe

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.registry import Parser, Result
    from mnt.pyfiction.cli.session import Session

ENGINES = {name.lower(): member for name, member in sidb_simulation_engine.__members__.items()}
"""The ``--engine`` names and the engines they select; ``clustercomplete`` exists only with ALGLIB."""

SWEEPS = {
    "epsilon_r": sweep_parameter.EPSILON_R,
    "lambda_tf": sweep_parameter.LAMBDA_TF,
    "mu_minus": sweep_parameter.MU_MINUS,
}
"""The ``--x-sweep`` names and the parameters they sweep."""

DEFAULT_SWEEPS = {
    "x": ("epsilon_r", 1.0, 10.0, 0.1),
    "y": ("lambda_tf", 1.0, 10.0, 0.1),
    "z": ("mu_minus", -0.5, -0.1, 0.025),
}
"""The default sweep of each axis of ``opdom``."""


def _physical_arguments(parser: Parser, *, base: bool, base_default: int = 2) -> None:
    physics = parser.group("physical parameters")
    physics.add_argument("-e", "--epsilon-r", type=float, default=5.6, help="relative permittivity (default: 5.6)")
    physics.add_argument("-l", "--lambda-tf", type=float, default=5.0, help="Thomas-Fermi screening length in nm")
    physics.add_argument("-m", "--mu-minus", type=float, default=-0.32, help="energy transition level (0/-) in eV")
    if base:
        physics.add_argument(
            "--base",
            type=int,
            choices=[2, 3],
            default=base_default,
            help=f"charge states per SiDB (default: {base_default})",
        )


def _apply_physical(params: object, args: argparse.Namespace) -> dict[str, object]:
    """Copy the physical parameters of ``args`` into a ``sidb_simulation_parameters`` and describe them."""
    if args.epsilon_r <= 0:
        msg = "epsilon_r must be positive"
        raise CommandError(msg)
    if args.lambda_tf <= 0:
        msg = "lambda_tf must be positive"
        raise CommandError(msg)
    params.epsilon_r = args.epsilon_r  # type: ignore[attr-defined]
    params.lambda_tf = args.lambda_tf  # type: ignore[attr-defined]
    params.mu_minus = args.mu_minus  # type: ignore[attr-defined]
    description: dict[str, object] = {
        "epsilon_r": args.epsilon_r,
        "lambda_tf": args.lambda_tf,
        "mu_minus": args.mu_minus,
    }
    if hasattr(args, "base"):
        params.base = args.base  # type: ignore[attr-defined]
        description["base"] = args.base
    return description


def _active_sidb_layout(session: Session, *, unsimulated: bool = False) -> sidb_layout:
    """Return the active SiDB layout.

    Args:
        session: The session.
        unsimulated: Refuse an element that already carries a simulation result, because running an
            engine on it would store the same layout a second time. ``temp`` and ``opdom`` push
            nothing, so they do not ask for this.

    Returns:
        The layout.

    Raises:
        CommandError: When the active element is of another technology, or is already simulated
            while ``unsimulated`` asks for a fresh one.
    """
    entry = session.cell_layouts.current()
    if not isinstance(entry.layout, sidb_layout):
        msg = f"the active layout is {TECHNOLOGIES[type(entry.layout)]}; an SiDB layout is needed"
        raise CommandError(msg)
    if unsimulated and entry.result is not None:
        index = next(
            (
                position
                for position, candidate in enumerate(session.cell_layouts)
                if candidate.result is None and candidate.layout == entry.layout
            ),
            None,
        )
        where = f"select it with 'current -c {index}'" if index is not None else "read or design it again"
        msg = f"the active element is already simulated; {where}"
        raise CommandError(msg)
    return entry.layout


def _store_result(session: Session, layout: sidb_layout, result: object, parameters: dict[str, object]) -> Result:
    if result is None:
        msg = "the simulation found no physically valid charge distribution"
        raise CommandError(msg)
    entry = CellEntry(layout, result, result.algorithm_name)  # type: ignore[attr-defined]
    session.cell_layouts.add(entry)
    description = describe(entry)
    simulation = description["simulation"]
    if isinstance(simulation, dict):
        energy = simulation["ground_state_energy_ev"]
        session.info(
            f"{simulation['engine']}: {simulation['stable_states']} stable states, ground state energy {energy:.6f} eV"
            if energy is not None
            else f"{simulation['engine']}: no physically valid charge distribution found"
        )
    return {"cell_layout": description, "parameters": parameters}


def _quickexact_arguments(parser: Parser) -> None:
    _physical_arguments(parser, base=False)
    parser.add_argument("-g", "--global-potential", type=float, default=0.0, help="global external potential in V")


@command("quickexact", Category.SIMULATION, _quickexact_arguments)
def quickexact_command(session: Session, args: argparse.Namespace) -> Result:
    """Simulate the active SiDB layout exactly with QuickExact, finding every physically valid charge distribution.

    The number of charge states per SiDB (2 or 3) is detected automatically.
    """
    layout = _active_sidb_layout(session, unsimulated=True)
    params = quickexact_params()
    parameters = _apply_physical(params.simulation_parameters, args)
    params.global_potential = args.global_potential
    parameters["global_potential"] = args.global_potential
    return _store_result(session, layout, quickexact(layout, params), parameters)


def _quicksim_arguments(parser: Parser) -> None:
    _physical_arguments(parser, base=False)
    parser.add_argument("-i", "--iterations", type=int, default=80, help="iteration steps (default: 80)")
    parser.add_argument("-a", "--alpha", type=float, default=0.7, help="alpha parameter (default: 0.7)")


@command("quicksim", Category.SIMULATION, _quicksim_arguments)
def quicksim_command(session: Session, args: argparse.Namespace) -> Result:
    """Simulate the active SiDB layout heuristically with QuickSim, approximating the ground state."""
    layout = _active_sidb_layout(session, unsimulated=True)
    if args.iterations < 1:
        msg = "the number of iterations must be at least 1"
        raise CommandError(msg)
    if not 0 < args.alpha <= 1:
        msg = "alpha must be in (0, 1]"
        raise CommandError(msg)
    params = quicksim_params()
    parameters = _apply_physical(params.simulation_parameters, args)
    params.iteration_steps = args.iterations
    params.alpha = args.alpha
    parameters.update(iterations=args.iterations, alpha=args.alpha)
    return _store_result(session, layout, quicksim(layout, params), parameters)


def _clustercomplete_arguments(parser: Parser) -> None:
    # base 3 is what ClusterComplete is for, and what the C++ shell defaulted to
    _physical_arguments(parser, base=True, base_default=3)
    parser.add_argument("-g", "--global-potential", type=float, default=0.0, help="global external potential in V")
    parser.add_argument("-w", "--witness-limit", type=int, default=6, help="witness partitioning limit (default: 6)")
    parser.add_argument("-o", "--overlap-limit", type=int, default=6, help="overlapping witnesses limit (default: 6)")
    parser.add_argument("-r", "--report-stats", action="store_true", help="report ground state space statistics")


@command("clustercomplete", Category.SIMULATION, _clustercomplete_arguments)
def clustercomplete_command(session: Session, args: argparse.Namespace) -> Result:
    """Simulate the active SiDB layout exactly with ClusterComplete, which scales to multi-gate layouts in base 3.

    The witness limits tune the first pruning stage; -r prints its statistics.
    """
    if not hasattr(pyfiction, "clustercomplete"):
        msg = "this build of pyfiction has no ALGLIB, which 'clustercomplete' needs"
        raise CommandError(msg)
    layout = _active_sidb_layout(session, unsimulated=True)
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


def _engine_argument(parser: Parser) -> None:
    parser.add_argument(
        "--engine",
        type=str.lower,
        choices=list(ENGINES),
        default="quickexact",
        help="the simulation engine (default: quickexact)",
    )


def _temp_arguments(parser: Parser) -> None:
    parser.add_argument("-c", "--confidence", type=float, default=0.99, help="confidence level (default: 0.99)")
    parser.add_argument("-t", "--max-temperature", type=float, default=400.0, help="highest temperature in K to try")
    parser.add_argument(
        "-g", "--gate-based", action="store_true", help="judge stability against the active truth table"
    )
    _physical_arguments(parser, base=True)
    _engine_argument(parser)


@command("temp", Category.SIMULATION, _temp_arguments)
def temp(session: Session, args: argparse.Namespace) -> Result:
    """Compute the critical temperature of the active SiDB layout.

    The critical temperature is where the ground state's population probability drops below the
    confidence level. With -g, the layout is a gate implementing the active truth table, and
    the erroneous states are those with a wrong output.
    """
    layout = _active_sidb_layout(session)
    if not 0 < args.confidence <= 1:
        msg = "the confidence level must be in (0, 1]"
        raise CommandError(msg)
    if args.max_temperature <= 0:
        msg = "the maximum temperature must be positive"
        raise CommandError(msg)
    params = critical_temperature_params()
    params.confidence_level = args.confidence
    params.max_temperature = args.max_temperature
    params.operational_params.sim_engine = ENGINES[args.engine]
    parameters = _apply_physical(params.operational_params.simulation_parameters, args)
    stats = critical_temperature_stats()
    if args.gate_based:
        if layout.num_pis() == 0 or layout.num_pos() == 0:
            msg = "gate-based simulation needs a layout with input and output dots"
            raise CommandError(msg)
        spec = [session.truth_tables.current()]
        temperature = critical_temperature_gate_based(layout, spec, params, stats)
    else:
        temperature = critical_temperature_non_gate_based(layout, params, stats)

    # the statistics leave the energy gap at infinity when no erroneous state exists, and JSON has no infinity
    energy_gap = stats.energy_between_ground_state_and_first_erroneous
    gap = energy_gap if math.isfinite(energy_gap) else None

    if stats.num_valid_lyt == 0:
        session.info(f"the ground state of '{layout.get_layout_name()}' could not be determined")
    else:
        bound = "> " if temperature >= args.max_temperature else ""
        session.info(f"critical temperature of '{layout.get_layout_name()}': {bound}{temperature:.2f} K")
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


def _opdom_arguments(parser: Parser) -> None:
    parser.add_argument("file", type=Path, help="the CSV file to write the domain to")
    algorithm = parser.exclusive_group()
    algorithm.add_argument("-r", "--random-sampling", type=int, metavar="N", help="sample N random points")
    algorithm.add_argument("-f", "--flood-fill", type=int, metavar="N", help="flood fill from N random points")
    algorithm.add_argument("-c", "--contour-tracing", type=int, metavar="N", help="trace contours from N random points")
    parser.add_argument("-o", "--operational-only", action="store_true", help="write only the operational points")
    parser.add_argument(
        "-s",
        "--sketch",
        action="store_true",
        help="judge points by filtering alone, without simulation; implies kink rejection",
    )
    for axis in ("x", "y", "z"):
        sweep = parser.group(f"{axis} axis")
        default = DEFAULT_SWEEPS[axis]
        sweep.add_argument(
            f"-{axis}",
            f"--{axis}-sweep",
            choices=list(SWEEPS),
            default=default[0] if axis != "z" else None,
            help=f"the parameter (default: {default[0] if axis != 'z' else 'unused'})",
        )
        sweep.add_argument(f"--{axis}-min", type=float, default=default[1], help=f"(default: {default[1]})")
        sweep.add_argument(f"--{axis}-max", type=float, default=default[2], help=f"(default: {default[2]})")
        sweep.add_argument(f"--{axis}-step", type=float, default=default[3], help=f"(default: {default[3]})")
    _physical_arguments(parser, base=True)
    _engine_argument(parser)


@command("opdom", Category.SIMULATION, _opdom_arguments)
def opdom(session: Session, args: argparse.Namespace) -> Result:
    """Compute the operational domain of the active SiDB gate and write it as CSV.

    The domain is the set of parameter points where the gate implements the active truth table.
    Grid search is the default; random sampling, flood fill, and contour tracing start from N
    random samples. The x and y axes sweep epsilon_r and lambda_tf by default; -z adds a third.
    """
    layout = _active_sidb_layout(session)
    spec = [session.truth_tables.current()]
    samples = next((n for n in (args.random_sampling, args.flood_fill, args.contour_tracing) if n is not None), None)
    if samples is not None and samples < 1:
        msg = "the number of samples must be at least 1"
        raise CommandError(msg)

    params = operational_domain_params()
    params.operational_params.sim_engine = ENGINES[args.engine]
    parameters = _apply_physical(params.operational_params.simulation_parameters, args)
    if args.sketch:
        params.operational_params.strategy_to_analyze_operational_status = operational_analysis_strategy.FILTER_ONLY
        params.operational_params.op_condition = operational_condition.REJECT_KINKS
    sweeps = []
    swept: set[str] = set()
    for axis in ("x", "y", "z"):
        name = getattr(args, f"{axis}_sweep")
        if name is None:
            continue
        if name in swept:
            msg = f"'{name}' is swept on more than one axis; every axis needs its own parameter"
            raise CommandError(msg)
        swept.add(name)
        low, high, step = (getattr(args, f"{axis}_{key}") for key in ("min", "max", "step"))
        if step <= 0 or low > high:
            msg = f"the {axis} axis needs min <= max and a positive step"
            raise CommandError(msg)
        sweeps.append(operational_domain_value_range(SWEEPS[name], low, high, step))
    params.sweep_dimensions = sweeps

    stats = operational_domain_stats()
    if args.random_sampling is not None:
        domain = operational_domain_random_sampling(layout, spec, samples, params, stats)
    elif args.flood_fill is not None:
        domain = operational_domain_flood_fill(layout, spec, samples, params, stats)
    elif args.contour_tracing is not None:
        domain = operational_domain_contour_tracing(layout, spec, samples, params, stats)
    else:
        domain = operational_domain_grid_search(layout, spec, params, stats)

    writing = write_operational_domain_params()
    writing.writing_mode = (
        sample_writing_mode.OPERATIONAL_ONLY if args.operational_only else sample_writing_mode.ALL_SAMPLES
    )
    write_operational_domain(domain, str(args.file), writing)
    session.info(
        f"{stats.num_operational_parameter_combinations} of {stats.num_evaluated_parameter_combinations} "
        f"evaluated points are operational; wrote {args.file}"
    )
    return {
        **stats_to_dict(stats),
        "engine": args.engine,
        "sketch": args.sketch,
        "sweeps": [
            {"parameter": getattr(args, f"{axis}_sweep")} for axis in ("x", "y", "z") if getattr(args, f"{axis}_sweep")
        ],
        "file": str(args.file),
        "parameters": parameters,
    }
