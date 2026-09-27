# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""SiDB simulation engines, results, and their analysis."""

from __future__ import annotations

from mnt.pyfiction._native.sidb.simulation import (
    ChargeTransitionThresholdBounds,
    ExactSimulationEngine,
    HeuristicSimulationEngine,
    PotentialLandscape,
    SimulationEngine,
    SimulationResult,
    check_simulation_results_for_equivalence,
    is_ground_state,
    sidb_simulation_engine_name,
)
from mnt.pyfiction._native.sidb.simulation.engines import (
    AutomaticBaseNumberDetection,
    QuickExactParams,
    QuickSimParams,
    exhaustive_ground_state_simulation,
    quickexact,
    quicksim,
)

__all__ = [
    "AutomaticBaseNumberDetection",
    "ChargeTransitionThresholdBounds",
    "ExactSimulationEngine",
    "HeuristicSimulationEngine",
    "PotentialLandscape",
    "QuickExactParams",
    "QuickSimParams",
    "SimulationEngine",
    "SimulationResult",
    "check_simulation_results_for_equivalence",
    "exhaustive_ground_state_simulation",
    "is_ground_state",
    "quickexact",
    "quicksim",
    "sidb_simulation_engine_name",
]

try:
    from mnt.pyfiction._native.sidb.simulation.engines import (
        ClusterCompleteParams,
        GroundStateSpaceReporting,
        clustercomplete,
    )
except ImportError:
    pass
else:
    __all__ += [
        "ClusterCompleteParams",
        "GroundStateSpaceReporting",
        "clustercomplete",
    ]
