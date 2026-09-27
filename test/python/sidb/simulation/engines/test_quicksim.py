# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``quicksim``."""

from __future__ import annotations

import threading
from typing import TYPE_CHECKING

from mnt.pyfiction.sidb import (
    ChargeState,
    Defect,
    DefectType,
    DotTag,
    Lattice,
    LatticeSite,
    SiDBLayout,
    SimulationParams,
)
from mnt.pyfiction.sidb.io import read_sqd_layout
from mnt.pyfiction.sidb.simulation import QuickSimParams, quicksim

if TYPE_CHECKING:
    from pathlib import Path


def test_perturber_and_sidb_pair() -> None:
    """QuickSim finds the H-Si(100)-2x1 ground state."""

    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(0, 0, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(4, 0, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(6, 0, 1), DotTag.NORMAL)

    params = QuickSimParams()
    params.simulation_parameters = SimulationParams()
    params.iteration_steps = 80
    params.alpha = 0.7
    assert params.iteration_steps == 80
    assert params.alpha == 0.7

    params_one = QuickSimParams()
    params_one.iteration_steps = 50
    params_one.alpha = 0.4
    params_one.number_threads = 1
    assert params_one.iteration_steps == 50
    assert params_one.alpha == 0.4
    assert params_one.number_threads == 1

    result = quicksim(layout, params=params)

    assert result is not None
    assert result.algorithm_name == "QuickSim"
    assert result.layout == layout
    assert len(result.charge_distributions) <= 80

    groundstate = result.ground_states()[0]

    assert groundstate.get_charge_state(LatticeSite(0, 0, 1)) == ChargeState.NEGATIVE
    assert groundstate.get_charge_state(LatticeSite(4, 0, 1)) == ChargeState.NEUTRAL
    assert groundstate.get_charge_state(LatticeSite(6, 0, 1)) == ChargeState.NEGATIVE


def test_perturber_and_sidb_pair_111() -> None:
    """QuickSim finds the H-Si(111)-1x1 ground state and honors its timeout."""

    layout = SiDBLayout(Lattice.si_111_1x1())
    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(1, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(2, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(3, 0, 0), DotTag.NORMAL)

    params = QuickSimParams()
    params.simulation_parameters = SimulationParams()
    params.simulation_parameters.mu_minus = -0.32
    params.iteration_steps = 80
    params.alpha = 0.7
    assert params.simulation_parameters.mu_minus == -0.32

    result = quicksim(layout, params=params)

    assert result is not None
    assert result.algorithm_name == "QuickSim"

    groundstate = result.ground_states()

    assert len(groundstate) == 1

    assert groundstate[0].get_charge_state(LatticeSite(0, 0, 0)) == ChargeState.NEGATIVE
    assert groundstate[0].get_charge_state(LatticeSite(1, 0, 0)) == ChargeState.NEUTRAL
    assert groundstate[0].get_charge_state(LatticeSite(2, 0, 0)) == ChargeState.NEUTRAL
    assert groundstate[0].get_charge_state(LatticeSite(3, 0, 0)) == ChargeState.NEGATIVE

    # test timeout
    params.timeout = 1
    params.iteration_steps = 10000
    params.number_threads = 1

    # should return None since no solution can be found in 1 millisecond.
    assert quicksim(layout, params=params) is None


def test_charged_defects_are_not_supported() -> None:
    """QuickSim rejects layouts with charged defects."""

    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(4, 0, 0), DotTag.NORMAL)
    layout.assign_defect(LatticeSite(2, 2, 0), Defect(DefectType.SI_VACANCY, -1))

    assert quicksim(layout) is None


def test_quicksim_reports_progress_from_worker_threads(resources_dir: Path) -> None:
    """The iterations are reported by the worker threads, which needs the GIL released during the call."""
    layout = read_sqd_layout(str(resources_dir / "21_hex_inputsdbp_and_v19.sqd"))

    params = QuickSimParams()
    params.iteration_steps = 50000  # long enough for the throttled reporter to forward worker reports
    params.number_threads = 2

    reports: list[tuple[str, int, int]] = []
    threads: set[int] = set()

    def on_progress(task: str, done: int, total: int) -> None:
        threads.add(threading.get_ident())
        reports.append((task, done, total))

    params.on_progress = on_progress
    workers = []
    params.on_worker_progress = lambda *report: workers.append(report)

    result = quicksim(layout, params=params)

    assert result is not None
    iterations = [(done, total) for task, done, total in reports if task == "iterations"]
    assert iterations[0] == (0, 50000)
    assert iterations == sorted(iterations)
    assert iterations[-1] == (50000, 50000)
    # a worker thread reported in between, so the calling thread cannot have held the GIL
    assert len(threads) > 1

    final = {worker: (done, total) for worker, count, description, done, total, active in workers if not active}
    assert set(final) == {0, 1}
    assert sum(done for done, total in final.values()) == params.iteration_steps
    assert all(done == total for done, total in final.values())
    assert all(count == 2 for worker, count, *rest in workers)
