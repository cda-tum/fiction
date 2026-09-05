# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``is_ground_state``."""

from __future__ import annotations

import pytest

from mnt.pyfiction import (
    charge_distribution,
    is_ground_state,
    lattice,
    lattice_site,
    potential_landscape,
    quickexact,
    quickexact_params,
    sidb_100_lattice,
    sidb_111_lattice,
    sidb_charge_state,
    sidb_layout,
    sidb_simulation_parameters,
    sidb_simulation_result,
    sidb_technology,
)


@pytest.mark.parametrize(
    "lat",
    [pytest.param(lattice.si_100_2x1(), id="100"), pytest.param(lattice.si_111_1x1(), id="111")],
)
def test_three_sidbs(lat: lattice) -> None:
    """Ground-state matching supports both non-template lattices.

    Args:
        lat: Lattice to test.
    """

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

    exact_results = sidb_simulation_result()
    exact_results.layout = layout
    exact_results.charge_distributions = [cd1, cd2, cd3, cd4]

    heuristic_results = sidb_simulation_result()
    heuristic_results.layout = layout
    heuristic_results.charge_distributions = [cd4]

    assert is_ground_state(heuristic_results, exact_results)

    heuristic_results.charge_distributions = [cd1]

    assert not is_ground_state(heuristic_results, exact_results)


def test_empty_results() -> None:
    """Empty results contain no ground state."""

    assert not is_ground_state(sidb_simulation_result(), sidb_simulation_result())


@pytest.mark.parametrize(
    "layout",
    [pytest.param(sidb_100_lattice(), id="100"), pytest.param(sidb_111_lattice(), id="111")],
)
def test_legacy_results(layout: sidb_100_lattice | sidb_111_lattice) -> None:
    """Ground-state matching accepts transitional simulation results.

    Args:
        layout: Cell-level layout to test.
    """

    layout.assign_cell_type((0, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((2, 0, 0), sidb_technology.cell_type.NORMAL)

    result = quickexact(layout, quickexact_params())

    assert is_ground_state(result, result)
