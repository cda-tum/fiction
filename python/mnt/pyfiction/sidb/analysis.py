# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Operational, thermal, and defect analyses of SiDB layouts."""

from __future__ import annotations

from dataclasses import dataclass
from typing import TYPE_CHECKING, NamedTuple

from mnt.pyfiction._native.sidb.simulation import analysis as _analysis
from mnt.pyfiction._native.sidb.simulation import defects as _defects
from mnt.pyfiction._native.sidb.simulation import logic as _logic
from mnt.pyfiction._native.sidb.simulation.analysis import (
    CriticalTemperatureParams,
    CriticalTemperatureStats,
    EnergyDistribution,
    EnergyState,
    PhysicallyValidParametersDomain,
    PhysicalPopulationStabilityParams,
    PopulationStabilityInformation,
    StateType,
    TimeToSolutionParams,
    TimeToSolutionStats,
    TransitionType,
    calculate_energy_distribution,
    can_positive_charges_occur,
    minimum_energy,
    occupation_probability_gate_based,
    occupation_probability_non_gate_based,
    physical_population_stability,
    physically_valid_parameters,
)
from mnt.pyfiction._native.sidb.simulation.defects import (
    DimerDisplacementPolicy,
    DisplacementAnalysisMode,
    DisplacementRobustnessDomain,
    DisplacementRobustnessDomainParams,
    DisplacementRobustnessDomainStats,
)
from mnt.pyfiction._native.sidb.simulation.logic import (
    BdlPair,
    BdlPairDetectionParams,
    BdlWire,
    BdlWireDetectionParams,
    BdlWireSelection,
    CriticalTemperatureDomain,
    InputEncoding,
    InputPatternParams,
    OperationalAnalysisStrategy,
    OperationalCondition,
    OperationalDomain,
    OperationalDomainParams,
    OperationalDomainRatioParams,
    OperationalDomainStats,
    OperationalParams,
    OperationalStatus,
    ParameterPoint,
    SweepParameter,
    SweepRange,
    _InputPatterns,
    detect_bdl_pairs,
    detect_bdl_wires,
    is_kink_induced_non_operational,
    kink_induced_non_operational_input_patterns,
    operational_domain_ratio,
    operational_input_patterns,
)
from mnt.pyfiction.sidb import SiDBLayout

if TYPE_CHECKING:
    from collections.abc import Iterator, Sequence

    from mnt.pyfiction.sidb.simulation import QuickSimParams, SimulationResult
    from mnt.pyfiction.synthesis import TruthTable

__all__ = [
    "BdlPair",
    "BdlPairDetectionParams",
    "BdlWire",
    "BdlWireDetectionParams",
    "BdlWireSelection",
    "CriticalTemperatureDomain",
    "CriticalTemperatureDomainResult",
    "CriticalTemperatureParams",
    "CriticalTemperatureResult",
    "CriticalTemperatureStats",
    "DimerDisplacementPolicy",
    "DisplacementAnalysisMode",
    "DisplacementRobustnessDomain",
    "DisplacementRobustnessDomainParams",
    "DisplacementRobustnessDomainStats",
    "DisplacementRobustnessResult",
    "EnergyDistribution",
    "EnergyState",
    "InputEncoding",
    "InputPatternParams",
    "OperationalAnalysisStrategy",
    "OperationalCondition",
    "OperationalDomain",
    "OperationalDomainParams",
    "OperationalDomainRatioParams",
    "OperationalDomainResult",
    "OperationalDomainStats",
    "OperationalParams",
    "OperationalResult",
    "OperationalStatus",
    "ParameterPoint",
    "PhysicalPopulationStabilityParams",
    "PhysicallyValidParametersDomain",
    "PopulationStabilityInformation",
    "StateType",
    "SweepParameter",
    "SweepRange",
    "TimeToSolutionParams",
    "TimeToSolutionStats",
    "TransitionType",
    "calculate_energy_distribution",
    "can_positive_charges_occur",
    "critical_temperature_domain_contour_tracing",
    "critical_temperature_domain_flood_fill",
    "critical_temperature_domain_grid_search",
    "critical_temperature_domain_random_sampling",
    "critical_temperature_gate_based",
    "critical_temperature_non_gate_based",
    "detect_bdl_pairs",
    "detect_bdl_wires",
    "determine_displacement_robustness_domain",
    "input_patterns",
    "is_kink_induced_non_operational",
    "is_operational",
    "kink_induced_non_operational_input_patterns",
    "minimum_energy",
    "occupation_probability_gate_based",
    "occupation_probability_non_gate_based",
    "operational_domain_contour_tracing",
    "operational_domain_flood_fill",
    "operational_domain_grid_search",
    "operational_domain_random_sampling",
    "operational_domain_ratio",
    "operational_input_patterns",
    "physical_population_stability",
    "physically_valid_parameters",
    "time_to_solution",
    "time_to_solution_for_given_simulation_results",
]


