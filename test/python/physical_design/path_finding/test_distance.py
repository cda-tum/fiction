# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.layouts import coordinate
from mnt.pyfiction.physical_design.path_finding import (
    chebyshev_distance,
    euclidean_distance,
    manhattan_distance,
    squared_euclidean_distance,
    twoddwave_distance,
)

if TYPE_CHECKING:
    from collections.abc import Callable


@pytest.mark.parametrize(
    "metric",
    [manhattan_distance, euclidean_distance, squared_euclidean_distance, twoddwave_distance, chebyshev_distance],
)
@pytest.mark.parametrize(("source", "target"), [((0, 0), (3, 4)), ((0, 0, -2), (3, 4, 5))])
def test_distance_tuple_inputs(
    metric: Callable[
        [coordinate | tuple[int, int] | tuple[int, int, int], coordinate | tuple[int, int] | tuple[int, int, int]],
        int | float,
    ],
    source: tuple[int, int] | tuple[int, int, int],
    target: tuple[int, int] | tuple[int, int, int],
) -> None:
    """Convert two- and three-axis tuple inputs to coordinates for every metric."""
    assert metric(source, target) == metric(coordinate(source), coordinate(target))


def test_manhattan() -> None:
    """Measure Manhattan distance between coordinate pairs."""
    assert manhattan_distance(coordinate(0, 0), coordinate(0, 0)) == 0
    assert manhattan_distance(coordinate(0, 0), coordinate(1, 0)) == 1
    assert manhattan_distance(coordinate(0, 0), coordinate(0, 1)) == 1
    assert manhattan_distance(coordinate(0, 0), coordinate(1, 1)) == 2
    assert manhattan_distance(coordinate(0, 0), coordinate(2, 2)) == 4
    assert manhattan_distance(coordinate(0, 0), coordinate(3, 3)) == 6
    assert manhattan_distance(coordinate(0, 0), coordinate(4, 4)) == 8


def test_euclidean() -> None:
    """Measure Euclidean distance between coordinate pairs."""
    assert euclidean_distance(coordinate(0, 0), coordinate(0, 0)) == 0
    assert euclidean_distance(coordinate(0, 0), coordinate(1, 0)) == 1
    assert euclidean_distance(coordinate(0, 0), coordinate(0, 1)) == 1
    assert euclidean_distance(coordinate(0, 0), coordinate(1, 1)) == pytest.approx(2**0.5, abs=1e-7)
    assert euclidean_distance(coordinate(0, 0), coordinate(2, 2)) == pytest.approx(2 * 2**0.5, abs=1e-7)
    assert euclidean_distance(coordinate(0, 0), coordinate(3, 3)) == pytest.approx(3 * 2**0.5, abs=1e-7)
    assert euclidean_distance(coordinate(0, 0), coordinate(4, 4)) == pytest.approx(4 * 2**0.5, abs=1e-7)


def test_squared_euclidean() -> None:
    """Measure squared Euclidean distance between coordinate pairs."""
    assert squared_euclidean_distance(coordinate(0, 0), coordinate(0, 0)) == 0
    assert squared_euclidean_distance(coordinate(0, 0), coordinate(1, 0)) == 1
    assert squared_euclidean_distance(coordinate(0, 0), coordinate(0, 1)) == 1
    assert squared_euclidean_distance(coordinate(0, 0), coordinate(1, 1)) == 2
    assert squared_euclidean_distance(coordinate(0, 0), coordinate(2, 2)) == 8
    assert squared_euclidean_distance(coordinate(0, 0), coordinate(3, 3)) == 18
    assert squared_euclidean_distance(coordinate(0, 0), coordinate(4, 4)) == 32


def test_chebyshev() -> None:
    """Measure Chebyshev distance between coordinate pairs."""
    assert chebyshev_distance(coordinate(0, 0), coordinate(0, 0)) == 0
    assert chebyshev_distance(coordinate(0, 0), coordinate(1, 0)) == 1
    assert chebyshev_distance(coordinate(0, 0), coordinate(0, 1)) == 1
    assert chebyshev_distance(coordinate(0, 0), coordinate(1, 1)) == 1
    assert chebyshev_distance(coordinate(0, 0), coordinate(2, 2)) == 2
    assert chebyshev_distance(coordinate(0, 0), coordinate(3, 3)) == 3
    assert chebyshev_distance(coordinate(0, 0), coordinate(4, 4)) == 4


def test_twoddwave() -> None:
    """Measure forward 2DDWave distance between coordinate pairs."""
    assert twoddwave_distance(coordinate(0, 0), coordinate(0, 0)) == 0
    assert twoddwave_distance(coordinate(0, 0), coordinate(1, 0)) == 1
    assert twoddwave_distance(coordinate(0, 0), coordinate(0, 1)) == 1
    assert twoddwave_distance(coordinate(0, 0), coordinate(1, 1)) == 2
    assert twoddwave_distance(coordinate(0, 0), coordinate(2, 2)) == 4
    assert twoddwave_distance(coordinate(0, 0), coordinate(3, 3)) == 6
    assert twoddwave_distance(coordinate(0, 0), coordinate(4, 4)) == 8


def test_squared_euclidean_range() -> None:
    """Preserve exact squared distances after widening signed differences."""
    assert squared_euclidean_distance(coordinate(-2147483647, 0), coordinate(2147483647, 0)) == 18446744056529682436
