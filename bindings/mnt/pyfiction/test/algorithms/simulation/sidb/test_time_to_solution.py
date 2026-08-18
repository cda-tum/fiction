import math

import pytest

from mnt.pyfiction import (
    automatic_base_number_detection,
    charge_distribution_surface_100,
    charge_distribution_surface_111,
    exact_sidb_simulation_engine,
    quickexact,
    quickexact_params,
    quicksim,
    quicksim_params,
    sidb_100_lattice,
    sidb_111_lattice,
    sidb_simulation_parameters,
    sidb_technology,
    time_to_solution,
    time_to_solution_for_given_simulation_results,
    time_to_solution_params,
    time_to_solution_stats,
)


def test_one_sidb_100_lattice():
    layout = sidb_100_lattice((0, 0))
    layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)

    quicksim_parameter = quicksim_params()
    quicksim_parameter.simulation_parameters = sidb_simulation_parameters(3, -0.3)

    tts_params = time_to_solution_params()
    tts_params.engine = exact_sidb_simulation_engine.QUICKEXACT
    stats = time_to_solution_stats()

    cds = charge_distribution_surface_100(layout)

    time_to_solution(cds, quicksim_parameter, tts_params, stats)

    assert stats.acc == 100
    assert stats.time_to_solution > 0.0
    assert stats.mean_single_runtime > 0.0


def test_one_sidb_111_lattice():
    layout = sidb_111_lattice((0, 0))
    layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)

    quicksim_parameter = quicksim_params()
    quicksim_parameter.simulation_parameters = sidb_simulation_parameters(3, -0.3)

    tts_params = time_to_solution_params()
    tts_params.engine = exact_sidb_simulation_engine.QUICKEXACT
    stats = time_to_solution_stats()

    cds = charge_distribution_surface_111(layout)

    time_to_solution(cds, quicksim_parameter, tts_params, stats)

    assert stats.acc == 100
    assert stats.time_to_solution > 0.0
    assert stats.mean_single_runtime > 0.0


def test_time_to_solution_with_simulation_results():
    layout = sidb_100_lattice((0, 0))

    # Assign SiDBs to the layout
    layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((1, 6, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((3, 6, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((5, 6, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((10, 6, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((15, 6, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((18, 6, 0), sidb_technology.cell_type.NORMAL)

    # Define simulation parameters
    params = sidb_simulation_parameters(2, -0.32)
    quicksim_params_inst = quicksim_params()
    quicksim_params_inst.simulation_parameters = params

    number_of_repetitions = 100
    # Run the QuickSim simulations
    simulation_results_quicksim = [quicksim(layout, quicksim_params_inst) for _ in range(number_of_repetitions)]

    quickexact_params_inst = quickexact_params()
    quickexact_params_inst.simulation_parameters = params
    quickexact_params_inst.base_number_detection = automatic_base_number_detection.OFF
    assert quickexact_params_inst.simulation_parameters.mu_minus == -0.32
    assert quickexact_params_inst.base_number_detection == automatic_base_number_detection.OFF

    # Run the QuickExact simulation
    simulation_results_quickexact = quickexact(layout, quickexact_params_inst)

    # Calculate time-to-solution using the simulation results
    st = time_to_solution_stats()
    time_to_solution_for_given_simulation_results(
        simulation_results_quickexact,
        simulation_results_quicksim,
        0.997,
        st,
    )

    assert st.time_to_solution > 0.0
    assert st.mean_single_runtime > 0.0

    if st.acc == 100:
        tts_calculated = st.mean_single_runtime
        assert st.time_to_solution == pytest.approx(tts_calculated, abs=1e-6)
    elif not math.isclose(st.acc, 0.0):
        # To avoid division by zero, ensure st.acc is not 1.0
        tts_calculated = st.mean_single_runtime * math.log(1.0 - 0.997) / math.log(1.0 - st.acc / 100)
        assert st.time_to_solution == pytest.approx(tts_calculated, abs=1e-6)
