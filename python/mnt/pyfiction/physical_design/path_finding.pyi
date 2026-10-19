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

def manhattan_distance(
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
) -> int:
    """
    The Manhattan distance :math:`D` between two layout coordinates
    :math:`(x_1, y_1)` and :math:`(x_2, y_2)` given by

    :math:`D = |x_1 - x_2| + |y_1 - y_2|`

    Args:
        source: Source coordinate.
        target: Target coordinate.

    Returns:
        Manhattan distance between `source` and `target`.
    """

def euclidean_distance(
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
) -> float:
    """
    The Euclidean distance :math:`D` between two layout coordinates
    :math:`(x_1, y_1)` and :math:`(x_2, y_2)` given by

    :math:`D = \\sqrt{(x_1 - x_2)^2 + (y_1 - y_2)^2}`

    Args:
        source: Source coordinate.
        target: Target coordinate.

    Returns:
        Euclidean distance between `source` and `target`.
    """

def squared_euclidean_distance(
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
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
        source: Source coordinate.
        target: Target coordinate.

    Returns:
        Squared euclidean distance between `source` and `target`.

    Precondition:
        The squared distance is representable by `uint64_t`.
    """

def twoddwave_distance(
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
) -> int:
    """
    The 2DDWave distance :math:`D` between two layout coordinates :math:`s
    = (x_1, y_1)` and :math:`t = (x_2, y_2)` given by

    :math:`D = |x_1 - x_2| + |y_1 - y_2|` iff :math:`s \\leq t` and
    :math:`\\infty`, otherwise.

    Thereby, :math:`s \\leq t` iff :math:`x_1 \\leq x_2` and :math:`y_1 \\leq
    y_2`.

    Args:
        source: Source coordinate.
        target: Target coordinate.

    Returns:
        2DDWave distance between `source` and `target`.

    Note:
        Infinite distances return 4,294,967,295 (2^32 - 1)
        (`std::numeric_limits<uint32_t>::max()` in C++). This 32-bit
        sentinel prevents overflows when adding distances in the default
        64-bit unsigned number range.
    """

def chebyshev_distance(
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
) -> int:
    """
    The Chebyshev distance :math:`D` between two layout coordinates
    :math:`(x_1, y_1)` and :math:`(x_2, y_2)` given by

    :math:`D = \\max(|x_2 - x_1|, |y_2 - y_1|)`

    In contrast to the Manhattan distance, this function assumes the same
    cost for diagonal moves as it does for horizontal and vertical ones.

    Args:
        source: Source coordinate.
        target: Target coordinate.

    Returns:
        Chebyshev distance between `source` and `target`.
    """

@overload
def a_star(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    *,
    crossings: bool = False,
    obstructions: mnt.pyfiction.layouts.obstructions | None = None,
) -> list[mnt.pyfiction.layouts.coordinate]: ...
@overload
def a_star(
    layout: mnt.pyfiction.layouts.cartesian_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    *,
    crossings: bool = False,
    obstructions: mnt.pyfiction.layouts.obstructions | None = None,
) -> list[mnt.pyfiction.layouts.coordinate]: ...
@overload
def a_star(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    *,
    crossings: bool = False,
    obstructions: mnt.pyfiction.layouts.obstructions | None = None,
) -> list[mnt.pyfiction.layouts.coordinate]: ...
@overload
def a_star(
    layout: mnt.pyfiction.layouts.shifted_cartesian_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    *,
    crossings: bool = False,
    obstructions: mnt.pyfiction.layouts.obstructions | None = None,
) -> list[mnt.pyfiction.layouts.coordinate]: ...
@overload
def a_star(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    *,
    crossings: bool = False,
    obstructions: mnt.pyfiction.layouts.obstructions | None = None,
) -> list[mnt.pyfiction.layouts.coordinate]: ...
@overload
def a_star(
    layout: mnt.pyfiction.layouts.hexagonal_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    *,
    crossings: bool = False,
    obstructions: mnt.pyfiction.layouts.obstructions | None = None,
) -> list[mnt.pyfiction.layouts.coordinate]:
    """
    Find a shortest loop-free path from source to target in the layout.
    The keyword options select obstructions and crossing constraints.
    Raises ValueError when source or target lies outside the layout extent.

    Args:
        obstructions: Coordinates and connections to avoid; None supplies no additional obstructions.
        crossings: Allow wires to cross on separate layers.
    """

@overload
def a_star_distance(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> int: ...
@overload
def a_star_distance(
    layout: mnt.pyfiction.layouts.cartesian_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> int: ...
@overload
def a_star_distance(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> int: ...
@overload
def a_star_distance(
    layout: mnt.pyfiction.layouts.shifted_cartesian_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> int: ...
@overload
def a_star_distance(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> int: ...
@overload
def a_star_distance(
    layout: mnt.pyfiction.layouts.hexagonal_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    obstructions: mnt.pyfiction.layouts.obstructions = ...,
) -> int:
    """
    A distance function that does not approximate but compute the actual
    minimum path length on the given layout via A* traversal. Naturally,
    this function cannot be evaluated in :math:`\\mathcal{O}(1)`, but has
    the polynomial complexity of A*.

    If no path between `source` and `target` exists in `layout`, the
    returned distance is 18,446,744,073,709,551,615 (2^64 - 1)
    (`std::numeric_limits<uint64_t>::max()` in C++).

    Args:
        layout: The layout in which the distance between `source` and
                `target` is to be determined.
        source: Source coordinate.
        target: Target coordinate.
        obstructions: Additional constraints; caller and layout
                      obstructions remain unchanged.

    Template Args:
        Lyt: Coordinate layout type.

    Returns:
        Minimum path length between `source` and `target` in `layout`.
    """

@overload
def yen_k_shortest_paths(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    k: int,
    *,
    crossings: bool = False,
    obstructions: mnt.pyfiction.layouts.obstructions | None = None,
) -> list[list[mnt.pyfiction.layouts.coordinate]]: ...
@overload
def yen_k_shortest_paths(
    layout: mnt.pyfiction.layouts.cartesian_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    k: int,
    *,
    crossings: bool = False,
    obstructions: mnt.pyfiction.layouts.obstructions | None = None,
) -> list[list[mnt.pyfiction.layouts.coordinate]]: ...
@overload
def yen_k_shortest_paths(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    k: int,
    *,
    crossings: bool = False,
    obstructions: mnt.pyfiction.layouts.obstructions | None = None,
) -> list[list[mnt.pyfiction.layouts.coordinate]]: ...
@overload
def yen_k_shortest_paths(
    layout: mnt.pyfiction.layouts.shifted_cartesian_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    k: int,
    *,
    crossings: bool = False,
    obstructions: mnt.pyfiction.layouts.obstructions | None = None,
) -> list[list[mnt.pyfiction.layouts.coordinate]]: ...
@overload
def yen_k_shortest_paths(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    k: int,
    *,
    crossings: bool = False,
    obstructions: mnt.pyfiction.layouts.obstructions | None = None,
) -> list[list[mnt.pyfiction.layouts.coordinate]]: ...
@overload
def yen_k_shortest_paths(
    layout: mnt.pyfiction.layouts.hexagonal_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    k: int,
    *,
    crossings: bool = False,
    obstructions: mnt.pyfiction.layouts.obstructions | None = None,
) -> list[list[mnt.pyfiction.layouts.coordinate]]:
    """
    Find up to k shortest loop-free paths from source to target.
    The keyword options select obstructions and crossing constraints.
    Raises ValueError when source or target lies outside the layout extent.

    Args:
        obstructions: Coordinates and connections to avoid; None supplies no additional obstructions.
        crossings: Allow wires to cross on separate layers.
    """

@overload
def enumerate_all_paths(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    *,
    crossings: bool = False,
    obstructions: mnt.pyfiction.layouts.obstructions | None = None,
) -> list[list[mnt.pyfiction.layouts.coordinate]]: ...
@overload
def enumerate_all_paths(
    layout: mnt.pyfiction.layouts.cartesian_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    *,
    crossings: bool = False,
    obstructions: mnt.pyfiction.layouts.obstructions | None = None,
) -> list[list[mnt.pyfiction.layouts.coordinate]]: ...
@overload
def enumerate_all_paths(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    *,
    crossings: bool = False,
    obstructions: mnt.pyfiction.layouts.obstructions | None = None,
) -> list[list[mnt.pyfiction.layouts.coordinate]]: ...
@overload
def enumerate_all_paths(
    layout: mnt.pyfiction.layouts.shifted_cartesian_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    *,
    crossings: bool = False,
    obstructions: mnt.pyfiction.layouts.obstructions | None = None,
) -> list[list[mnt.pyfiction.layouts.coordinate]]: ...
@overload
def enumerate_all_paths(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    *,
    crossings: bool = False,
    obstructions: mnt.pyfiction.layouts.obstructions | None = None,
) -> list[list[mnt.pyfiction.layouts.coordinate]]: ...
@overload
def enumerate_all_paths(
    layout: mnt.pyfiction.layouts.hexagonal_layout,
    source: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    target: mnt.pyfiction.layouts.coordinate | tuple[int, int] | tuple[int, int, int],
    *,
    crossings: bool = False,
    obstructions: mnt.pyfiction.layouts.obstructions | None = None,
) -> list[list[mnt.pyfiction.layouts.coordinate]]:
    """
    Enumerate all loop-free paths from source to target in the layout.
    The keyword options select obstructions and crossing constraints.

    Args:
        crossings: Allow wires to cross on separate layers.
        obstructions: Coordinates and connections to avoid; None supplies no additional obstructions.
    """