def input_patterns(
    layout: SiDBLayout, *, params: InputPatternParams | None = None, input_wires: Sequence[BdlWire] | None = None
) -> Iterator[SiDBLayout]:
    """Iterate independent layouts in input-pattern order, starting with pattern zero.

    Args:
        layout: SiDB layout whose input BDL pairs encode the pattern bits.
        params: Pair detection and input encoding options.
        input_wires: Known input wires. Omit to detect them.

    Returns:
        An iterator of layout snapshots. A layout with no inputs yields one snapshot.

    Raises:
        ValueError: More than 63 input pairs or an input pair without a complete wire.
    """
    options = params if params is not None else InputPatternParams()
    patterns = _InputPatterns(layout, options) if input_wires is None else _InputPatterns(layout, options, input_wires)
    if not patterns.is_valid():
        msg = "Each input BDL pair requires a complete input wire"
        raise ValueError(msg)
    return patterns


@dataclass(frozen=True)
class CriticalTemperatureResult:
    """Critical temperature in kelvin and simulation statistics."""

    temperature: float
    stats: CriticalTemperatureStats


@dataclass(frozen=True)
class OperationalDomainResult:
    """Operational parameter points and search statistics."""

    domain: OperationalDomain
    stats: OperationalDomainStats


@dataclass(frozen=True)
class CriticalTemperatureDomainResult:
    """Critical temperatures over parameter space and search statistics."""

    domain: CriticalTemperatureDomain
    stats: OperationalDomainStats


@dataclass(frozen=True)
class DisplacementRobustnessResult:
    """Displacement robustness domain and search statistics."""

    domain: DisplacementRobustnessDomain
    stats: DisplacementRobustnessDomainStats


def operational_domain_grid_search(
    layout: SiDBLayout, spec: Sequence[TruthTable], *, params: OperationalDomainParams | None = None
) -> OperationalDomainResult:
    """Compute the operational domain with grid search.

    Args:
        layout: SiDB gate layout.
        spec: Truth tables in output order.
        params: Search options and progress callbacks.

    Returns:
        The sampled domain and search statistics.
    """
    options = params if params is not None else OperationalDomainParams()
    stats = OperationalDomainStats()
    domain = _logic.operational_domain_grid_search(layout, spec, options, stats)
    return OperationalDomainResult(domain, stats)


def operational_domain_random_sampling(
    layout: SiDBLayout, spec: Sequence[TruthTable], samples: int, *, params: OperationalDomainParams | None = None
) -> OperationalDomainResult:
    """Compute the operational domain with random sampling.

    Args:
        layout: SiDB gate layout.
        spec: Truth tables in output order.
        samples: Number of initial random samples.
        params: Search options and progress callbacks.

    Returns:
        The sampled domain and search statistics.
    """
    options = params if params is not None else OperationalDomainParams()
    stats = OperationalDomainStats()
    domain = _logic.operational_domain_random_sampling(layout, spec, samples, options, stats)
    return OperationalDomainResult(domain, stats)


def operational_domain_flood_fill(
    layout: SiDBLayout, spec: Sequence[TruthTable], samples: int, *, params: OperationalDomainParams | None = None
) -> OperationalDomainResult:
    """Compute the operational domain with flood fill.

    Args:
        layout: SiDB gate layout.
        spec: Truth tables in output order.
        samples: Number of initial random samples.
        params: Search options and progress callbacks.

    Returns:
        The sampled domain and search statistics.
    """
    options = params if params is not None else OperationalDomainParams()
    stats = OperationalDomainStats()
    domain = _logic.operational_domain_flood_fill(layout, spec, samples, options, stats)
    return OperationalDomainResult(domain, stats)


def operational_domain_contour_tracing(
    layout: SiDBLayout, spec: Sequence[TruthTable], samples: int, *, params: OperationalDomainParams | None = None
) -> OperationalDomainResult:
    """Compute the operational domain with contour tracing.

    Args:
        layout: SiDB gate layout.
        spec: Truth tables in output order.
        samples: Number of initial random samples.
        params: Search options and progress callbacks.

    Returns:
        The sampled domain and search statistics.
    """
    options = params if params is not None else OperationalDomainParams()
    stats = OperationalDomainStats()
    domain = _logic.operational_domain_contour_tracing(layout, spec, samples, options, stats)
    return OperationalDomainResult(domain, stats)


