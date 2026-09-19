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

from mnt.pyfiction import (
    lattice,
    lattice_site,
    quicksim,
    quicksim_params,
    read_sqd_layout,
    sidb_charge_state,
    sidb_defect,
    sidb_defect_type,
    sidb_dot_tag,
    sidb_layout,
    sidb_simulation_parameters,
)

if TYPE_CHECKING:
    from pathlib import Path


def test_perturber_and_sidb_pair() -> None:
    """QuickSim finds the H-Si(100)-2x1 ground state."""

    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(4, 0, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(6, 0, 1), sidb_dot_tag.NORMAL)

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

    result = quicksim(layout, params)

    assert result is not None
    assert result.algorithm_name == "QuickSim"
    assert result.layout == layout
    assert len(result.charge_distributions) <= 80

    groundstate = result.groundstates()[0]

    assert groundstate.get_charge_state(lattice_site(0, 0, 1)) == sidb_charge_state.NEGATIVE
    assert groundstate.get_charge_state(lattice_site(4, 0, 1)) == sidb_charge_state.NEUTRAL
    assert groundstate.get_charge_state(lattice_site(6, 0, 1)) == sidb_charge_state.NEGATIVE


def test_perturber_and_sidb_pair_111() -> None:
    """QuickSim finds the H-Si(111)-1x1 ground state and honors its timeout."""

    layout = sidb_layout(lattice.si_111_1x1())
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(1, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(2, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(3, 0, 0), sidb_dot_tag.NORMAL)

    params = quicksim_params()
    params.simulation_parameters = sidb_simulation_parameters()
    params.simulation_parameters.mu_minus = -0.32
    params.iteration_steps = 80
    params.alpha = 0.7
    assert params.simulation_parameters.mu_minus == -0.32

    result = quicksim(layout, params)

    assert result is not None
    assert result.algorithm_name == "QuickSim"

    groundstate = result.groundstates()

    assert len(groundstate) == 1

    assert groundstate[0].get_charge_state(lattice_site(0, 0, 0)) == sidb_charge_state.NEGATIVE
    assert groundstate[0].get_charge_state(lattice_site(1, 0, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state(lattice_site(2, 0, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state(lattice_site(3, 0, 0)) == sidb_charge_state.NEGATIVE

    # test timeout
    params.timeout = 1
    params.iteration_steps = 10000
    params.number_threads = 1

    # should return None since no solution can be found in 1 millisecond.
    assert quicksim(layout, params) is None


def test_charged_defects_are_not_supported() -> None:
    """QuickSim rejects layouts with charged defects."""

    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(4, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_defect(lattice_site(2, 2, 0), sidb_defect(sidb_defect_type.SI_VACANCY, -1))

    assert quicksim(layout) is None


def test_quicksim_reports_progress_from_worker_threads(resources_dir: Path) -> None:
    """The iterations are reported by the worker threads, which needs the GIL released during the call."""
    layout = read_sqd_layout(str(resources_dir / "21_hex_inputsdbp_and_v19.sqd"))

    params = quicksim_params()
    params.iteration_steps = 50000  # long enough for the throttled reporter to forward worker reports
    params.number_threads = 2

    reports: list[tuple[str, int, int]] = []
    threads: set[int] = set()

    def on_progress(task: str, done: int, total: int) -> None:
        threads.add(threading.get_ident())
        reports.append((task, done, total))

    params.on_progress = on_progress

    result = quicksim(layout, params)

    assert result is not None
    iterations = [(done, total) for task, done, total in reports if task == "iterations"]
    assert iterations[0] == (0, 50000)
    assert iterations == sorted(iterations)
    assert iterations[-1] == (50000, 50000)
    # a worker thread reported in between, so the calling thread cannot have held the GIL
    assert len(threads) > 1
