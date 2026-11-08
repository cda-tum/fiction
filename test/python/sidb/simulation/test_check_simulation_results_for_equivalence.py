# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``check_simulation_results_for_equivalence``."""

from __future__ import annotations

import pytest

from mnt.pyfiction.sidb import charge_distribution, lattice, lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.model import sidb_charge_state
from mnt.pyfiction.sidb.simulation import check_simulation_results_for_equivalence, sidb_simulation_result


@pytest.mark.parametrize(
    "lat",
    [pytest.param(lattice.si_100_2x1(), id="100"), pytest.param(lattice.si_111_1x1(), id="111")],
)
def test_two_sidbs(lat: lattice) -> None:
    """Equivalence compares charge states on both non-template lattices.

    Args:
        lat: Lattice to test.
    """

    layout = sidb_layout(lat)
    layout.assign_sidb(lattice_site(0, 0, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(0, 1, 1), sidb_dot_tag.NORMAL)

    first_result = sidb_simulation_result()
    second_result = sidb_simulation_result()

    first_result.charge_distributions = [charge_distribution(layout)]
    second_result.charge_distributions = [charge_distribution(layout)]

    assert check_simulation_results_for_equivalence(first_result, second_result)

    # a different charge state breaks the equivalence
    other = charge_distribution(layout)
    other.assign_charge_state(lattice_site(0, 1, 1), sidb_charge_state.NEUTRAL)
    second_result.charge_distributions = [other]

    assert not check_simulation_results_for_equivalence(first_result, second_result)

    # so does a different number of distributions
    second_result.charge_distributions = [charge_distribution(layout), other]

    assert not check_simulation_results_for_equivalence(first_result, second_result)


@pytest.mark.parametrize("difference", ["order", "energy", "sites", "size", "first_duplicate", "second_duplicate"])
def test_equivalence_observes_complete_distributions(difference: str) -> None:
    """Equivalence ignores solution order but preserves energy, geometry, and multiplicity."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site())
    negative = charge_distribution(layout)
    neutral = charge_distribution(layout, sidb_charge_state.NEUTRAL)
    first, second = sidb_simulation_result(), sidb_simulation_result()
    first.layout = second.layout = layout
    first.charge_distributions = [negative, neutral]
    second.charge_distributions = [neutral, negative]
    if difference == "energy":
        changed = charge_distribution(layout)
        changed.assign_energy(1.0)
        second.charge_distributions = [neutral, changed]
    elif difference in ("sites", "size"):
        shifted = sidb_layout()
        shifted.assign_sidb(lattice_site(1, 0))
        if difference == "size":
            shifted.assign_sidb(lattice_site(2, 0))
        second.charge_distributions = [charge_distribution(shifted, sidb_charge_state.NEUTRAL), negative]
    elif difference == "first_duplicate":
        first.charge_distributions = [negative, negative]
    elif difference == "second_duplicate":
        second.charge_distributions = [negative, negative]
    assert check_simulation_results_for_equivalence(first, second) is (difference == "order")


def test_equivalence_beyond_charge_index_range() -> None:
    """Distinct base-3 configurations stay distinct when their 64-bit indices collide."""
    digits = "11112220022122120101211020120210210211221"
    layout = sidb_layout()
    for index in range(len(digits)):
        layout.assign_sidb(lattice_site(index, 0))
    zero = charge_distribution(layout)
    wrapped = charge_distribution(layout)
    states = [sidb_charge_state.NEGATIVE, sidb_charge_state.NEUTRAL, sidb_charge_state.POSITIVE]
    for index, digit in enumerate(digits):
        wrapped.assign_charge_state_by_index(index, states[int(digit)])
    assert zero.charge_index(3) == wrapped.charge_index(3)
    first, second = sidb_simulation_result(), sidb_simulation_result()
    first.layout = second.layout = layout
    first.charge_distributions = [zero, wrapped]
    second.charge_distributions = [wrapped, zero]
    assert check_simulation_results_for_equivalence(first, first)
    assert check_simulation_results_for_equivalence(first, second)
    second.charge_distributions = [zero, zero]
    assert not check_simulation_results_for_equivalence(first, second)
