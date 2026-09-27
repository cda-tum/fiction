# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Place, route, transform, and optimize FCN gate layouts."""

from __future__ import annotations

import contextlib
from typing import TYPE_CHECKING, Generic, TypeVar, cast, overload

from mnt.pyfiction._native import physical_design as _native
from mnt.pyfiction._native.physical_design import (
    ClockPhases,
    GoldCostObjective,
    GoldEffortMode,
    GraphOrientedLayoutDesignParams,
    GraphOrientedLayoutDesignStats,
    HexagonalizationIoPinExtensionMode,
    HexagonalizationIoPinRoutingError,
    HexagonalizationParams,
    HexagonalizationStats,
    OrthogonalParams,
    OrthogonalStats,
    PostLayoutOptimizationParams,
    PostLayoutOptimizationStats,
    WiringReductionParams,
    WiringReductionStats,
    apply_bestagon_library,
    apply_qca_one_library,
    apply_sim7_mol_library,
    apply_topolinano_library,
)
from mnt.pyfiction.layouts import (
    CartesianGateLayout,
    EvenColumnCartesianGateLayout,
    EvenColumnHexGateLayout,
    EvenRowCartesianGateLayout,
    HexagonalGateLayout,
    OddColumnHexGateLayout,
    OddRowCartesianGateLayout,
    OddRowHexGateLayout,
    ShiftedCartesianGateLayout,
)

from . import routing

if TYPE_CHECKING:
    from collections.abc import Callable

    from mnt.pyfiction.layouts._types import GateLayout, GateLayoutT
    from mnt.pyfiction.networks import TechnologyNetwork

_Layout_co = TypeVar("_Layout_co", covariant=True)
_Stats_co = TypeVar("_Stats_co", covariant=True)


class LayoutResult(Generic[_Layout_co, _Stats_co]):
    """An algorithm's owned layout and read-only statistics from the same run."""

    __slots__ = ("_layout", "_stats")

    def __init__(self, layout: _Layout_co, stats: _Stats_co) -> None:
        """Pair a result layout with its statistics.

        Args:
            layout: The owned layout, or None when a bounded search finds no layout.
            stats: Statistics from the same algorithm run.
        """
        self._layout = layout
        self._stats = stats

    @property
    def layout(self) -> _Layout_co:
        """The owned layout, or None when a bounded search finds no layout."""
        return self._layout

    @property
    def stats(self) -> _Stats_co:
        """The algorithm's statistics, including unsuccessful searches."""
        return self._stats


_ORTHOGONAL: dict[type[object], Callable[[TechnologyNetwork, OrthogonalParams, OrthogonalStats], GateLayout]] = {
    CartesianGateLayout: _native.orthogonal,
    HexagonalGateLayout: _native.orthogonal_hexagonal,
    OddRowHexGateLayout: _native.orthogonal_odd_row_hex,
    OddColumnHexGateLayout: _native.orthogonal_odd_column_hex,
    EvenColumnHexGateLayout: _native.orthogonal_even_column_hex,
}


@overload
def orthogonal(
    network: TechnologyNetwork, *, params: OrthogonalParams | None = None
) -> LayoutResult[CartesianGateLayout, OrthogonalStats]: ...


@overload
def orthogonal(
    network: TechnologyNetwork, *, layout_type: type[GateLayoutT], params: OrthogonalParams | None = None
) -> LayoutResult[GateLayoutT, OrthogonalStats]: ...


def orthogonal(
    network: TechnologyNetwork,
    *,
    layout_type: type[object] = CartesianGateLayout,
    params: OrthogonalParams | None = None,
) -> LayoutResult[GateLayout, OrthogonalStats]:
    """Place a network with 2DDWave clocking and return layout and statistics.

    Args:
        network: A technology network with at most two inputs per gate.
        layout_type: CartesianGateLayout or one of the four hexagonal gate layouts.
        params: Clock-phase and progress options; None uses native defaults.

    Returns:
        The placed layout and statistics.

    Raises:
        ValueError: The selected topology does not support orthogonal placement.
    """
    if layout_type not in _ORTHOGONAL:
        msg = f"orthogonal does not support {layout_type.__name__}"
        raise ValueError(msg)
    stats = OrthogonalStats()
    layout = _ORTHOGONAL[layout_type](network, params if params is not None else OrthogonalParams(), stats)
    return LayoutResult(layout, stats)


def exact_available() -> bool:
    """Return whether this build includes the Z3 exact-placement solver."""
    return hasattr(_native, "exact_cartesian")


with contextlib.suppress(ImportError):
    from mnt.pyfiction._native.physical_design import ExactParams, ExactStats, TechnologyConstraints

_EXACT: dict[type[object], str] = {
    CartesianGateLayout: "exact_cartesian",
    ShiftedCartesianGateLayout: "exact_shifted_cartesian",
    HexagonalGateLayout: "exact_hexagonal",
    OddRowCartesianGateLayout: "exact_odd_row_cartesian",
    EvenRowCartesianGateLayout: "exact_even_row_cartesian",
    EvenColumnCartesianGateLayout: "exact_even_column_cartesian",
    OddRowHexGateLayout: "exact_odd_row_hex",
    OddColumnHexGateLayout: "exact_odd_column_hex",
    EvenColumnHexGateLayout: "exact_even_column_hex",
}


