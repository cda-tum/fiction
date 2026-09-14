# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The opdom command."""

from __future__ import annotations

from pathlib import Path
from typing import TYPE_CHECKING

from mnt.pyfiction import (
    operational_analysis_strategy,
    operational_condition,
    operational_domain_contour_tracing,
    operational_domain_flood_fill,
    operational_domain_grid_search,
    operational_domain_params,
    operational_domain_random_sampling,
    operational_domain_stats,
    operational_domain_value_range,
    sample_writing_mode,
    sweep_parameter,
    write_operational_domain,
    write_operational_domain_params,
)
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.parsing import finite_float, positive_int
from mnt.pyfiction.cli.registry import Category, command
from mnt.pyfiction.cli.statistics import stats_to_dict

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.parsing import Parser
    from mnt.pyfiction.cli.registry import Result
    from mnt.pyfiction.cli.session import Session
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


def _opdom_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument("file", type=Path, help="the CSV file to write the domain to")
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


@command(
    "opdom",
    Category.SIMULATION,
    _opdom_arguments,
    inputs="Active SiDB layout; gate checks also use the active truth table.",
    example='read and.sqd; tt -e "(ab)"; opdom domain.csv',
)
def opdom(session: Session, args: argparse.Namespace) -> Result:
    """Compute the operational domain of the active SiDB gate and write it as CSV.

    The domain is the set of parameter points where the gate implements the active truth table.
    Grid search is the default; random sampling, flood fill, and contour tracing start from N
    random samples. The x and y axes sweep epsilon_r and lambda_tf by default; -z adds a third.
    """
    layout = _active_sidb_layout(session)
    spec = [session.truth_tables.current()]
    samples = next((n for n in (args.random_sampling, args.flood_fill, args.contour_tracing) if n is not None), None)

    params = operational_domain_params()
    params.operational_params.sim_engine = ENGINES[args.engine]
    parameters = _apply_physical(params.operational_params.simulation_parameters, args)
    if args.sketch:
        params.operational_params.strategy_to_analyze_operational_status = operational_analysis_strategy.FILTER_ONLY
        params.operational_params.op_condition = operational_condition.REJECT_KINKS
    params.sweep_dimensions = _sweep_dimensions(args)

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
        "grid_search": samples is None,
        "sketch": args.sketch,
        "sweeps": [
            {"parameter": getattr(args, f"{axis}_sweep")} for axis in ("x", "y", "z") if getattr(args, f"{axis}_sweep")
        ],
        "file": str(args.file),
        "parameters": parameters,
    }


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
