# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The opdom command."""

from __future__ import annotations

from functools import partial
from pathlib import Path
from typing import TYPE_CHECKING

from rich.table import Table

from mnt.fiction.cli.commands.io.tt import _table_from_string
from mnt.fiction.cli.errors import CommandError
from mnt.fiction.cli.parsing import finite_float, integer, positive_float, positive_int
from mnt.fiction.cli.registry import Category, command
from mnt.fiction.cli.statistics import stats_to_dict
from mnt.pyfiction import synthesis
from mnt.pyfiction.sidb import sidb_dot_tag
from mnt.pyfiction.sidb.simulation.io import (
    sample_writing_mode,
    write_operational_domain,
    write_operational_domain_params,
)
from mnt.pyfiction.sidb.simulation.logic import (
    bdl_wire_selection,
    detect_bdl_pairs,
    detect_bdl_wires,
    input_bdl_configuration,
    operational_analysis_strategy,
    operational_condition,
    operational_domain,
    operational_domain_contour_tracing,
    operational_domain_flood_fill,
    operational_domain_grid_search,
    operational_domain_params,
    operational_domain_random_sampling,
    operational_domain_stats,
    operational_domain_value_range,
    sweep_parameter,
)

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session
    from mnt.pyfiction.sidb import sidb_layout
    from mnt.pyfiction.synthesis import dynamic_truth_table
from mnt.fiction.cli.opdom_plotting import plot_arguments, validate_outputs, write_plot

from ._common import ENGINES, _active_sidb_layout, _apply_physical, _engine_argument, _physical_arguments

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


GATES = {
    "id": synthesis.create_id_tt,
    "not": synthesis.create_not_tt,
    "and": synthesis.create_and_tt,
    "nand": synthesis.create_nand_tt,
    "or": synthesis.create_or_tt,
    "nor": synthesis.create_nor_tt,
    "xor": synthesis.create_xor_tt,
    "xnor": synthesis.create_xnor_tt,
    "maj": synthesis.create_maj_tt,
}
"""Named single-output gate specifications."""

TWO_CHARGE_STATES = 2
"""Charge-state count supported by QuickSim."""

MAX_BDL_INPUTS = 63
"""Maximum input count representable by the native pattern iterator."""

MAX_EXPRESSION_INPUTS = 16
"""Maximum input count of an expression: variables run from ``a`` to ``p``."""


def _opdom_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument("file", type=Path, help="the CSV file to write the domain to")
    domain_arguments(parser)


