# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``calculate_energy_distribution``."""

from __future__ import annotations

import pytest

from mnt.pyfiction.sidb import charge_distribution, lattice, lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.model import sidb_charge_state
from mnt.pyfiction.sidb.simulation import potential_landscape
from mnt.pyfiction.sidb.simulation.analysis import calculate_energy_distribution, energy_distribution, energy_state


@pytest.mark.parametrize("query", ["min_energy", "max_energy"])
def test_empty_energy_distribution_rejects_extrema(query: str) -> None:
    """Empty distributions reject queries that require an energy state.

    Args:
        query: Energy extremum to request.
    """
    with pytest.raises(ValueError, match="energy distribution is empty"):
        getattr(energy_distribution(), query)()


def test_degeneracy_returns_state_count() -> None:
    """Energy lookup returns the stored state count rather than the energy-level index."""
    distribution = energy_distribution()
    distribution.add_energy_state(energy_state(-1.0, 7))
    distribution.add_energy_state(energy_state(0.0, 3))
    assert distribution.degeneracy(-1.0) == 7
    assert distribution.degeneracy(-1.0 + 1e-7) == 7
    assert distribution.degeneracy(0.0) == 3
    assert distribution.degeneracy(1.0) is None


@pytest.mark.parametrize(
    ("lat", "all_negative_energy"),
    [
        pytest.param(lattice.si_100_2x1(), 0.48066663155586997, id="100"),
        pytest.param(lattice.si_111_1x1(), 0.233980661373219, id="111"),
    ],
)
def test_three_sidbs(lat, all_negative_energy):
    layout = sidb_layout(lat)

    layout.assign_sidb(lattice_site(0, 0, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(4, 0, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(6, 0, 1), sidb_dot_tag.NORMAL)

    landscape = potential_landscape(layout, base=3, mu_minus=-0.32, epsilon_r=5.6, lambda_tf=5.0)

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

    states = []
    for i in range(distribution.size()):
        state = distribution.get_nth_state(i)
        assert state is not None
        states.append(state)
    assert all(s.degeneracy == 1 for s in states)
    assert [s.electrostatic_potential_energy for s in states] == pytest.approx(
        sorted([cd1.energy(), cd2.energy(), cd3.energy(), cd4.energy()]), abs=1e-7
    )


def test_degenerate_states():
    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(10, 0, 0), sidb_dot_tag.NORMAL)

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
    first = distribution.get_nth_state(0)
    second = distribution.get_nth_state(1)
    assert first is not None
    assert second is not None
    assert [
        (first.electrostatic_potential_energy, first.degeneracy),
        (second.electrostatic_potential_energy, second.degeneracy),
    ] == [(pytest.approx(0.5), 2), (pytest.approx(1.0), 1)]
    assert distribution.get_nth_state(2) is None


def test_duplicate_configurations_do_not_inflate_degeneracy() -> None:
    """Degeneracy counts distinct charge configurations rather than repeated input entries."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site())
    neutral = charge_distribution(layout, sidb_charge_state.NEUTRAL)
    negative = charge_distribution(layout)
    positive = charge_distribution(layout, sidb_charge_state.POSITIVE)
    positive.assign_energy(1.0)
    distribution = calculate_energy_distribution([neutral] * 3 + [negative] * 4 + [positive] * 3)
    assert distribution.size() == 2
    assert distribution.degeneracy(0.0) == 2
    assert distribution.degeneracy(1.0) == 1


def test_empty_configuration_has_one_zero_energy_state() -> None:
    """An empty configuration is distinct from an absent distribution."""
    distribution = calculate_energy_distribution([charge_distribution()])
    assert distribution.size() == 1
    state = distribution.get_nth_state(0)
    assert state is not None
    assert (state.electrostatic_potential_energy, state.degeneracy) == (0.0, 1)
