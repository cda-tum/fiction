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

from mnt.pyfiction import (
    sidb_layout,
    sidb_simulation_engine,
)
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.stores import TECHNOLOGIES, CellEntry, describe

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.registry import Parser, Result
    from mnt.pyfiction.cli.session import Session


ENGINES = {name.lower(): member for name, member in sidb_simulation_engine.__members__.items()}
"""The ``--engine`` names and the engines they select; ``clustercomplete`` exists only with ALGLIB."""


def _physical_arguments(parser: Parser, *, base: bool, base_default: int = 2, potential: bool = False) -> None:
    """Add the shared physical parameters and optional base and potential arguments."""
    physics = parser.group("physical parameters")
    physics.add_argument("-e", "--epsilon-r", type=float, default=5.6, help="relative permittivity")
    physics.add_argument("-l", "--lambda-tf", type=float, default=5.0, help="Thomas-Fermi screening length in nm")
    physics.add_argument("-m", "--mu-minus", type=float, default=-0.32, help="energy transition level (0/-) in eV")
    if potential:
        physics.add_argument("-g", "--global-potential", type=float, default=0.0, help="global external potential in V")
    if base:
        physics.add_argument(
            "--base",
            type=int,
            choices=[2, 3],
            default=base_default,
            help="charge states per SiDB",
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
                for position, candidate in enumerate(session.cell_layouts, start=1)
                if candidate.result is None and candidate.layout == entry.layout
            ),
            None,
        )
        where = f"select it with 'current -c {index}'" if index is not None else "read or design it again"
        msg = f"the active element is already simulated; {where}"
        raise CommandError(msg)
    return entry.layout


def _store_result(session: Session, layout: sidb_layout, result: object, parameters: dict[str, object]) -> Result:
    """Store the simulation result and describe its physical parameters."""
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


def _engine_argument(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument(
        "--engine",
        type=str.lower,
        choices=list(ENGINES),
        default="quickexact",
        help="the simulation engine",
    )