def domain_arguments(parser: Parser, *, require_spec: bool = False) -> None:
    """Add shared operational domain computation options.

    Args:
        parser: The command parser.
        require_spec: Require an explicit logic specification.
    """
    logic = parser.add_mutually_exclusive_group(required=require_spec)
    logic.add_argument("--gate", choices=list(GATES), help="expected gate function")
    logic.add_argument(
        "--table", action="append", metavar="BITS", help="binary or 0x-prefixed hex table; repeat per output"
    )
    logic.add_argument("--expression", action="append", metavar="EXPR", help="Boolean expression; repeat per output")
    algorithm = parser.add_mutually_exclusive_group()
    algorithm.add_argument(
        "-g", "--grid-search", action="store_true", help="reconstruct the domain by grid search; the default"
    )
    algorithm.add_argument("-r", "--random-sampling", type=positive_int, metavar="N", help="sample N random points")
    algorithm.add_argument("-f", "--flood-fill", type=positive_int, metavar="N", help="flood fill from N random points")
    algorithm.add_argument(
        "-c", "--contour-tracing", type=positive_int, metavar="N", help="trace contours from N random points"
    )
    parser.add_argument(
        "-s",
        "--sketch",
        action="store_true",
        help="judge points by filtering alone, without simulation; implies kink rejection",
    )
    parser.add_argument("-o", "--operational-only", action="store_true", help="write only the operational points")
    for axis in ("x", "y", "z"):
        sweep = parser.add_argument_group(f"{axis} axis")
        default = DEFAULT_SWEEPS[axis]
        sweep.add_argument(
            f"-{axis}",
            f"--{axis}-sweep",
            choices=list(SWEEPS),
            default=default[0] if axis != "z" else None,
            help="the parameter",
        )
        sweep.add_argument(f"--{axis}-min", type=finite_float, default=default[1], help="lower bound of the sweep")
        sweep.add_argument(f"--{axis}-max", type=finite_float, default=default[2], help="upper bound of the sweep")
        sweep.add_argument(f"--{axis}-step", type=finite_float, default=default[3], help="step between samples")
    _physical_arguments(parser, base=True)
    _engine_argument(parser)

    defaults = operational_domain_params()
    operational = defaults.operational_params
    wires = operational.input_bdl_iterator_params.bdl_wire_params
    advanced = parser.add_argument_group("operational analysis")
    advanced.add_argument("--threads", type=positive_int, default=defaults.number_of_threads, help="worker threads")
    advanced.add_argument(
        "--timeout-ms",
        type=partial(integer, maximum=2**64 - 1),
        help="complete computation budget in milliseconds; unlimited by default",
    )
    advanced.add_argument(
        "--condition", choices=["tolerate_kinks", "reject_kinks"], help="kink handling; Sketch requires rejection"
    )
    advanced.add_argument(
        "--strategy",
        choices=["simulation_only", "filter_only", "filter_then_simulation"],
        help="analysis strategy; simulation_only by default",
    )
    advanced.add_argument(
        "--input-encoding", choices=["distance", "absence"], default="distance", help="BDL input encoding"
    )
    advanced.add_argument(
        "--bdl-pair-min",
        type=finite_float,
        default=wires.bdl_pairs_params.minimum_distance,
        help="minimum BDL pair distance in nm",
    )
    advanced.add_argument(
        "--bdl-pair-max",
        type=finite_float,
        default=wires.bdl_pairs_params.maximum_distance,
        help="maximum BDL pair distance in nm",
    )
    advanced.add_argument(
        "--bdl-wire-distance",
        type=positive_float,
        default=wires.threshold_bdl_interdistance,
        help="BDL wire distance threshold in nm",
    )
    plot_arguments(parser)


@command(
    "opdom",
    Category.SIMULATION,
    _opdom_arguments,
    inputs="Active SiDB layout; gate checks also use the active truth table.",
    example='read and.sqd; tt -e "(ab)"; opdom domain.csv',
    progress=True,
)
def opdom(session: Session, args: argparse.Namespace) -> Result:
    """Compute the operational domain of the active SiDB gate and export CSV or plots.

    The domain is the set of parameter points where the gate implements the expected functions.
    Grid search is the default; random sampling, flood fill, and contour tracing start from N
    random samples. The x and y axes sweep epsilon_r and lambda_tf by default; -z adds a third.
    """
    layout = _active_sidb_layout(session)
    samples = next((n for n in (args.random_sampling, args.flood_fill, args.contour_tracing) if n is not None), None)

    params, parameters = domain_parameters(session, args)
    spec = gate_specification(session, layout, args, params)
    validate_outputs(args, log_path=session.log_path)
    domain, stats = compute_domain(layout, spec, args, params)
    write_csv(domain, args)
    for path in args.plot:
        write_plot(domain, path, args)
        session.info(f"wrote {path}")
    report_statistics(session, stats, sketch=args.sketch)
    if args.file is not None:
        session.info(
            f"{stats.num_operational_parameter_combinations} of {stats.num_evaluated_parameter_combinations} "
            f"evaluated points are operational; wrote {args.file}"
        )
    return {
        **stats_to_dict(stats),
        "engine": args.engine,
        "grid_search": samples is None,
        "sketch": args.sketch,
        "sweeps": [
            {"parameter": getattr(args, f"{axis}_sweep")} for axis in ("x", "y", "z") if getattr(args, f"{axis}_sweep")
        ],
        "file": str(args.file) if args.file is not None else None,
        "plots": [str(path) for path in args.plot],
        "parameters": parameters,
        "threads": params.number_of_threads,
        "operational_parameters": stats_to_dict(params.operational_params),
    }


