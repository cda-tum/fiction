# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Design rule, equivalence, and performance checks of gate-level layouts."""

from __future__ import annotations

from typing import TYPE_CHECKING, NamedTuple

from mnt.pyfiction._native import verification as _native

if TYPE_CHECKING:
    from mnt.pyfiction.layouts._types import GateLayout
    from mnt.pyfiction.networks import TechnologyNetwork

from mnt.pyfiction._native.verification import (
    DesignRuleParams,
    DesignRuleResult,
    EquivalenceResult,
    EquivalenceType,
    GateCounts,
    count_gate_types,
)

__all__ = [
    "DesignRuleParams",
    "DesignRuleResult",
    "EquivalenceResult",
    "EquivalenceType",
    "GateCounts",
    "LayoutPerformance",
    "count_gate_types",
    "critical_path_length_and_throughput",
    "equivalence_checking",
    "gate_level_drvs",
]


class LayoutPerformance(NamedTuple):
    """Critical path length and the clock-cycle interval between outputs."""

    critical_path_length: int
    throughput: int


def critical_path_length_and_throughput(layout: GateLayout) -> LayoutPerformance:
    """Measure the timing of a clocked gate layout.

    Args:
        layout: Gate layout to inspect.

    Returns:
        Critical path length in clock phases and the clock-cycle interval between outputs.
    """
    return LayoutPerformance(*_native.critical_path_length_and_throughput(layout))


def gate_level_drvs(layout: GateLayout, *, params: DesignRuleParams | None = None) -> DesignRuleResult:
    """Check a gate layout's design rules and return counts and the JSON report.

    Args:
        layout: Gate layout to inspect.
        params: Enabled checks and progress callback. None uses the native defaults.

    Returns:
        Warning and violation counts and a JSON report. This function prints nothing.
    """
    result = DesignRuleResult()
    _native.gate_level_drvs(layout, params if params is not None else DesignRuleParams(), statistics=result)
    return result


def equivalence_checking(
    specification: TechnologyNetwork | GateLayout,
    implementation: TechnologyNetwork | GateLayout,
) -> EquivalenceResult:
    """Check functional and throughput equivalence of networks or gate layouts.

    Args:
        specification: Reference technology network or gate layout.
        implementation: Technology network or gate layout to verify.

    Returns:
        Equivalence type, throughput difference, counterexample, and design rule reports.
        Design rule violations prevent the equivalence check; inspect spec_drv_stats
        and impl_drv_stats before interpreting eq.
    """
    result = EquivalenceResult()
    _native.equivalence_checking(specification, implementation, result)
    return result
