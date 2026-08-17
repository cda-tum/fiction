import pytest

from mnt.pyfiction import (
    charge_distribution_surface_100,
    charge_distribution_surface_111,
    exhaustive_ground_state_simulation,
    sidb_100_lattice,
    sidb_111_lattice,
    sidb_charge_state,
    sidb_simulation_parameters,
    sidb_technology,
)


def test_perturber_and_sidb_pair():
    layout = sidb_100_lattice((10, 10))
    layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((4, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((6, 1), sidb_technology.cell_type.NORMAL)

    params = sidb_simulation_parameters()

    charge_distribution_surface_100(layout)

    result = exhaustive_ground_state_simulation(layout, params)

    assert result.algorithm_name == "ExGS"
    assert len(result.charge_distributions) == 1

    groundstate = result.charge_distributions[0]

    assert groundstate.get_charge_state((0, 1)) == sidb_charge_state.NEGATIVE
    assert groundstate.get_charge_state((4, 1)) == sidb_charge_state.NEUTRAL
    assert groundstate.get_charge_state((6, 1)) == sidb_charge_state.NEGATIVE


def test_perturber_and_sidb_pair_111():
    layout = sidb_111_lattice((4, 1))
    layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((1, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((2, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((3, 0), sidb_technology.cell_type.NORMAL)

    params = sidb_simulation_parameters()
    params.mu_minus = -0.32
    params.base = 2

    assert params.mu_minus == pytest.approx(-0.32)

    cds = charge_distribution_surface_111(layout)

    result = exhaustive_ground_state_simulation(cds, params)

    assert result.algorithm_name == "ExGS"

    groundstate = result.groundstates()

    assert len(groundstate) == 1

    assert groundstate[0].get_charge_state((0, 0)) == sidb_charge_state.NEGATIVE
    assert groundstate[0].get_charge_state((1, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state((2, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state((3, 0)) == sidb_charge_state.NEGATIVE
