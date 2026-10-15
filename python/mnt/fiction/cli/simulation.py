# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Physical simulation options shared by the shell and dedicated workflows."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction.sidb.simulation import sidb_simulation_engine

from .parsing import finite_float, integer, positive_float

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.sidb.model import sidb_simulation_parameters

    from .parsing import Parser

ENGINES = {name.lower(): member for name, member in sidb_simulation_engine.__members__.items()}
"""The ``--engine`` names and the engines they select; ``clustercomplete`` exists only with ALGLIB."""


def physical_arguments(parser: Parser, *, base: bool, base_default: int = 2, potential: bool = False) -> None:
    """Add shared physical simulation arguments.

    Args:
        parser: The command parser.
        base: Include the charge-state option.
        base_default: The default number of charge states.
        potential: Include the global-potential option.
    """
    physics = parser.add_argument_group("physical parameters")
    physics.add_argument("-e", "--epsilon-r", type=positive_float, default=5.6, help="relative permittivity")
    physics.add_argument(
        "-l", "--lambda-tf", type=positive_float, default=5.0, help="Thomas-Fermi screening length in nm"
    )
    physics.add_argument(
        "-m", "--mu-minus", type=finite_float, default=-0.32, help="energy transition level (0/-) in eV"
    )
    if potential:
        physics.add_argument(
            "-g", "--global-potential", type=finite_float, default=0.0, help="global external potential in V"
        )
    if base:
        physics.add_argument(
            "--base",
            type=integer,
            choices=[2, 3],
            default=base_default,
            help="charge states per SiDB",
        )


def apply_physical(params: sidb_simulation_parameters, args: argparse.Namespace) -> dict[str, object]:
    """Copy the physical parameters into the native simulation parameters.

    Returns:
        The fixed physical values for statistics logging.
    """
    params.epsilon_r = args.epsilon_r
    params.lambda_tf = args.lambda_tf
    params.mu_minus = args.mu_minus
    description: dict[str, object] = {
        "epsilon_r": args.epsilon_r,
        "lambda_tf": args.lambda_tf,
        "mu_minus": args.mu_minus,
    }
    if hasattr(args, "base"):
        params.base = args.base
        description["base"] = args.base
    return description


def engine_argument(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument(
        "--engine",
        type=str.lower,
        choices=list(ENGINES),
        default="quickexact",
        help="the simulation engine",
    )
