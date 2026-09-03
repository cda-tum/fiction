# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``calculate_energy_distribution``."""

from __future__ import annotations

from itertools import pairwise

import pytest

from mnt.pyfiction import (
    calculate_energy_distribution,
    charge_distribution,
    lattice,
    lattice_site,
    potential_landscape,
    sidb_charge_state,
    sidb_layout,
    sidb_simulation_parameters,
    sidb_technology,
)


@pytest.mark.parametrize(
    ("lat", "all_negative_energy"),
    [
        pytest.param(lattice.si_100_2x1(), 0.48066663155586997, id="100"),
        pytest.param(lattice.si_111_1x1(), 0.233980661373219, id="111"),
    ],
)
def test_three_sidbs(lat, all_negative_energy):
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
    cd4 = charge_distribution(layout)
    cd4.assign_charge_state(lattice_site(0, 0, 1), sidb_charge_state.NEUTRAL)
    cd4.assign_charge_state(lattice_site(4, 0, 1), sidb_charge_state.POSITIVE)
    cd4.assign_charge_state(lattice_site(6, 0, 1), sidb_charge_state.NEGATIVE)
    cd4 = landscape.evaluate(cd4)

    assert cd1.energy() == pytest.approx(all_negative_energy, abs=1e-7)
    assert cd2.energy() == pytest.approx(0.0, abs=1e-7)
    assert 0.0 < cd3.energy() < cd1.energy()
    assert cd4.energy() < 0.0  # the positive SiDB attracts its negative neighbor

    distribution = calculate_energy_distribution([cd1, cd2, cd3, cd4])

    assert distribution.size() == 4
    assert distribution.min_energy() == pytest.approx(cd4.energy(), abs=1e-7)
    assert distribution.max_energy() == pytest.approx(cd1.energy(), abs=1e-7)

    states = [distribution.get_nth_state(i) for i in range(distribution.size())]
    assert all(s.degeneracy == 1 for s in states)
    assert all(
        lower.electrostatic_potential_energy < higher.electrostatic_potential_energy
        for lower, higher in pairwise(states)
    )


def test_degenerate_states():
    layout = sidb_layout()
    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(10, 0, 0), sidb_technology.cell_type.NORMAL)

    left = charge_distribution(layout, sidb_charge_state.NEUTRAL)
    left.assign_charge_state(lattice_site(0, 0, 0), sidb_charge_state.NEGATIVE)
    left.assign_energy(0.5)
    right = charge_distribution(layout, sidb_charge_state.NEUTRAL)
    right.assign_charge_state(lattice_site(10, 0, 0), sidb_charge_state.NEGATIVE)
    right.assign_energy(0.5)
    excited = charge_distribution(layout)
    excited.assign_energy(1.0)

    distribution = calculate_energy_distribution([excited, left, right])

    assert distribution.size() == 2
    assert distribution.get_nth_state(0).electrostatic_potential_energy == pytest.approx(0.5)
    assert distribution.get_nth_state(0).degeneracy == 2
    assert distribution.get_nth_state(1).degeneracy == 1
