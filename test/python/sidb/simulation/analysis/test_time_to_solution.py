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

from mnt.pyfiction.sidb import DotTag, Lattice, LatticeSite, SiDBLayout, SimulationParams
from mnt.pyfiction.sidb.analysis import (
    TimeToSolutionParams,
    time_to_solution,
    time_to_solution_for_given_simulation_results,
)
from mnt.pyfiction.sidb.simulation import (
    AutomaticBaseNumberDetection,
    ExactSimulationEngine,
    QuickExactParams,
    QuickSimParams,
    quickexact,
    quicksim,
)


def test_one_sidb_100_lattice() -> None:
    """Check time to solution on the Si(100) lattice."""
    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)

    quicksim_parameter = QuickSimParams()
    quicksim_parameter.simulation_parameters = SimulationParams(3, -0.3)

    tts_params = TimeToSolutionParams()
    tts_params.engine = ExactSimulationEngine.QUICKEXACT

    stats = time_to_solution(layout, quicksim_params=quicksim_parameter, params=tts_params)

    assert stats.acc == 100
    assert stats.time_to_solution > 0.0
    assert stats.mean_single_runtime > 0.0


def test_one_sidb_111_lattice() -> None:
    """Check time to solution on the Si(111) lattice."""
    layout = SiDBLayout(Lattice.si_111_1x1())
    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)

    quicksim_parameter = QuickSimParams()
    quicksim_parameter.simulation_parameters = SimulationParams(3, -0.3)

    tts_params = TimeToSolutionParams()
    tts_params.engine = ExactSimulationEngine.QUICKEXACT

    stats = time_to_solution(layout, quicksim_params=quicksim_parameter, params=tts_params)

    assert stats.acc == 100
    assert stats.time_to_solution > 0.0
    assert stats.mean_single_runtime > 0.0


def test_time_to_solution_with_simulation_results() -> None:
    """Check time to solution from exact and heuristic results."""
    layout = SiDBLayout()

    # Assign SiDBs to the layout
    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(1, 3, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(3, 3, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(5, 3, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(10, 3, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(15, 3, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(18, 3, 0), DotTag.NORMAL)

    # Define simulation parameters
    params = SimulationParams(2, -0.32)
    quicksim_params_inst = QuickSimParams()
    quicksim_params_inst.simulation_parameters = params

    number_of_repetitions = 100
    # Run the QuickSim simulations
    simulation_results_quicksim = []
    for _ in range(number_of_repetitions):
        result = quicksim(layout, params=quicksim_params_inst)
        assert result is not None
        simulation_results_quicksim.append(result)

    quickexact_params_inst = QuickExactParams()
    quickexact_params_inst.simulation_parameters = params
    quickexact_params_inst.base_number_detection = AutomaticBaseNumberDetection.OFF
    assert quickexact_params_inst.simulation_parameters.mu_minus == -0.32
    assert quickexact_params_inst.base_number_detection == AutomaticBaseNumberDetection.OFF

    # Run the QuickExact simulation
    simulation_results_quickexact = quickexact(layout, params=quickexact_params_inst)

    # Calculate time-to-solution using the simulation results
    st = time_to_solution_for_given_simulation_results(
        simulation_results_quickexact, simulation_results_quicksim, confidence_level=0.997
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