def critical_temperature_domain_grid_search(
    layout: SiDBLayout, spec: Sequence[TruthTable], *, params: OperationalDomainParams | None = None
) -> CriticalTemperatureDomainResult:
    """Compute the critical temperature domain with grid search.

    Args:
        layout: SiDB gate layout.
        spec: Truth tables in output order.
        params: Search options and progress callbacks.

    Returns:
        The sampled domain and search statistics.
    """
    options = params if params is not None else OperationalDomainParams()
    stats = OperationalDomainStats()
    domain = _logic.critical_temperature_domain_grid_search(layout, spec, options, stats)
    return CriticalTemperatureDomainResult(domain, stats)


def critical_temperature_domain_random_sampling(
    layout: SiDBLayout, spec: Sequence[TruthTable], samples: int, *, params: OperationalDomainParams | None = None
) -> CriticalTemperatureDomainResult:
    """Compute the critical temperature domain with random sampling.

    Args:
        layout: SiDB gate layout.
        spec: Truth tables in output order.
        samples: Number of initial random samples.
        params: Search options and progress callbacks.

    Returns:
        The sampled domain and search statistics.
    """
    options = params if params is not None else OperationalDomainParams()
    stats = OperationalDomainStats()
    domain = _logic.critical_temperature_domain_random_sampling(layout, spec, samples, options, stats)
    return CriticalTemperatureDomainResult(domain, stats)


def critical_temperature_domain_flood_fill(
    layout: SiDBLayout, spec: Sequence[TruthTable], samples: int, *, params: OperationalDomainParams | None = None
) -> CriticalTemperatureDomainResult:
    """Compute the critical temperature domain with flood fill.

    Args:
        layout: SiDB gate layout.
        spec: Truth tables in output order.
        samples: Number of initial random samples.
        params: Search options and progress callbacks.

    Returns:
        The sampled domain and search statistics.
    """
    options = params if params is not None else OperationalDomainParams()
    stats = OperationalDomainStats()
    domain = _logic.critical_temperature_domain_flood_fill(layout, spec, samples, options, stats)
    return CriticalTemperatureDomainResult(domain, stats)


def critical_temperature_domain_contour_tracing(
    layout: SiDBLayout, spec: Sequence[TruthTable], samples: int, *, params: OperationalDomainParams | None = None
) -> CriticalTemperatureDomainResult:
    """Compute the critical temperature domain with contour tracing.

    Args:
        layout: SiDB gate layout.
        spec: Truth tables in output order.
        samples: Number of initial random samples.
        params: Search options and progress callbacks.

    Returns:
        The sampled domain and search statistics.
    """
    options = params if params is not None else OperationalDomainParams()
    stats = OperationalDomainStats()
    domain = _logic.critical_temperature_domain_contour_tracing(layout, spec, samples, options, stats)
    return CriticalTemperatureDomainResult(domain, stats)


def critical_temperature_non_gate_based(
    layout: SiDBLayout, *, params: CriticalTemperatureParams | None = None
) -> CriticalTemperatureResult:
    """Find the temperature at which excited states exceed the configured confidence bound.

    Args:
        layout: SiDB layout.
        params: Physical settings, confidence bound, temperature ceiling, and callbacks.

    Returns:
        Temperature in kelvin and simulation statistics.
    """
    options = params if params is not None else CriticalTemperatureParams()
    stats = CriticalTemperatureStats()
    temperature = _analysis.critical_temperature_non_gate_based(layout, options, stats)
    return CriticalTemperatureResult(temperature, stats)


def critical_temperature_gate_based(
    layout: SiDBLayout | Sequence[SiDBLayout],
    spec: Sequence[TruthTable],
    *,
    params: CriticalTemperatureParams | None = None,
    output_bdl_pairs: Sequence[BdlPair] | None = None,
    input_bdl_wires: Sequence[BdlWire] | None = None,
    output_bdl_wires: Sequence[BdlWire] | None = None,
) -> CriticalTemperatureResult:
    """Find the temperature at which logic errors exceed the configured confidence bound.

    Args:
        layout: A gate layout, or independent layouts in input-pattern order.
        spec: Truth tables in output order.
        params: Physical settings, confidence bound, temperature ceiling, and callbacks.
        output_bdl_pairs: Output pairs for explicit input-pattern layouts.
        input_bdl_wires: Input wires for explicit input-pattern layouts.
        output_bdl_wires: Output wires for explicit input-pattern layouts.

    Returns:
        Temperature in kelvin and simulation statistics.

    Raises:
        ValueError: Explicit input patterns and BDL wiring are not supplied together.
    """
    options = params if params is not None else CriticalTemperatureParams()
    stats = CriticalTemperatureStats()
    if isinstance(layout, SiDBLayout):
        if any(value is not None for value in (output_bdl_pairs, input_bdl_wires, output_bdl_wires)):
            msg = "Explicit wires and pairs require one layout per input pattern"
            raise ValueError(msg)
        temperature = _analysis.critical_temperature_gate_based(layout, spec, options, stats)
    else:
        if output_bdl_pairs is None or input_bdl_wires is None or output_bdl_wires is None:
            msg = "Input-pattern layouts require output pairs and input and output wires"
            raise ValueError(msg)
        temperature = _analysis.critical_temperature_gate_based(
            layout, spec, options, output_bdl_pairs, input_bdl_wires, output_bdl_wires, stats
        )
    return CriticalTemperatureResult(temperature, stats)


