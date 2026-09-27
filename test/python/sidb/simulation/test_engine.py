# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction.sidb.simulation import (
    ExactSimulationEngine,
    HeuristicSimulationEngine,
    SimulationEngine,
    sidb_simulation_engine_name,
)


def test_sidb_simulation_engine_names():
    assert sidb_simulation_engine_name(SimulationEngine.QUICKEXACT) == "QuickExact"
    assert sidb_simulation_engine_name(SimulationEngine.QUICKSIM) == "QuickSim"
    assert sidb_simulation_engine_name(SimulationEngine.EXGS) == "ExGS"
    assert sidb_simulation_engine_name(SimulationEngine.CLUSTERCOMPLETE) == "ClusterComplete"

    assert sidb_simulation_engine_name(ExactSimulationEngine.QUICKEXACT) == "QuickExact"
    assert sidb_simulation_engine_name(ExactSimulationEngine.EXGS) == "ExGS"
    assert sidb_simulation_engine_name(ExactSimulationEngine.CLUSTERCOMPLETE) == "ClusterComplete"

    assert sidb_simulation_engine_name(HeuristicSimulationEngine.QUICKSIM) == "QuickSim"
