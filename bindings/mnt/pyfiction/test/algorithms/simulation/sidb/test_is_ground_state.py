from __future__ import annotations

import pytest

from mnt.pyfiction import (
    charge_distribution_surface_100,
    charge_distribution_surface_111,
    is_ground_state,
    sidb_100_lattice,
    sidb_111_lattice,
    sidb_charge_state,
    sidb_simulation_parameters,
    sidb_simulation_result_100,
    sidb_simulation_result_111,
    sidb_technology,
)


@pytest.mark.parametrize(
    ("sidb_lattice", "charge_distribution_surface", "sidb_simulation_result"),
    [
        pytest.param(sidb_100_lattice, charge_distribution_surface_100, sidb_simulation_result_100, id="100"),
        pytest.param(sidb_111_lattice, charge_distribution_surface_111, sidb_simulation_result_111, id="111"),
    ],
)
def test_three_sidbs(sidb_lattice, charge_distribution_surface, sidb_simulation_result):
    layout = sidb_lattice((10, 10))

    layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((4, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((6, 1), sidb_technology.cell_type.NORMAL)

    cds1 = charge_distribution_surface(layout)  # all negative
    cds2 = charge_distribution_surface(
        layout,
        sidb_simulation_parameters(),
        sidb_charge_state.NEUTRAL,
    )  # all neutral
    cds3 = charge_distribution_surface(layout)
    cds3.assign_charge_state((6, 1), sidb_charge_state.NEUTRAL)  # only two SiDBs are negative
    cds3.update_after_charge_change()

    cds4 = charge_distribution_surface(layout)
    cds4.assign_charge_state((0, 1), sidb_charge_state.NEUTRAL)
    cds4.assign_charge_state((4, 1), sidb_charge_state.POSITIVE)
    cds4.assign_charge_state((6, 1), sidb_charge_state.NEGATIVE)
    cds4.update_after_charge_change()

    exact_results = sidb_simulation_result()
    exact_results.charge_distributions = [cds1, cds2, cds3, cds4]

    heuristic_results = sidb_simulation_result()
    heuristic_results.charge_distributions = [cds4]

    assert is_ground_state(heuristic_results, exact_results)