def determine_displacement_robustness_domain(
    layout: SiDBLayout, spec: Sequence[TruthTable], *, params: DisplacementRobustnessDomainParams | None = None
) -> DisplacementRobustnessResult:
    """Analyze which SiDB displacements preserve the specified logic.

    Args:
        layout: SiDB gate layout.
        spec: Truth tables in output order.
        params: Displacement options, simulation settings, and callbacks.

    Returns:
        The displacement domain and analysis statistics.
    """
    options = params if params is not None else DisplacementRobustnessDomainParams()
    stats = DisplacementRobustnessDomainStats()
    domain = _defects.determine_displacement_robustness_domain(layout, spec, options, stats)
    return DisplacementRobustnessResult(domain, stats)


def time_to_solution(
    layout: SiDBLayout, *, quicksim_params: QuickSimParams, params: TimeToSolutionParams | None = None
) -> TimeToSolutionStats:
    """Measure QuickSim accuracy and time to solution against an exact simulation.

    Args:
        layout: SiDB layout.
        quicksim_params: QuickSim settings for repeated trials.
        params: Trial count, reference engine, confidence level, and callbacks.

    Returns:
        Time-to-solution and accuracy statistics.
    """
    options = params if params is not None else TimeToSolutionParams()
    stats = TimeToSolutionStats()
    _analysis.time_to_solution(layout, quicksim_params, options, stats)
    return stats


def time_to_solution_for_given_simulation_results(
    results_exact: SimulationResult, results_heuristic: Sequence[SimulationResult], *, confidence_level: float = 0.997
) -> TimeToSolutionStats:
    """Calculate time to solution from completed exact and heuristic simulations.

    Args:
        results_exact: Exact reference simulation.
        results_heuristic: Independent heuristic trials.
        confidence_level: Target probability of finding a ground state.

    Returns:
        Time-to-solution and accuracy statistics.
    """
    stats = TimeToSolutionStats()
    _analysis.time_to_solution_for_given_simulation_results(results_exact, results_heuristic, confidence_level, stats)
    return stats


class OperationalResult(NamedTuple):
    """Logical operation status and the number of simulator invocations."""

    status: OperationalStatus
    simulator_invocations: int

    def __bool__(self) -> bool:
        """Return whether the layout implements the specified logic."""
        return self.status == OperationalStatus.OPERATIONAL


def is_operational(
    layout: SiDBLayout | Sequence[SiDBLayout],
    spec: Sequence[TruthTable],
    *,
    params: OperationalParams | None = None,
    input_wires: Sequence[BdlWire] | None = None,
    output_wires: Sequence[BdlWire] | None = None,
    canvas: SiDBLayout | None = None,
) -> OperationalResult:
    """Check whether the SiDB ground states implement a Boolean specification.

    Args:
        layout: A gate layout, or independent layouts in input-pattern order.
        spec: Truth tables in output order.
        params: Simulation settings, input encoding, and filtering strategy.
        input_wires: Known input wires; supply together with output_wires.
        output_wires: Known output wires; supply together with input_wires.
        canvas: Explicit pruning canvas when supplying wires.

    Returns:
        Logical operation status and simulator invocation count.

    Raises:
        ValueError: Explicit input patterns or a canvas lack both input and output wires.
    """
    options = params if params is not None else OperationalParams()
    if input_wires is None and output_wires is None and canvas is None and isinstance(layout, SiDBLayout):
        return OperationalResult(*_logic.is_operational(layout, spec, options))
    if input_wires is None or output_wires is None:
        msg = "Explicit input patterns, wires, and canvas require both input and output wires"
        raise ValueError(msg)
    return OperationalResult(*_logic.is_operational(layout, spec, options, input_wires, output_wires, canvas))
