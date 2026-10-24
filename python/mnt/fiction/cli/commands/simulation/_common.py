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
from mnt.fiction.cli.stores import TECHNOLOGIES, CellEntry, describe
from mnt.pyfiction.sidb import sidb_layout

if TYPE_CHECKING:
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session
    from mnt.pyfiction.sidb.simulation import sidb_simulation_result


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
