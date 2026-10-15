# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import math
from typing import Any

import pytest

from mnt.pyfiction.sidb import lattice, lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.simulation import exact_sidb_simulation_engine
from mnt.pyfiction.sidb.simulation.analysis import (
    time_to_solution,
    time_to_solution_for_given_simulation_results,
    time_to_solution_stats,
)
from mnt.pyfiction.sidb.simulation.engines import automatic_base_number_detection, quickexact, quicksim


def test_one_sidb_100_lattice() -> None:
    """Check time to solution on the Si(100) lattice."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)

    quicksim_parameter: dict[str, Any] = {"base": 3, "mu_minus": -0.3, "epsilon_r": 5.6, "lambda_tf": 5.0}

    stats = time_to_solution_stats()

    time_to_solution(layout, ps=stats, **quicksim_parameter, engine=exact_sidb_simulation_engine.QUICKEXACT)

    assert stats.acc == 100
    assert stats.time_to_solution > 0.0
    assert stats.mean_single_runtime > 0.0


def test_one_sidb_111_lattice() -> None:
    """Check time to solution on the Si(111) lattice."""
    layout = sidb_layout(lattice.si_111_1x1())
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)

    quicksim_parameter: dict[str, Any] = {"base": 3, "mu_minus": -0.3, "epsilon_r": 5.6, "lambda_tf": 5.0}

    stats = time_to_solution_stats()

    time_to_solution(layout, ps=stats, **quicksim_parameter, engine=exact_sidb_simulation_engine.QUICKEXACT)

    assert stats.acc == 100
    assert stats.time_to_solution > 0.0
    assert stats.mean_single_runtime > 0.0


def test_time_to_solution_with_simulation_results() -> None:
    """Check time to solution from exact and heuristic results."""
    layout = sidb_layout()

    # Assign SiDBs to the layout
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(1, 3, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(3, 3, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(5, 3, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(10, 3, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(15, 3, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(18, 3, 0), sidb_dot_tag.NORMAL)

    # Define simulation parameters
    options: dict[str, Any] = {"base": 2, "mu_minus": -0.32, "epsilon_r": 5.6, "lambda_tf": 5.0}
    quicksim_options = options.copy()

    number_of_repetitions = 100
    # Run the QuickSim simulations
    simulation_results_quicksim = []
    for _ in range(number_of_repetitions):
        result = quicksim(layout, **quicksim_options)
        assert result is not None
        simulation_results_quicksim.append(result)

    quickexact_options = options.copy()
    quickexact_options["base_number_detection"] = automatic_base_number_detection.OFF

    # Run the QuickExact simulation
    simulation_results_quickexact = quickexact(layout, **quickexact_options)

    # Calculate time-to-solution using the simulation results
    st = time_to_solution_stats()
    time_to_solution_for_given_simulation_results(
        simulation_results_quickexact,
        simulation_results_quicksim,
        confidence_level=0.997,
        ps=st,
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


def test_independent_quicksim_and_tts_timeouts() -> None:
    """Each heuristic timeout fails its attempt; the whole-search timeout raises."""
    layout = sidb_layout()
    for x, y in [(0, 0), (1, 3), (3, 3), (5, 3), (10, 3), (15, 3), (18, 3)]:
        layout.assign_sidb(lattice_site(x, y, 0), sidb_dot_tag.NORMAL)
    stats = time_to_solution_stats()
    time_to_solution(
        layout,
        base=2,
        quicksim_timeout=0,
        timeout=10000,
        number_threads=1,
        iteration_steps=10,
        repetitions=2,
        ps=stats,
    )
    assert stats.acc == 0
    assert math.isinf(stats.time_to_solution)
    with pytest.raises(TimeoutError):
        time_to_solution(layout, quicksim_timeout=10000, timeout=0, repetitions=2)


def test_tts_callback_routing() -> None:
    """TTS, heuristic, and worker callbacks report their respective tasks."""
    layout = sidb_layout()
    for x, y in [(0, 0), (1, 3), (3, 3), (5, 3), (10, 3), (15, 3), (18, 3)]:
        layout.assign_sidb(lattice_site(x, y, 0), sidb_dot_tag.NORMAL)
    repetitions: list[tuple[str, int, int]] = []
    iterations: list[tuple[str, int, int]] = []
    workers: list[tuple[int, int, str, int, int, bool]] = []
    time_to_solution(
        layout,
        base=2,
        number_threads=1,
        iteration_steps=2,
        repetitions=2,
        on_progress=lambda task, done, total: repetitions.append((task, done, total)),
        quicksim_on_progress=lambda task, done, total: iterations.append((task, done, total)),
        on_worker_progress=lambda worker, count, task, done, total, active: workers.append((
            worker,
            count,
            task,
            done,
            total,
            active,
        )),
    )
    assert repetitions
    assert all(task == "repetitions" for task, _, _ in repetitions)
    assert iterations
    assert all(task == "iterations" for task, _, _ in iterations)
    assert workers
    assert any(not active for _, _, _, _, _, active in workers)
