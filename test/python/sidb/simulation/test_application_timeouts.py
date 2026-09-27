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

from mnt.pyfiction.sidb import ChargeState
from mnt.pyfiction.sidb.analysis import (
    CriticalTemperatureParams,
    DisplacementRobustnessDomainParams,
    OperationalDomainParams,
    OperationalDomainRatioParams,
    OperationalParams,
    ParameterPoint,
    PhysicalPopulationStabilityParams,
    TimeToSolutionParams,
    critical_temperature_domain_contour_tracing,
    critical_temperature_domain_flood_fill,
    critical_temperature_domain_grid_search,
    critical_temperature_domain_random_sampling,
    critical_temperature_gate_based,
    critical_temperature_non_gate_based,
    determine_displacement_robustness_domain,
    is_operational,
    operational_domain_contour_tracing,
    operational_domain_flood_fill,
    operational_domain_grid_search,
    operational_domain_random_sampling,
    operational_domain_ratio,
    operational_input_patterns,
    physical_population_stability,
    physically_valid_parameters,
    time_to_solution,
)
from mnt.pyfiction.sidb.io import read_sqd_layout
from mnt.pyfiction.sidb.simulation import PotentialLandscape, QuickSimParams
from mnt.pyfiction.synthesis import (
    standard_functions,
)

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path


@pytest.mark.parametrize("parameter_type", [OperationalParams, PhysicalPopulationStabilityParams, TimeToSolutionParams])
def test_timeout_parameter(
    parameter_type: type[OperationalParams | PhysicalPopulationStabilityParams | TimeToSolutionParams],
) -> None:
    """Budgets default to unlimited and reject values outside unsigned milliseconds."""
    params = parameter_type()
    assert params.timeout == 2**64 - 1
    for invalid in [-1, 2**64, 1.5]:
        with pytest.raises((TypeError, ValueError, OverflowError)):
            params.timeout = invalid  # ty: ignore[invalid-assignment]  # deliberately invalid


def test_zero_budgets(resources_dir: Path) -> None:
    """All existing simulation-application entry points expose the built-in TimeoutError."""
    layout = read_sqd_layout(str(resources_dir / "21_hex_inputsdbp_and_v19.sqd"))
    spec = [standard_functions("and")[0]]
    operational = OperationalParams()
    operational.timeout = 0
    domain = OperationalDomainParams()
    domain.operational_params = operational
    temperature = CriticalTemperatureParams()
    temperature.operational_params = operational
    displacement = DisplacementRobustnessDomainParams()
    displacement.operational_params = operational
    ratio = OperationalDomainRatioParams()
    ratio.op_domain_params = domain
    population = PhysicalPopulationStabilityParams()
    population.timeout = 0
    tts = TimeToSolutionParams()
    tts.timeout = 0

    calls: list[Callable[[], object]] = [
        lambda: is_operational(layout, spec, params=operational),
        lambda: operational_input_patterns(layout, spec, operational),
        lambda: operational_domain_grid_search(layout, spec, params=domain).domain,
        lambda: operational_domain_random_sampling(layout, spec, 0, params=domain).domain,
        lambda: operational_domain_flood_fill(layout, spec, 0, params=domain).domain,
        lambda: operational_domain_contour_tracing(layout, spec, 0, params=domain).domain,
        lambda: critical_temperature_domain_grid_search(layout, spec, params=domain).domain,
        lambda: critical_temperature_domain_random_sampling(layout, spec, 0, params=domain).domain,
        lambda: critical_temperature_domain_flood_fill(layout, spec, 0, params=domain).domain,
        lambda: critical_temperature_domain_contour_tracing(layout, spec, 0, params=domain).domain,
        lambda: critical_temperature_gate_based(layout, spec, params=temperature).temperature,
        lambda: critical_temperature_non_gate_based(layout, params=temperature).temperature,
        lambda: determine_displacement_robustness_domain(layout, spec, params=displacement).domain,
        lambda: operational_domain_ratio(layout, spec, ParameterPoint([5.6, 5.0]), ratio),
        lambda: physically_valid_parameters(
            layout, PotentialLandscape(layout).evaluate([ChargeState.NEGATIVE] * layout.num_dots()), domain
        ),
        lambda: physical_population_stability(layout, population),
        lambda: time_to_solution(layout, quicksim_params=QuickSimParams(), params=tts),
    ]
    for call in calls:
        with pytest.raises(TimeoutError, match="deadline"):
            call()
    assert operational.timeout == 0


def test_domain_timeout_preserves_statistics(resources_dir: Path) -> None:
    """An expired outer budget survives nested simulator calls and does not publish partial statistics."""
    layout = read_sqd_layout(str(resources_dir / "21_hex_inputsdbp_and_v19.sqd"))
    params = OperationalDomainParams()
    params.operational_params.timeout = 10
    params.number_of_threads = 2
    result = None

    def report(_task: str, done: int, _total: int) -> None:
        """Let the budget expire during a non-interruptible callback."""
        if done == 0:
            time.sleep(0.03)

    params.on_progress = report
    with pytest.raises(TimeoutError):
        result = operational_domain_grid_search(layout, [standard_functions("and")[0]], params=params)
    assert result is None
