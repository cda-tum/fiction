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

from mnt.pyfiction.sidb import charge_distribution
from mnt.pyfiction.sidb.io import read_sqd_layout
from mnt.pyfiction.sidb.simulation.analysis import (
    critical_temperature_gate_based,
    critical_temperature_non_gate_based,
    critical_temperature_params,
    physical_population_stability,
    physical_population_stability_params,
    physically_valid_parameters,
    time_to_solution,
    time_to_solution_params,
)
from mnt.pyfiction.sidb.simulation.defects import (
    determine_displacement_robustness_domain,
    displacement_robustness_domain_params,
)
from mnt.pyfiction.sidb.simulation.engines import quicksim_params
from mnt.pyfiction.sidb.simulation.logic import (
    critical_temperature_domain_contour_tracing,
    critical_temperature_domain_flood_fill,
    critical_temperature_domain_grid_search,
    critical_temperature_domain_random_sampling,
    is_operational,
    is_operational_params,
    operational_domain_contour_tracing,
    operational_domain_flood_fill,
    operational_domain_grid_search,
    operational_domain_params,
    operational_domain_random_sampling,
    operational_domain_ratio,
    operational_domain_ratio_params,
    operational_domain_stats,
    operational_input_patterns,
    parameter_point,
)
from mnt.pyfiction.synthesis import create_and_tt

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path


@pytest.mark.parametrize(
    "parameter_type", [is_operational_params, physical_population_stability_params, time_to_solution_params]
)
def test_timeout_parameter(
    parameter_type: type[is_operational_params | physical_population_stability_params | time_to_solution_params],
) -> None:
    """Budgets default to unlimited and reject values outside unsigned milliseconds."""
    params = parameter_type()
    assert params.timeout == 2**64 - 1
    for invalid in [-1, 2**64, 1.5]:
        with pytest.raises((TypeError, ValueError, OverflowError)):
            params.timeout = invalid


def test_zero_budgets(resources_dir: Path) -> None:
    """All existing simulation-application entry points expose the built-in TimeoutError."""
    layout = read_sqd_layout(str(resources_dir / "21_hex_inputsdbp_and_v19.sqd"))
    spec = [create_and_tt()]
    operational = is_operational_params()
    operational.timeout = 0
    domain = operational_domain_params()
    domain.operational_params = operational
    temperature = critical_temperature_params()
    temperature.operational_params = operational
    displacement = displacement_robustness_domain_params()
    displacement.operational_params = operational
    ratio = operational_domain_ratio_params()
    ratio.op_domain_params = domain
    population = physical_population_stability_params()
    population.timeout = 0
    tts = time_to_solution_params()
    tts.timeout = 0

    calls: list[Callable[[], object]] = [
        lambda: is_operational(layout, spec, operational),
        lambda: operational_input_patterns(layout, spec, operational),
        lambda: operational_domain_grid_search(layout, spec, domain),
        lambda: operational_domain_random_sampling(layout, spec, 0, domain),
        lambda: operational_domain_flood_fill(layout, spec, 0, domain),
        lambda: operational_domain_contour_tracing(layout, spec, 0, domain),
        lambda: critical_temperature_domain_grid_search(layout, spec, domain),
        lambda: critical_temperature_domain_random_sampling(layout, spec, 0, domain),
        lambda: critical_temperature_domain_flood_fill(layout, spec, 0, domain),
        lambda: critical_temperature_domain_contour_tracing(layout, spec, 0, domain),
        lambda: critical_temperature_gate_based(layout, spec, temperature),
        lambda: critical_temperature_non_gate_based(layout, temperature),
        lambda: determine_displacement_robustness_domain(layout, spec, displacement),
        lambda: operational_domain_ratio(layout, spec, parameter_point([5.6, 5.0]), ratio),
        lambda: physically_valid_parameters(layout, charge_distribution(layout), domain),
        lambda: physical_population_stability(layout, population),
        lambda: time_to_solution(layout, quicksim_params(), tts),
    ]
    for call in calls:
        with pytest.raises(TimeoutError, match="deadline"):
            call()
    assert operational.timeout == 0


def test_domain_timeout_preserves_statistics(resources_dir: Path) -> None:
    """An expired outer budget survives nested simulator calls and does not publish partial statistics."""
    layout = read_sqd_layout(str(resources_dir / "21_hex_inputsdbp_and_v19.sqd"))
    params = operational_domain_params()
    params.operational_params.timeout = 10
    params.number_of_threads = 2
    stats = operational_domain_stats()
    initial_count = stats.num_evaluated_parameter_combinations

    def report(_task: str, done: int, _total: int) -> None:
        """Let the budget expire during a non-interruptible callback."""
        if done == 0:
            time.sleep(0.03)

    params.on_progress = report
    with pytest.raises(TimeoutError):
        operational_domain_grid_search(layout, [create_and_tt()], params, stats)
    assert stats.num_evaluated_parameter_combinations == initial_count
