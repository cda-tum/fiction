# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Path finding and distance functions on layouts."""

from typing import overload

import mnt.pyfiction.layouts
import mnt.pyfiction.layouts.coords

@overload
def manhattan_distance(
    layout: mnt.pyfiction.layouts.cartesian_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
) -> int: ...
@overload
def manhattan_distance(
    layout: mnt.pyfiction.layouts.shifted_cartesian_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
) -> int: ...
@overload
def manhattan_distance(
    layout: mnt.pyfiction.layouts.hexagonal_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
) -> int:
    """
    The Manhattan distance :math:`D` between two layout coordinates
    :math:`(x_1, y_1)` and :math:`(x_2, y_2)` given by

    :math:`D = |x_1 - x_2| + |y_1 - y_2|`

    Args:
        lyt: Layout.
        source: Source coordinate.
        target: Target coordinate.

    Template Args:
        Lyt: Coordinate layout type.
        Dist: Integral type for the distance.

    Returns:
        Manhattan distance between `source` and `target`.
    """

@overload
def euclidean_distance(
    layout: mnt.pyfiction.layouts.cartesian_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
) -> float: ...
@overload
def euclidean_distance(
    layout: mnt.pyfiction.layouts.shifted_cartesian_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
) -> float: ...
@overload
def euclidean_distance(
    layout: mnt.pyfiction.layouts.hexagonal_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
) -> float:
    """
    The Euclidean distance :math:`D` between two layout coordinates
    :math:`(x_1, y_1)` and :math:`(x_2, y_2)` given by

    :math:`D = \\sqrt{(x_1 - x_2)^2 + (y_1 - y_2)^2}`

    Args:
        lyt: Layout.
        source: Source coordinate.
        target: Target coordinate.

    Template Args:
        Lyt: Coordinate layout type.
        Dist: Floating-point type for the distance.

    Returns:
        Euclidean distance between `source` and `target`.
    """

@overload
def squared_euclidean_distance(
    layout: mnt.pyfiction.layouts.cartesian_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
) -> int: ...
@overload
def squared_euclidean_distance(
    layout: mnt.pyfiction.layouts.shifted_cartesian_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
) -> int: ...
@overload
def squared_euclidean_distance(
    layout: mnt.pyfiction.layouts.hexagonal_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
) -> int:
    """
    The squared Euclidean distance :math:`D` between two layout
    coordinates :math:`(x_1, y_1)` and :math:`(x_2, y_2)` given by

    :math:`D = \\sqrt{(x_1 - x_2)^2 + (y_1 - y_2)^2}^2 = (x_1 - x_2)^2 +
    (y_1 - y_2)^2`

    In contrast to the regular Euclidean distance, this function is
    differentiable and can be used in optimization algorithms that require
    gradients. Additionally, it is computationally cheaper by omitting the
    square root operation.

    Args:
        lyt: Layout.
        source: Source coordinate.
        target: Target coordinate.

    Template Args:
        Lyt: Coordinate layout type.
        Dist: Integral type for the distance.

    Returns:
        Squared euclidean distance between `source` and `target`.
    """

@overload
def twoddwave_distance(
    layout: mnt.pyfiction.layouts.cartesian_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
) -> int: ...
@overload
def twoddwave_distance(
    layout: mnt.pyfiction.layouts.shifted_cartesian_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
) -> int: ...
@overload
def twoddwave_distance(
    layout: mnt.pyfiction.layouts.hexagonal_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
) -> int:
    """
    The 2DDWave distance :math:`D` between two layout coordinates :math:`s
    = (x_1, y_1)` and :math:`t = (x_2, y_2)` given by

    :math:`D = |x_1 - x_2| + |y_1 - y_2|` iff :math:`s \\leq t` and
    :math:`\\infty`, otherwise.

    Thereby, :math:`s \\leq t` iff :math:`x_1 \\leq x_2` and :math:`y_1 \\leq
    y_2`.

    Args:
        lyt: Layout.
        source: Source coordinate.
        target: Target coordinate.

    Template Args:
        Lyt: Coordinate layout type.
        Dist: Integral type for the distance.

    Returns:
        2DDWave distance between `source` and `target`.

    Note:
        To represent :math:`\\infty`,
        `std::numeric_limits<uint32_t>::max()` is returned for distances
        of infinite length. We are using `uint32_t` to prevent overflows
        when adding distances in the default `uint64_t` number range.
    """

