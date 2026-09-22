# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Shared execution budgets for simulation applications."""

from __future__ import annotations

import time
from typing import TYPE_CHECKING

import pytest

from mnt import pyfiction as pf

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path


@pytest.mark.parametrize(
    "parameter_type", [pf.is_operational_params, pf.physical_population_stability_params, pf.time_to_solution_params]
)
def test_timeout_parameter(
    parameter_type: type[
        pf.is_operational_params | pf.physical_population_stability_params | pf.time_to_solution_params
    ],
) -> None:
    """Budgets default to unlimited and reject values outside unsigned milliseconds."""
    params = parameter_type()
    assert params.timeout == 2**64 - 1
    for invalid in [-1, 2**64, 1.5]:
        with pytest.raises((TypeError, ValueError, OverflowError)):
            params.timeout = invalid


def test_zero_budgets(resources_dir: Path) -> None:
    """All existing simulation-application entry points expose the built-in TimeoutError."""
    layout = pf.read_sqd_layout(str(resources_dir / "21_hex_inputsdbp_and_v19.sqd"))
    spec = [pf.create_and_tt()]
    operational = pf.is_operational_params()
    operational.timeout = 0
    domain = pf.operational_domain_params()
    domain.operational_params = operational
    temperature = pf.critical_temperature_params()
    temperature.operational_params = operational
    displacement = pf.displacement_robustness_domain_params()
    displacement.operational_params = operational
    ratio = pf.operational_domain_ratio_params()
    ratio.op_domain_params = domain
    population = pf.physical_population_stability_params()
    population.timeout = 0
    tts = pf.time_to_solution_params()
    tts.timeout = 0

    calls: list[Callable[[], object]] = [
        lambda: pf.is_operational(layout, spec, operational),
        lambda: pf.operational_input_patterns(layout, spec, operational),
        lambda: pf.operational_domain_grid_search(layout, spec, domain),
        lambda: pf.operational_domain_random_sampling(layout, spec, 0, domain),
        lambda: pf.operational_domain_flood_fill(layout, spec, 0, domain),
        lambda: pf.operational_domain_contour_tracing(layout, spec, 0, domain),
        lambda: pf.critical_temperature_domain_grid_search(layout, spec, domain),
        lambda: pf.critical_temperature_domain_random_sampling(layout, spec, 0, domain),
        lambda: pf.critical_temperature_domain_flood_fill(layout, spec, 0, domain),
        lambda: pf.critical_temperature_domain_contour_tracing(layout, spec, 0, domain),
        lambda: pf.critical_temperature_gate_based(layout, spec, temperature),
        lambda: pf.critical_temperature_non_gate_based(layout, temperature),
        lambda: pf.determine_displacement_robustness_domain(layout, spec, displacement),
        lambda: pf.operational_domain_ratio(layout, spec, pf.parameter_point([5.6, 5.0]), ratio),
        lambda: pf.physically_valid_parameters(layout, pf.charge_distribution(layout), domain),
        lambda: pf.physical_population_stability(layout, population),
        lambda: pf.time_to_solution(layout, pf.quicksim_params(), tts),
    ]
    for call in calls:
        with pytest.raises(TimeoutError, match="deadline"):
            call()
    assert operational.timeout == 0


def test_domain_timeout_preserves_statistics(resources_dir: Path) -> None:
    """An expired outer budget survives nested simulator calls and does not publish partial statistics."""
    layout = pf.read_sqd_layout(str(resources_dir / "21_hex_inputsdbp_and_v19.sqd"))
    params = pf.operational_domain_params()
    params.operational_params.timeout = 10
    params.number_of_threads = 2
    stats = pf.operational_domain_stats()
    initial_count = stats.num_evaluated_parameter_combinations

    def report(_task: str, done: int, _total: int) -> None:
        """Let the budget expire during a non-interruptible callback."""
        if done == 0:
            time.sleep(0.03)

    params.on_progress = report
    with pytest.raises(TimeoutError):
        pf.operational_domain_grid_search(layout, [pf.create_and_tt()], params, stats)
    assert stats.num_evaluated_parameter_combinations == initial_count
