# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Path finding and distance functions on layouts."""

from __future__ import annotations

from typing import TYPE_CHECKING, TypeAlias

from mnt.pyfiction._native.physical_design import (
    ColorRoutingParams,
    GraphColoringEngine,
    clear_routing,
    color_routing,
    extract_routing_objectives,
    is_crossable_wire,
    reserve_input_nodes,
    route_path,
)
from mnt.pyfiction._native.physical_design import (
    place as _place,
)
from mnt.pyfiction._native.physical_design.path_finding import (
    AStarParams,
    EnumerateAllPathsParams,
    YenKShortestPathsParams,
    a_star,
    a_star_distance,
    chebyshev_distance,
    enumerate_all_paths,
    euclidean_distance,
    manhattan_distance,
    squared_euclidean_distance,
    twoddwave_distance,
    yen_k_shortest_paths,
)
from mnt.pyfiction.layouts import CartesianGateLayout, HexagonalGateLayout, ShiftedCartesianGateLayout
from mnt.pyfiction.layouts.coords import OffsetCoordinate

if TYPE_CHECKING:
    from mnt.pyfiction.networks import TechnologyNetwork

__all__ = [
    "AStarParams",
    "ColorRoutingParams",
    "EnumerateAllPathsParams",
    "GraphColoringEngine",
    "YenKShortestPathsParams",
    "a_star",
    "a_star_distance",
    "chebyshev_distance",
    "clear_routing",
    "color_routing",
    "enumerate_all_paths",
    "euclidean_distance",
    "extract_routing_objectives",
    "is_crossable_wire",
    "manhattan_distance",
    "place",
    "reserve_input_nodes",
    "route_path",
    "squared_euclidean_distance",
    "twoddwave_distance",
    "yen_k_shortest_paths",
]


_RoutingLayout: TypeAlias = CartesianGateLayout | ShiftedCartesianGateLayout | HexagonalGateLayout
_BINARY_INPUTS = 2
_TERNARY_INPUTS = 3

_Coordinate: TypeAlias = OffsetCoordinate | tuple[int, int] | tuple[int, int, int]


def place(
    layout: _RoutingLayout,
    tile: _Coordinate,
    network: TechnologyNetwork,
    node: int,
    *inputs: _Coordinate,
    constant: bool | None = None,
) -> OffsetCoordinate:
    """Place one network node with explicit incoming layout coordinates.

    Args:
        layout: The gate layout to modify.
        tile: An empty tile within the layout bounds.
        network: Source technology network.
        node: Source node identifier.
        inputs: Coordinates of zero to three existing input gates or wires.
        constant: Optional third constant input for a majority or general Boolean function.

    Returns:
        The coordinate of the placed gate.

    Raises:
        IndexError: The node identifier is outside the source network.
        ValueError: The gate, input count, destination, or input coordinates are invalid.
    """
    if node < 0 or node >= len(network):
        msg = "network node index out of range"
        raise IndexError(msg)
    if not layout.is_within_bounds(tile) or not layout.is_empty_tile(tile):
        msg = "placement needs an empty tile within layout bounds"
        raise ValueError(msg)
    if any(not layout.is_within_bounds(site) or layout.is_empty_tile(site) for site in inputs):
        msg = "placement inputs must identify existing gates or wires"
        raise ValueError(msg)
    count = len(inputs)
    is_function = network.is_function(node)
    valid_function_arity = is_function and len(network.fanins(node)) == count + (constant is not None)
    if constant is not None and (count != _BINARY_INPUTS or not valid_function_arity):
        msg = "a constant input requires a three-input function with two coordinate inputs"
        raise ValueError(msg)
    if count == 0 and network.is_pi(node):
        return _place(layout, tile, network, node)
    if count == 1 and (network.is_inv(node) or network.is_buf(node)):
        return _place(layout, tile, network, node, inputs[0])
    if count == _BINARY_INPUTS and valid_function_arity:
        return _place(layout, tile, network, node, inputs[0], inputs[1], constant)
    if count == _TERNARY_INPUTS and valid_function_arity:
        return _place(layout, tile, network, node, inputs[0], inputs[1], inputs[2])
    msg = "unsupported gate or wrong number of placement inputs"
    raise ValueError(msg)