@overload
def chebyshev_distance(
    layout: mnt.pyfiction.layouts.cartesian_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
) -> int: ...
@overload
def chebyshev_distance(
    layout: mnt.pyfiction.layouts.shifted_cartesian_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
) -> int: ...
@overload
def chebyshev_distance(
    layout: mnt.pyfiction.layouts.hexagonal_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
) -> int:
    """
    The Chebyshev distance :math:`D` between two layout coordinates
    :math:`(x_1, y_1)` and :math:`(x_2, y_2)` given by

    :math:`D = \\max(|x_2 - x_1|, |y_2 - y_1|)`

    In contrast to the Manhattan distance, this function assumes the same
    cost for diagonal moves as it does for horizontal and vertical ones.

    Args:
        lyt: Layout.
        source: Source coordinate.
        target: Target coordinate.

    Template Args:
        Lyt: Coordinate layout type.
        Dist: Integral type for the distance.

    Returns:
        Chebyshev distance between `source` and `target`.
    """

class a_star_params:
    """Parameters for the A* algorithm."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def crossings(self) -> bool:
        """
        Allow paths to cross over obstructed tiles if they are occupied by
        wire segments.
        """

    @crossings.setter
    def crossings(self, arg: bool, /) -> None: ...

@overload
def a_star(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    params: a_star_params = ...,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> list[mnt.pyfiction.layouts.coords.offset_coordinate]: ...
@overload
def a_star(
    layout: mnt.pyfiction.layouts.cartesian_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    params: a_star_params = ...,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> list[mnt.pyfiction.layouts.coords.offset_coordinate]: ...
@overload
def a_star(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    params: a_star_params = ...,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> list[mnt.pyfiction.layouts.coords.offset_coordinate]: ...
@overload
def a_star(
    layout: mnt.pyfiction.layouts.shifted_cartesian_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    params: a_star_params = ...,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> list[mnt.pyfiction.layouts.coords.offset_coordinate]: ...
@overload
def a_star(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    params: a_star_params = ...,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> list[mnt.pyfiction.layouts.coords.offset_coordinate]: ...
@overload
def a_star(
    layout: mnt.pyfiction.layouts.hexagonal_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    params: a_star_params = ...,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> list[mnt.pyfiction.layouts.coords.offset_coordinate]:
    """
    The A* path finding algorithm for shortest loop-less paths between a
    given source and target coordinate in a layout. This function
    automatically detects whether the given layout implements a clocking
    interface (see `gate_level_layout`) and respects the underlying
    information flow imposed by `layout`'s clocking scheme.

    A* is an extension of Dijkstra's algorithm for shortest paths but
    offers better average complexity. It uses a heuristic distance
    function that estimates the remaining costs towards the target in
    every step. Thus, this heuristic function should neither be complex to
    calculate nor overestimating the remaining costs. Common heuristics to
    be used are the Manhattan and the Euclidean distance functions. See
    `distance_functor` for implementations.

    Paths do not pass obstructed coordinates or connections, except that
    the target is never obstructed. A coordinate or connection is
    obstructed if the `obstructions` argument marks it or if the layout's
    `is_obstructed_coordinate` or `is_obstructed_connection` reports it.
    Gate-level layouts report their occupied tiles and existing signal
    connections, so paths in gate-level layouts avoid all placed gates and
    wires.

    If crossings are enabled in the parameters, paths in gate-level
    layouts may cross other wires on the crossing layer. Wire crossings
    are only allowed over other wires and only if the crossing layer is
    not obstructed. Furthermore, it is ensured that crossings do not run
    along another wire but cross only in a single point (orthogonal
    crossings + knock-knees/double wires).

    In certain cases it might be desirable to determine regular coordinate
    paths even if the layout implements a clocking interface. This can be
    achieved by static-casting the layout to a coordinate layout when
    calling this function:
    ```
    {.cpp}
    using clk_lyt = gate_level_layout<cartesian_layout<>>;
    using path = layout_coordinate_path<cartesian_layout<>>;
    clk_lyt layout = ...;
    auto shortest_path = a_star<path>(static_cast<cartesian_layout<>>(layout), {source, target});
    ```




    A* was introduced in \\"A Formal Basis for the Heuristic Determination
    of Minimum Cost Paths\\" by Peter E. Hart, Nils J. Nilsson, and Bertram
    Raphael in IEEE Transactions on Systems Science and Cybernetics 1968,
    Volume 4, Issue 2.

    This implementation is based on the pseudocode from
    https://en.wikipedia.org/wiki/A_star_search_algorithm.

    Args:
        layout: The layout in which the shortest path between a source and
                target coordinate is to be found.
        objective: Source-target coordinate pair.
        dist_fn: A distance functor that implements the desired heuristic
                 estimation function.
        cost_fn: A cost functor that implements the desired cost function.
        params: Parameters.
        obstructions: Additional coordinate and connection constraints;
                      the search does not modify them.

    Template Args:
        Path: Type of the returned path.
        Lyt: Type of the layout to perform path finding on.
        Dist: Distance value type to be used in the heuristic estimation
              function.
        Cost: Cost value type to be used when determining moving cost
              between coordinates.

    Returns:
        The shortest loop-less path in `layout` from `objective.source` to
        `objective.target`.
    """

@overload
def a_star_distance(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> float: ...
@overload
def a_star_distance(
    layout: mnt.pyfiction.layouts.cartesian_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> float: ...
@overload
def a_star_distance(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> float: ...
@overload
def a_star_distance(
    layout: mnt.pyfiction.layouts.shifted_cartesian_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> float: ...
@overload
def a_star_distance(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> float: ...
@overload
def a_star_distance(
    layout: mnt.pyfiction.layouts.hexagonal_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> float:
    """
    A distance function that does not approximate but compute the actual
    minimum path length on the given layout via A* traversal. Naturally,
    this function cannot be evaluated in :math:`\\mathcal{O}(1)`, but has
    the polynomial complexity of A*.

    If no path between `source` and `target` exists in `layout`, the
    returned distance is `std::numeric_limits<Dist>::infinity()` for
    floating-point types or `std::numeric_limits<Dist>::max()` for
    integral types.

    Args:
        layout: The layout in which the distance between `source` and
                `target` is to be determined.
        source: Source coordinate.
        target: Target coordinate.
        obstructions: Additional constraints; caller and layout
                      obstructions remain unchanged.

    Template Args:
        Lyt: Coordinate layout type.
        Dist: Distance type.

    Returns:
        Minimum path length between `source` and `target` in `layout`.
    """

class yen_k_shortest_paths_params:
    """Parameters for Yen's :math:`k`-shortest paths algorithm."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def a_star_params(self) -> a_star_params:
        """Parameters for the internal A* algorithm."""

    @a_star_params.setter
    def a_star_params(self, arg: a_star_params, /) -> None: ...

