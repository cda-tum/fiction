# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``exhaustive_ground_state_simulation``."""

from __future__ import annotations

from mnt.pyfiction import (
    exhaustive_ground_state_simulation,
    lattice,
    lattice_site,
    sidb_charge_state,
    sidb_layout,
    sidb_simulation_parameters,
    sidb_technology,
)


def test_perturber_and_sidb_pair() -> None:
    """ExGS finds the H-Si(100)-2x1 ground state."""

    layout = sidb_layout()
    layout.assign_cell_type(lattice_site(0, 0, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(4, 0, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(6, 0, 1), sidb_technology.cell_type.NORMAL)

    result = exhaustive_ground_state_simulation(layout, sidb_simulation_parameters())

    assert result.algorithm_name == "ExGS"
    assert result.layout == layout
    assert len(result.charge_distributions) == 1

    groundstate = result.charge_distributions[0]

    assert groundstate.get_charge_state(lattice_site(0, 0, 1)) == sidb_charge_state.NEGATIVE
    assert groundstate.get_charge_state(lattice_site(4, 0, 1)) == sidb_charge_state.NEUTRAL
    assert groundstate.get_charge_state(lattice_site(6, 0, 1)) == sidb_charge_state.NEGATIVE


def test_perturber_and_sidb_pair_111() -> None:
    """ExGS finds the H-Si(111)-1x1 ground state."""

    layout = sidb_layout(lattice.si_111_1x1())
    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(1, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(2, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(3, 0, 0), sidb_technology.cell_type.NORMAL)

    params = sidb_simulation_parameters()
    params.mu_minus = -0.32
    params.base = 2

    result = exhaustive_ground_state_simulation(layout, params)

    assert result.algorithm_name == "ExGS"

    groundstate = result.groundstates()

    assert len(groundstate) == 1

    assert groundstate[0].get_charge_state(lattice_site(0, 0, 0)) == sidb_charge_state.NEGATIVE
    assert groundstate[0].get_charge_state(lattice_site(1, 0, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state(lattice_site(2, 0, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state(lattice_site(3, 0, 0)) == sidb_charge_state.NEGATIVE
