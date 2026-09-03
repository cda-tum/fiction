# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``charge_distribution``."""

from __future__ import annotations

from mnt.pyfiction import charge_distribution, lattice_site, sidb_charge_state, sidb_layout, sidb_technology


def three_sidbs() -> sidb_layout:
    layout = sidb_layout()
    layout.assign_cell_type(lattice_site(3, 1, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(1, 0, 1), sidb_technology.cell_type.NORMAL)
    return layout


def test_default_state():
    layout = three_sidbs()
    cd = charge_distribution(layout)

    assert cd.size() == 3
    assert len(cd) == 3
    assert not cd.empty()
    assert cd.sites() == layout.sidbs()
    assert cd.num_negative_sidbs() == 3
    assert cd.num_neutral_sidbs() == 0
    assert cd.num_positive_sidbs() == 0
    assert cd.energy() == 0.0
    assert cd.charge_exists(sidb_charge_state.NEGATIVE)
    assert not cd.charge_exists(sidb_charge_state.NEUTRAL)
    assert cd.get_charge_state(lattice_site(0, 0, 0)) == sidb_charge_state.NEGATIVE
    assert cd.get_charge_state(lattice_site(9, 9, 0)) == sidb_charge_state.NONE
    assert cd.get_charge_state_by_index(2) == sidb_charge_state.NEGATIVE
    assert cd.index_of(lattice_site(1, 0, 1)) == 1
    assert cd.index_of(lattice_site(1, 0, 0)) is None


def test_empty_distribution():
    cd = charge_distribution()

    assert cd.empty()
    assert cd.sites() == []
    assert cd.get_charge_state(lattice_site(0, 0, 0)) == sidb_charge_state.NONE


def test_assignment_by_site_and_by_index():
    layout = three_sidbs()
    cd = charge_distribution(layout, sidb_charge_state.NEUTRAL)

    cd.assign_charge_state(lattice_site(3, 1, 0), sidb_charge_state.POSITIVE)
    cd.assign_charge_state_by_index(0, sidb_charge_state.NEGATIVE)
    cd.assign_charge_state(lattice_site(9, 9, 0), sidb_charge_state.POSITIVE)  # ignored

    assert cd.charge_states() == [sidb_charge_state.NEGATIVE, sidb_charge_state.NEUTRAL, sidb_charge_state.POSITIVE]
    assert cd.num_negative_sidbs() == 1
    assert cd.num_neutral_sidbs() == 1
    assert cd.num_positive_sidbs() == 1

    cd.assign_all_charge_states(sidb_charge_state.NEGATIVE)
    assert cd.num_negative_sidbs() == 3


def test_charge_index():
    cd = charge_distribution(three_sidbs())
    assert cd.charge_index(2) == 0
    assert cd.charge_index(3) == 0

    cd.assign_charge_state_by_index(2, sidb_charge_state.NEUTRAL)
    assert cd.charge_index(2) == 1
    assert cd.charge_index(3) == 1

    cd.assign_charge_state_by_index(0, sidb_charge_state.POSITIVE)
    assert cd.charge_index(3) == 2 * 9 + 1


def test_equality_and_hash():
    layout = three_sidbs()
    a = charge_distribution(layout)
    b = charge_distribution(layout)

    assert a == b
    assert a.same_charge_states(b)
    assert hash(a) == hash(b)

    b.assign_energy(1.0)
    assert a != b
    assert a.same_charge_states(b)
    assert b.energy() == 1.0

    b.assign_charge_state_by_index(1, sidb_charge_state.NEUTRAL)
    assert not a.same_charge_states(b)

    assert len({a, b, charge_distribution(layout)}) == 2
