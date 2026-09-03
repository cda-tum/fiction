# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``sidb_simulation_result``."""

from __future__ import annotations

import pytest

from mnt.pyfiction import (
    charge_distribution,
    lattice,
    lattice_site,
    potential_landscape,
    sidb_charge_state,
    sidb_layout,
    sidb_simulation_parameters,
    sidb_simulation_result,
    sidb_technology,
)

LATTICES = [
    pytest.param(lattice.si_100_2x1(), id="100"),
    pytest.param(lattice.si_111_1x1(), id="111"),
]


def test_default_result():
    result = sidb_simulation_result()

    assert not result.algorithm_name
    assert result.layout == sidb_layout()
    assert result.charge_distributions == []
    assert result.additional_simulation_parameters == {}
    assert result.groundstates() == []


@pytest.mark.parametrize("lat", LATTICES)
def test_negative_and_neutral_layout(lat):
    layout = sidb_layout(lat)
    layout.assign_cell_type(lattice_site(0, 0, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(0, 1, 1), sidb_technology.cell_type.NORMAL)

    landscape = potential_landscape(layout, sidb_simulation_parameters())

    cd_negative = landscape.evaluate(charge_distribution(layout))
    cd_neutral = landscape.evaluate(charge_distribution(layout, sidb_charge_state.NEUTRAL))

    result = sidb_simulation_result()
    result.layout = layout
    result.charge_distributions = [cd_negative, cd_neutral]

    groundstate = result.groundstates()

    assert len(groundstate) == 1, "Expected exactly one ground state."
    assert groundstate[0].get_charge_state(lattice_site(0, 0, 1)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state(lattice_site(0, 1, 1)) == sidb_charge_state.NEUTRAL

    # lookup by distribution index and site
    assert result.charge_state(0, lattice_site(0, 0, 1)) == sidb_charge_state.NEGATIVE
    assert result.charge_state(1, lattice_site(0, 0, 1)) == sidb_charge_state.NEUTRAL


@pytest.mark.parametrize("lat", LATTICES)
def test_empty_layout(lat):
    layout = sidb_layout(lat)

    result = sidb_simulation_result()
    result.layout = layout
    result.charge_distributions = [charge_distribution(layout)]

    assert len(result.groundstates()) == 1


@pytest.mark.parametrize("lat", LATTICES)
def test_three_sidbs(lat):
    layout = sidb_layout(lat)
    layout.assign_cell_type(lattice_site(0, 0, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(4, 0, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(6, 0, 1), sidb_technology.cell_type.NORMAL)

    landscape = potential_landscape(layout, sidb_simulation_parameters())

    cd1 = landscape.evaluate(charge_distribution(layout))  # all negative
    cd2 = landscape.evaluate(charge_distribution(layout, sidb_charge_state.NEUTRAL))  # all neutral
    cd3 = charge_distribution(layout)
    cd3.assign_charge_state(lattice_site(6, 0, 1), sidb_charge_state.NEUTRAL)  # only two SiDBs are negative
    cd3 = landscape.evaluate(cd3)

    result = sidb_simulation_result()
    result.layout = layout
    result.charge_distributions = [cd1, cd2, cd3]

    ground_state = result.groundstates()
    assert len(ground_state) == 1

    assert ground_state[0] == cd2
    assert ground_state[0].get_charge_state(lattice_site(0, 0, 1)) == sidb_charge_state.NEUTRAL
    assert ground_state[0].get_charge_state(lattice_site(4, 0, 1)) == sidb_charge_state.NEUTRAL
    assert ground_state[0].get_charge_state(lattice_site(6, 0, 1)) == sidb_charge_state.NEUTRAL


def test_degenerate_ground_states():
    layout = sidb_layout()
    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(10, 0, 0), sidb_technology.cell_type.NORMAL)

    # two distributions with the same energy but different charge states are both ground states
    left = charge_distribution(layout, sidb_charge_state.NEUTRAL)
    left.assign_charge_state(lattice_site(0, 0, 0), sidb_charge_state.NEGATIVE)
    left.assign_energy(0.5)
    right = charge_distribution(layout, sidb_charge_state.NEUTRAL)
    right.assign_charge_state(lattice_site(10, 0, 0), sidb_charge_state.NEGATIVE)
    right.assign_energy(0.5)
    excited = charge_distribution(layout)
    excited.assign_energy(1.0)

    result = sidb_simulation_result()
    result.charge_distributions = [excited, left, right, left]

    assert len(result.groundstates()) == 2
