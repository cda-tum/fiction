from __future__ import annotations

from mnt.pyfiction import (
    charge_distribution_surface,
    charge_distribution_surface_111,
    quicksim,
    quicksim_params,
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

    params = quicksim_params()
    params.simulation_parameters = sidb_simulation_parameters()
    params.iteration_steps = 80
    params.alpha = 0.7
    assert params.iteration_steps == 80
    assert params.alpha == 0.7

    params_one = quicksim_params()
    params_one.iteration_steps = 50
    params_one.alpha = 0.4
    params_one.number_threads = 1
    assert params_one.iteration_steps == 50
    assert params_one.alpha == 0.4
    assert params_one.number_threads == 1

    charge_distribution_surface(layout)

    result = quicksim(layout, params)

    assert result.algorithm_name == "QuickSim"
    assert len(result.charge_distributions) <= 80

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

    params = quicksim_params()
    params.simulation_parameters = sidb_simulation_parameters()
    params.simulation_parameters.mu_minus = -0.32
    params.iteration_steps = 80
    params.alpha = 0.7
    assert params.iteration_steps == 80
    assert params.alpha == 0.7
    assert params.simulation_parameters.mu_minus == -0.32

    charge_distribution_surface_111(layout)

    result = quicksim(layout, params)

    assert result.algorithm_name == "QuickSim"

    groundstate = result.groundstates()

    assert len(groundstate) == 1

    assert groundstate[0].get_charge_state((0, 0)) == sidb_charge_state.NEGATIVE
    assert groundstate[0].get_charge_state((1, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state((2, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state((3, 0)) == sidb_charge_state.NEGATIVE

    # test timeout
    params.timeout = 1
    params.iteration_steps = 10000
    params.number_threads = 1

    # should return None since no solution can be found in 1 millisecond.
    assert quicksim(layout, params) is None