def domain_parameters(
    session: Session, args: argparse.Namespace
) -> tuple[operational_domain_params, dict[str, object]]:
    """Build native parameters and describe the fixed physical values.

    Args:
        session: Supplies progress callbacks.
        args: Parsed domain options.

    Returns:
        Native parameters and physical values for the log.
    """
    params = operational_domain_params()
    params.on_progress = session.report_progress
    params.on_worker_progress = session.report_worker_progress
    params.operational_params.sim_engine = ENGINES[args.engine]
    parameters = _apply_physical(params.operational_params.simulation_parameters, args)
    operational = params.operational_params
    if args.sketch and args.strategy not in {None, "filter_only"}:
        msg = "--sketch requires --strategy filter_only"
        raise CommandError(msg)
    args.sketch = args.sketch or args.strategy == "filter_only"
    if args.sketch and args.condition == "tolerate_kinks":
        msg = "Sketch requires --condition reject_kinks"
        raise CommandError(msg)
    operational.strategy_to_analyze_operational_status = operational_analysis_strategy[
        (args.strategy or ("filter_only" if args.sketch else "simulation_only")).upper()
    ]
    operational.op_condition = operational_condition[
        (args.condition or ("reject_kinks" if args.sketch else "tolerate_kinks")).upper()
    ]
    if args.engine == "quicksim" and args.base != TWO_CHARGE_STATES:
        msg = "QuickSim requires --base 2"
        raise CommandError(msg)
    if args.timeout_ms is not None:
        if args.engine == "clustercomplete" and args.timeout_ms != 2**64 - 1:
            msg = "ClusterComplete does not support a finite timeout"
            raise CommandError(msg)
        operational.timeout = args.timeout_ms
    params.number_of_threads = args.threads
    inputs = operational.input_bdl_iterator_params
    inputs.input_bdl_config = (
        input_bdl_configuration.PERTURBER_DISTANCE_ENCODED
        if args.input_encoding == "distance"
        else input_bdl_configuration.PERTURBER_ABSENCE_ENCODED
    )
    wires = inputs.bdl_wire_params
    if not 0 <= args.bdl_pair_min <= args.bdl_pair_max:
        msg = "BDL pair distances require 0 <= min <= max"
        raise CommandError(msg)
    wires.bdl_pairs_params.minimum_distance = args.bdl_pair_min
    wires.bdl_pairs_params.maximum_distance = args.bdl_pair_max
    wires.threshold_bdl_interdistance = args.bdl_wire_distance
    params.sweep_dimensions = _sweep_dimensions(args)

    return params, parameters


def compute_domain(
    layout: sidb_layout,
    spec: list[dynamic_truth_table],
    args: argparse.Namespace,
    params: operational_domain_params,
) -> tuple[operational_domain, operational_domain_stats]:
    """Run the selected reconstruction method once.

    Args:
        layout: The gate layout.
        spec: Expected functions, one per output.
        args: Selects the algorithm and sample count.
        params: Native domain parameters.

    Returns:
        The domain and native statistics.
    """
    stats = operational_domain_stats()
    if args.random_sampling is not None:
        domain = operational_domain_random_sampling(layout, spec, args.random_sampling, params, stats)
    elif args.flood_fill is not None:
        domain = operational_domain_flood_fill(layout, spec, args.flood_fill, params, stats)
    elif args.contour_tracing is not None:
        domain = operational_domain_contour_tracing(layout, spec, args.contour_tracing, params, stats)
    else:
        domain = operational_domain_grid_search(layout, spec, params, stats)

    return domain, stats


def write_csv(domain: operational_domain, args: argparse.Namespace) -> None:
    """Write the domain with the requested sample visibility.

    Args:
        domain: The computed samples.
        args: The output path and writing options.
    """
    if args.file is None:
        return
    writing = write_operational_domain_params()
    writing.writing_mode = (
        sample_writing_mode.OPERATIONAL_ONLY if args.operational_only else sample_writing_mode.ALL_SAMPLES
    )
    write_operational_domain(domain, str(args.file), writing)


