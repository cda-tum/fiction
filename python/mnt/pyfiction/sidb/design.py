# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Generators of SiDB layouts and circuits."""

from __future__ import annotations

from dataclasses import dataclass
from typing import TYPE_CHECKING

from mnt.pyfiction._native.sidb import generators as _generators
from mnt.pyfiction._native.sidb.generators import (
    CircuitDesignParams,
    ComplexGateDesignPolicy,
    GateDesignMode,
    GateDesignParams,
    GateDesignStats,
    OnTheFlyGateLibraryParams,
    PositiveCharges,
    RandomLayoutParams,
    TerminationCondition,
    generate_multiple_random_sidb_layouts,
    generate_random_sidb_layout,
    on_the_fly_sidb_circuit_design,
)

if TYPE_CHECKING:
    from collections.abc import Sequence

    from mnt.pyfiction.sidb import SiDBLayout
    from mnt.pyfiction.synthesis import TruthTable

__all__ = [
    "CircuitDesignParams",
    "ComplexGateDesignPolicy",
    "GateDesignMode",
    "GateDesignParams",
    "GateDesignResult",
    "GateDesignStats",
    "OnTheFlyGateLibraryParams",
    "PositiveCharges",
    "RandomLayoutParams",
    "TerminationCondition",
    "design_sidb_gates",
    "generate_multiple_random_sidb_layouts",
    "generate_random_sidb_layout",
    "on_the_fly_sidb_circuit_design",
]


@dataclass(frozen=True)
class GateDesignResult:
    """Designed SiDB gate layouts and search statistics."""

    layouts: list[SiDBLayout]
    stats: GateDesignStats


def design_sidb_gates(
    skeleton: SiDBLayout, spec: Sequence[TruthTable], *, params: GateDesignParams | None = None
) -> GateDesignResult:
    """Design SiDB gates on the supplied skeleton and canvas.

    Args:
        skeleton: Fixed SiDB input and output structure.
        spec: Truth tables in output order.
        params: Search mode, canvas, termination rule, and callbacks.

    Returns:
        Designed layouts and statistics, including when no solution is found.
    """
    options = params if params is not None else GateDesignParams()
    stats = GateDesignStats()
    layouts = _generators.design_sidb_gates(skeleton, spec, options, stats)
    return GateDesignResult(layouts, stats)