@overload
def exact(
    network: TechnologyNetwork, *, params: ExactParams | None = None
) -> LayoutResult[CartesianGateLayout | None, ExactStats]: ...


@overload
def exact(
    network: TechnologyNetwork, *, layout_type: type[GateLayoutT], params: ExactParams | None = None
) -> LayoutResult[GateLayoutT | None, ExactStats]: ...


def exact(
    network: TechnologyNetwork, *, layout_type: type[object] = CartesianGateLayout, params: ExactParams | None = None
) -> LayoutResult[GateLayout | None, ExactStats]:
    """Find a minimal layout within the given bounds using Z3.

    Args:
        network: The technology network to place and route.
        layout_type: Any supported concrete gate layout type.
        params: Search bounds, clocking, and progress options; None uses native defaults.

    Returns:
        Layout and statistics. The layout is None when the search finds no solution.

    Raises:
        RuntimeError: This build does not include Z3.
        ValueError: The layout type is unsupported.
    """
    if not exact_available():
        msg = "exact placement requires a pyfiction build with Z3"
        raise RuntimeError(msg)
    if layout_type not in _EXACT:
        msg = f"exact does not support {layout_type.__name__}"
        raise ValueError(msg)
    algorithm = cast(
        "Callable[[TechnologyNetwork, ExactParams, ExactStats], GateLayout | None]",
        getattr(_native, _EXACT[layout_type]),
    )
    stats = ExactStats()
    layout = algorithm(network, params if params is not None else ExactParams(), stats)
    return LayoutResult(layout, stats)


def graph_oriented_layout_design(
    network: TechnologyNetwork,
    *,
    params: GraphOrientedLayoutDesignParams | None = None,
    custom_cost_objective: Callable[[CartesianGateLayout], int] | None = None,
) -> LayoutResult[CartesianGateLayout | None, GraphOrientedLayoutDesignStats]:
    """Run graph-oriented placement and return the candidate and its statistics.

    Args:
        network: The technology network to place and route.
        params: Search effort, objectives, seed, and progress options.
        custom_cost_objective: Optional cost function evaluated on candidate layouts.

    Returns:
        Layout and statistics; the layout is None when the search finds no solution.
    """
    stats = GraphOrientedLayoutDesignStats()
    layout = _native.graph_oriented_layout_design(
        network, params if params is not None else GraphOrientedLayoutDesignParams(), stats, custom_cost_objective
    )
    return LayoutResult(layout, stats)


def hexagonalization(
    layout: CartesianGateLayout, *, params: HexagonalizationParams | None = None
) -> LayoutResult[HexagonalGateLayout, HexagonalizationStats]:
    """Convert a Cartesian gate layout to an even-row hexagonal layout.

    Args:
        layout: A 2DDWave-clocked Cartesian layout.
        params: Input/output extension and progress options.

    Returns:
        An independent hexagonal layout and conversion statistics.
    """
    stats = HexagonalizationStats()
    result = _native.hexagonalization(layout, params if params is not None else HexagonalizationParams(), stats)
    return LayoutResult(result, stats)


def post_layout_optimization(
    layout: CartesianGateLayout, *, params: PostLayoutOptimizationParams | None = None
) -> LayoutResult[CartesianGateLayout, PostLayoutOptimizationStats]:
    """Optimize an independent copy of a Cartesian layout.

    Args:
        layout: A 2DDWave-clocked Cartesian layout.
        params: Relocation, timeout, and progress options.

    Returns:
        The optimized copy and statistics. The input layout remains unchanged.
    """
    result = layout.clone()
    stats = PostLayoutOptimizationStats()
    _native.post_layout_optimization(result, params if params is not None else PostLayoutOptimizationParams(), stats)
    return LayoutResult(result, stats)


def wiring_reduction(
    layout: CartesianGateLayout, *, params: WiringReductionParams | None = None
) -> LayoutResult[CartesianGateLayout, WiringReductionStats]:
    """Shorten wires in an independent copy of a Cartesian layout.

    Args:
        layout: A 2DDWave-clocked Cartesian layout.
        params: Timeout and progress options.

    Returns:
        The optimized copy and statistics. The input layout remains unchanged.
    """
    result = layout.clone()
    stats = WiringReductionStats()
    _native.wiring_reduction(result, params if params is not None else WiringReductionParams(), stats)
    return LayoutResult(result, stats)


__all__ = [
    "ClockPhases",
    "GoldCostObjective",
    "GoldEffortMode",
    "GraphOrientedLayoutDesignParams",
    "GraphOrientedLayoutDesignStats",
    "HexagonalizationIoPinExtensionMode",
    "HexagonalizationIoPinRoutingError",
    "HexagonalizationParams",
    "HexagonalizationStats",
    "LayoutResult",
    "OrthogonalParams",
    "OrthogonalStats",
    "PostLayoutOptimizationParams",
    "PostLayoutOptimizationStats",
    "WiringReductionParams",
    "WiringReductionStats",
    "apply_bestagon_library",
    "apply_qca_one_library",
    "apply_sim7_mol_library",
    "apply_topolinano_library",
    "exact",
    "exact_available",
    "graph_oriented_layout_design",
    "hexagonalization",
    "orthogonal",
    "post_layout_optimization",
    "routing",
    "wiring_reduction",
]

if exact_available():
    __all__ += ["ExactParams", "ExactStats", "TechnologyConstraints"]
