# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``potential_landscape``."""

from __future__ import annotations

import pytest

from mnt.pyfiction import (
    charge_distribution,
    charge_transition_threshold_bounds,
    lattice_site,
    potential_landscape,
    sidb_charge_state,
    sidb_defect,
    sidb_defect_type,
    sidb_layout,
    sidb_simulation_parameters,
    sidb_technology,
)


def three_sidbs() -> sidb_layout:
    layout = sidb_layout()
    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(5, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(7, 0, 0), sidb_technology.cell_type.NORMAL)
    return layout


def test_geometry_and_potentials():
    layout = three_sidbs()
    landscape = potential_landscape(layout, sidb_simulation_parameters(2, -0.32))

    assert landscape.num_sidbs() == 3
    assert landscape.get_layout() == layout
    assert landscape.params().base == 2
    assert landscape.defects() == []

    assert landscape.nm_distance(0, 1) == pytest.approx(1.92, abs=1e-5)
    assert landscape.nm_distance(1, 2) == pytest.approx(0.768, abs=1e-5)
    assert landscape.nm_distance(1, 1) == 0.0
    assert landscape.chargeless_potential(1, 1) == 0.0
    assert landscape.chargeless_potential(0, 1) == landscape.chargeless_potential(1, 0)
    assert landscape.chargeless_potential(1, 2) > landscape.chargeless_potential(0, 1)
    assert landscape.chargeless_potential(1, 2) == pytest.approx(landscape.chargeless_potential_at_distance(0.768))
    assert landscape.local_external_potential(0) == 0.0
    assert landscape.local_potential_caused_by_defects(0) == 0.0

    thresholds = landscape.effective_charge_transition_thresholds(0)
    assert thresholds[charge_transition_threshold_bounds.NEGATIVE_UPPER_BOUND.value] == pytest.approx(0.32, abs=1e-5)
    assert thresholds[charge_transition_threshold_bounds.POSITIVE_LOWER_BOUND.value] == pytest.approx(0.91, abs=1e-5)


def test_energies_and_validity():
    layout = three_sidbs()
    landscape = potential_landscape(layout, sidb_simulation_parameters(2, -0.32))

    all_negative = landscape.evaluate(charge_distribution(layout))
    all_neutral = landscape.evaluate(charge_distribution(layout, sidb_charge_state.NEUTRAL))

    assert all_neutral.energy() == 0.0
    assert all_negative.energy() > 0.0
    assert landscape.energy(all_negative) == pytest.approx(all_negative.energy())
    assert not landscape.is_physically_valid(all_negative)  # the two close SiDBs cannot both be negative
    assert not landscape.is_physically_valid(all_neutral)  # the isolated SiDB has to be negative

    ground = charge_distribution(layout)
    ground.assign_charge_state(lattice_site(5, 0, 0), sidb_charge_state.NEUTRAL)
    ground = landscape.evaluate(ground)

    assert landscape.is_physically_valid(ground)
    assert 0.0 < ground.energy() < all_negative.energy()

    local_potentials = landscape.local_potentials(ground)
    assert len(local_potentials) == 3
    assert local_potentials[1] < 0.0  # the neutral SiDB sits between two negative ones

    internal = landscape.local_internal_potentials(ground)
    assert len(internal) == 3
    assert internal == pytest.approx(local_potentials)  # no external potential applied


def test_external_potentials():
    layout = three_sidbs()
    params = sidb_simulation_parameters(2, -0.32)
    plain = potential_landscape(layout, params)
    shifted = potential_landscape(layout, params, {lattice_site(0, 0, 0): 0.1}, 0.2)

    assert shifted.local_external_potential(0) == pytest.approx(0.3)
    assert shifted.local_external_potential(1) == pytest.approx(0.2)

    all_negative = charge_distribution(layout)
    assert shifted.energy(all_negative) < plain.energy(all_negative)


def test_charged_defect():
    layout = sidb_layout()
    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(3, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_defect(lattice_site(1, 1, 0), sidb_defect(sidb_defect_type.SI_VACANCY, -1, 5.6, 5.0))
    layout.assign_defect(lattice_site(6, 1, 0), sidb_defect(sidb_defect_type.SILOXANE, 0))  # neutral, does not enter

    landscape = potential_landscape(layout, sidb_simulation_parameters(2, -0.32))

    assert len(landscape.defects()) == 1
    assert landscape.local_potential_caused_by_defects(0) < 0.0
    assert landscape.local_potential_caused_by_defects(0) < landscape.local_potential_caused_by_defects(1)

    neutral = charge_distribution(layout, sidb_charge_state.NEUTRAL)
    assert landscape.local_internal_potentials(neutral)[0] == pytest.approx(
        landscape.local_potential_caused_by_defects(0)
    )

    # a negative defect makes the neutral distribution more attractive than the negative one
    assert landscape.energy(charge_distribution(layout)) > landscape.energy(neutral)
