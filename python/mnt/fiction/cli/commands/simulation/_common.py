# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Shared command helpers."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.errors import CommandError
from mnt.fiction.cli.parsing import finite_float, integer, positive_float
from mnt.fiction.cli.stores import TECHNOLOGIES, CellEntry, describe
from mnt.pyfiction.sidb import sidb_layout
from mnt.pyfiction.sidb.simulation import sidb_simulation_engine, sidb_simulation_result

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session
    from mnt.pyfiction.sidb.model import sidb_simulation_parameters


ENGINES = {name.lower(): member for name, member in sidb_simulation_engine.__members__.items()}
"""The ``--engine`` names and the engines they select; ``clustercomplete`` exists only with ALGLIB."""


def _physical_arguments(parser: Parser, *, base: bool, base_default: int = 2, potential: bool = False) -> None:
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


def _apply_physical(params: sidb_simulation_parameters, args: argparse.Namespace) -> dict[str, object]:
    """Copy the physical parameters of ``args`` into a ``sidb_simulation_parameters`` and describe them."""
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


def _active_sidb_layout(session: Session) -> sidb_layout:
    """Return the active SiDB layout.

    Args:
        session: The session.

    Returns:
        The layout.

    Raises:
        CommandError: When the active element is of another technology.
    """
    entry = session.cell_layouts.current()
    if not isinstance(entry.layout, sidb_layout):
        msg = f"the active layout is {TECHNOLOGIES[type(entry.layout)]}; an SiDB layout is needed"
        raise CommandError(msg)
    return entry.layout


def _store_result(
    session: Session, layout: sidb_layout, result: sidb_simulation_result | None, parameters: dict[str, object]
) -> Result:
    """Store the simulation result and describe its physical parameters."""
    if result is None:
        msg = "the simulation found no physically valid charge distribution"
        raise CommandError(msg)
    entry = CellEntry(layout, result, result.algorithm_name)
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


def _engine_argument(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument(
        "--engine",
        type=str.lower,
        choices=list(ENGINES),
        default="quickexact",
        help="the simulation engine",
    )
