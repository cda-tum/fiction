# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import math

import pytest

from mnt.pyfiction import (
    automatic_base_number_detection,
    exact_sidb_simulation_engine,
    lattice,
    lattice_site,
    quickexact,
    quickexact_params,
    quicksim,
    quicksim_params,
    sidb_layout,
    sidb_simulation_parameters,
    sidb_technology,
    time_to_solution,
    time_to_solution_for_given_simulation_results,
    time_to_solution_params,
    time_to_solution_stats,
)


def test_one_sidb_100_lattice() -> None:
    """Check time to solution on the Si(100) lattice."""
    layout = sidb_layout()
    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)

    quicksim_parameter = quicksim_params()
    quicksim_parameter.simulation_parameters = sidb_simulation_parameters(3, -0.3)

    tts_params = time_to_solution_params()
    tts_params.engine = exact_sidb_simulation_engine.QUICKEXACT
    stats = time_to_solution_stats()

    time_to_solution(layout, quicksim_parameter, tts_params, stats)

    assert stats.acc == 100
    assert stats.time_to_solution > 0.0
    assert stats.mean_single_runtime > 0.0


def test_one_sidb_111_lattice() -> None:
    """Check time to solution on the Si(111) lattice."""
    layout = sidb_layout(lattice.si_111_1x1())
    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)

    quicksim_parameter = quicksim_params()
    quicksim_parameter.simulation_parameters = sidb_simulation_parameters(3, -0.3)

    tts_params = time_to_solution_params()
    tts_params.engine = exact_sidb_simulation_engine.QUICKEXACT
    stats = time_to_solution_stats()

    time_to_solution(layout, quicksim_parameter, tts_params, stats)

    assert stats.acc == 100
    assert stats.time_to_solution > 0.0
    assert stats.mean_single_runtime > 0.0


def test_time_to_solution_with_simulation_results() -> None:
    """Check time to solution from exact and heuristic results."""
    layout = sidb_layout()

    # Assign SiDBs to the layout
    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(1, 3, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(3, 3, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(5, 3, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(10, 3, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(15, 3, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(18, 3, 0), sidb_technology.cell_type.NORMAL)

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