def gate_specification(
    session: Session, layout: sidb_layout, args: argparse.Namespace, params: operational_domain_params
) -> list[dynamic_truth_table]:
    """Resolve expected functions and validate the gate's detected BDL ports.

    Args:
        session: Supplies the active truth table when no override is given.
        layout: The gate layout.
        args: Named gate, tables, or expressions.
        params: Supplies the BDL detection settings and analysis strategy.

    Returns:
        One truth table per output with the detected input width.

    Raises:
        CommandError: Ports or specifications do not match, or Sketch lacks a canvas.
    """
    wire_params = params.operational_params.input_bdl_iterator_params.bdl_wire_params
    inputs = detect_bdl_wires(layout, wire_params, bdl_wire_selection.INPUT)
    outputs = detect_bdl_pairs(layout, sidb_dot_tag.OUTPUT, wire_params.bdl_pairs_params)
    if not inputs or not outputs:
        msg = "the layout needs detected BDL input and output ports"
        raise CommandError(msg)
    if len(inputs) > MAX_BDL_INPUTS:
        msg = "the layout supports at most 63 BDL inputs"
        raise CommandError(msg)
    _validate_layout(layout, args)
    if args.gate:
        spec = [GATES[args.gate]()]
    elif args.table:
        spec = [_table_from_string(text) for text in args.table]
    elif args.expression:
        if len(inputs) > MAX_EXPRESSION_INPUTS:
            msg = f"--expression supports at most {MAX_EXPRESSION_INPUTS} BDL inputs"
            raise CommandError(msg)
        spec = []
        for expression in args.expression:
            if any("a" <= char <= "p" and ord(char) - ord("a") >= len(inputs) for char in expression):
                msg = "an expression references an input absent from the layout"
                raise CommandError(msg)
            table = synthesis.dynamic_truth_table(len(inputs))
            table.create_from_expression(expression)
            spec.append(table)
    else:
        spec = [session.truth_tables.current()]
    if len(spec) != len(outputs) or any(table.num_vars() != len(inputs) for table in spec):
        msg = f"the specification must have {len(outputs)} output table(s), each with {len(inputs)} input(s)"
        raise CommandError(msg)
    return spec


def _validate_layout(layout: sidb_layout, args: argparse.Namespace) -> None:
    """Reject layout features unsupported by the requested analysis.

    Args:
        layout: The gate layout.
        args: Engine and Sketch options.

    Raises:
        CommandError: QuickSim encounters charged defects or Sketch has no canvas.
    """
    if args.engine == "quicksim" and layout.num_positively_charged_defects() + layout.num_negatively_charged_defects():
        msg = "QuickSim does not support charged defects"
        raise CommandError(msg)
    if args.sketch and not layout.dots_with_tag(sidb_dot_tag.LOGIC):
        msg = "Sketch requires LOGIC dots for its canvas"
        raise CommandError(msg)


def report_statistics(session: Session, stats: operational_domain_stats, *, sketch: bool) -> None:
    """Print native computation statistics, including in quiet mode.

    Args:
        session: Supplies the Rich console.
        stats: Native computation statistics.
        sketch: Label positive classifications as potentially operational.
    """
    table = Table(title="Operational domain sketch" if sketch else "Operational domain", show_header=False)
    table.add_column("Statistic", style="cyan")
    table.add_column("Value", justify="right")
    for label, value in (
        ("Computation runtime (s)", f"{stats.time_total.total_seconds():.6f}"),
        ("Samples evaluated", str(stats.num_evaluated_parameter_combinations)),
        ("Potentially operational" if sketch else "Operational", str(stats.num_operational_parameter_combinations)),
        ("Non-operational", str(stats.num_non_operational_parameter_combinations)),
        ("Total parameter points", str(stats.num_total_parameter_points)),
        ("Simulator calls", str(stats.num_simulator_invocations)),
    ):
        table.add_row(label, value)
    session.console.print(table)


def _sweep_dimensions(args: argparse.Namespace) -> list[operational_domain_value_range]:
    """Validate and construct the physical parameter sweeps.

    Args:
        args: Operational-domain options.

    Returns:
        Distinct, bounded sweeps with positive steps.

    Raises:
        CommandError: An axis is repeated or has invalid bounds.
    """
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
        if name in {"epsilon_r", "lambda_tf"} and low <= 0:
            msg = f"the {axis} axis needs positive {name} values"
            raise CommandError(msg)
        if step <= 0 or low > high:
            msg = f"the {axis} axis needs min <= max and a positive step"
            raise CommandError(msg)
        sweeps.append(operational_domain_value_range(SWEEPS[name], low, high, step))
    return sweeps