@overload
def yen_k_shortest_paths(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    k: int,
    params: yen_k_shortest_paths_params = ...,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> list[list[mnt.pyfiction.layouts.coords.offset_coordinate]]: ...
@overload
def yen_k_shortest_paths(
    layout: mnt.pyfiction.layouts.cartesian_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    k: int,
    params: yen_k_shortest_paths_params = ...,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> list[list[mnt.pyfiction.layouts.coords.offset_coordinate]]: ...
@overload
def yen_k_shortest_paths(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    k: int,
    params: yen_k_shortest_paths_params = ...,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> list[list[mnt.pyfiction.layouts.coords.offset_coordinate]]: ...
@overload
def yen_k_shortest_paths(
    layout: mnt.pyfiction.layouts.shifted_cartesian_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    k: int,
    params: yen_k_shortest_paths_params = ...,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> list[list[mnt.pyfiction.layouts.coords.offset_coordinate]]: ...
@overload
def yen_k_shortest_paths(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    k: int,
    params: yen_k_shortest_paths_params = ...,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> list[list[mnt.pyfiction.layouts.coords.offset_coordinate]]: ...
@overload
def yen_k_shortest_paths(
    layout: mnt.pyfiction.layouts.hexagonal_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    k: int,
    params: yen_k_shortest_paths_params = ...,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> list[list[mnt.pyfiction.layouts.coords.offset_coordinate]]:
    """
    Yen's algorithm for finding up to :math:`k` shortest paths without
    loops from a source to a target coordinate. If :math:`k` is larger
    than the number of possible paths from source to target, the size of
    the returned path collection will be smaller than :math:`k`.

    This implementation uses the A* algorithm with the Manhattan distance
    function internally.

    This function automatically detects whether the given layout
    implements a clocking interface (see `gate_level_layout`) and respects
    the underlying information flow imposed by `layout`'s clocking scheme.
    This algorithm does neither generate duplicate nor looping paths, even
    in a cyclic clocking scheme. That is, along each path, each coordinate
    can occur at maximum once.

    Paths do not pass obstructed coordinates or connections, except that
    the target is never obstructed. A coordinate or connection is
    obstructed if the `obstructions` argument marks it or if the layout's
    `is_obstructed_coordinate` or `is_obstructed_connection` reports it.
    Gate-level layouts report their occupied tiles and existing signal
    connections, so paths in gate-level layouts avoid all placed gates and
    wires.

    If crossings are enabled in the parameters, paths in gate-level
    layouts may cross other wires on the crossing layer. Wire crossings
    are only allowed over other wires and only if the crossing layer is
    not obstructed. Furthermore, it is ensured that crossings do not run
    along another wire but cross only in a single point (orthogonal
    crossings + knock-knees/double wires).

    In certain cases it might be desirable to enumerate regular coordinate
    paths even if the layout implements a clocking interface. This can be
    achieved by static-casting the layout to a coordinate layout when
    calling this function:
    ```
    {.cpp}
    using clk_lyt = gate_level_layout<cartesian_layout<>>;
    using path = layout_coordinate_path<cartesian_layout<>>;
    clk_lyt layout = ...;
    auto k_paths = yen_k_shortest_paths<path>(static_cast<cartesian_layout<>>(layout), {source, target}, k);
    ```




    The algorithm was originally described in \\"An algorithm for finding
    shortest routes from all source nodes to a given destination in
    general networks\\" by Jin Y. Yen in Quarterly of Applied Mathematics,
    1970.

    Args:
        layout: The layout in which the :math:`k` shortest paths are to be
                found.
        objective: Source-target coordinate pair.
        k: Maximum number of shortest paths to find.
        params: Parameters.
        obstructions: Additional constraints; caller and layout
                      obstructions remain unchanged.

    Template Args:
        Path: Type of the returned individual paths.
        Lyt: Type of the layout to perform path finding on.

    Returns:
        A collection of up to :math:`k` shortest loop-less paths in
        `layout` from `objective.source` to `objective.target`.
    """

class enumerate_all_paths_params:
    """Parameters for the algorithm that enumerates all paths in a layout."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def crossings(self) -> bool:
        """
        Allow paths to cross over obstructed tiles if they are occupied by
        wire segments.
        """

    @crossings.setter
    def crossings(self, arg: bool, /) -> None: ...

@overload
def enumerate_all_paths(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    params: enumerate_all_paths_params = ...,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> list[list[mnt.pyfiction.layouts.coords.offset_coordinate]]: ...
@overload
def enumerate_all_paths(
    layout: mnt.pyfiction.layouts.cartesian_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    params: enumerate_all_paths_params = ...,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> list[list[mnt.pyfiction.layouts.coords.offset_coordinate]]: ...
@overload
def enumerate_all_paths(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    params: enumerate_all_paths_params = ...,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> list[list[mnt.pyfiction.layouts.coords.offset_coordinate]]: ...
@overload
def enumerate_all_paths(
    layout: mnt.pyfiction.layouts.shifted_cartesian_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    params: enumerate_all_paths_params = ...,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> list[list[mnt.pyfiction.layouts.coords.offset_coordinate]]: ...
@overload
def enumerate_all_paths(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    params: enumerate_all_paths_params = ...,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> list[list[mnt.pyfiction.layouts.coords.offset_coordinate]]: ...
@overload
def enumerate_all_paths(
    layout: mnt.pyfiction.layouts.hexagonal_layout,
    source: mnt.pyfiction.layouts.coords.offset_coordinate,
    target: mnt.pyfiction.layouts.coords.offset_coordinate,
    params: enumerate_all_paths_params = ...,
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> list[list[mnt.pyfiction.layouts.coords.offset_coordinate]]:
    """
    Enumerates all possible paths in a layout that start at a given source
    coordinate and lead to given target coordinate. This function
    automatically detects whether the given layout implements a clocking
    interface (see `gate_level_layout`) and respects the underlying
    information flow imposed by `layout`'s clocking scheme. This algorithm
    does neither generate duplicate nor looping paths, even in a cyclic
    clocking scheme. That is, along each path, each coordinate can occur
    at maximum once.

    Paths do not pass obstructed coordinates or connections, except that
    the target is never obstructed. A coordinate or connection is
    obstructed if the `obstructions` argument marks it or if the layout's
    `is_obstructed_coordinate` or `is_obstructed_connection` reports it.
    Gate-level layouts report their occupied tiles and existing signal
    connections, so paths in gate-level layouts avoid all placed gates and
    wires.

    If crossings are enabled in the parameters, paths in gate-level
    layouts may cross other wires on the crossing layer. Wire crossings
    are only allowed over other wires and only if the crossing layer is
    not obstructed. Furthermore, it is ensured that crossings do not run
    along another wire but cross only in a single point (orthogonal
    crossings + knock-knees/double wires).

    In certain cases it might be desirable to enumerate regular coordinate
    paths even if the layout implements a clocking interface. This can be
    achieved by static-casting the layout to a coordinate layout when
    calling this function:
    ```
    {.cpp}
    using clk_lyt = gate_level_layout<cartesian_layout<>>;
    using path = layout_coordinate_path<cartesian_layout<>>;
    clk_lyt layout = ...;
    auto all_paths = enumerate_all_paths<path>(static_cast<cartesian_layout<>>(layout), {source, target});
    ```



    Args:
        layout: The layout whose paths are to be enumerated.
        objective: Source-target coordinate pair.
        params: Parameters.
        obstructions: Additional coordinate and connection constraints;
                      the search does not modify them.

    Template Args:
        Path: Type of the returned individual paths.
        Lyt: Type of the layout to perform path finding on.

    Returns:
        A collection of all unique paths in `layout` from
        `objective.source` to `objective.target`.
    """
