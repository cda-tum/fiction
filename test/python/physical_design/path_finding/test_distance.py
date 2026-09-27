# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction.layouts import (
    CartesianGateLayout,
    HexagonalGateLayout,
    ShiftedCartesianGateLayout,
)
from mnt.pyfiction.layouts.coords import OffsetCoordinate
from mnt.pyfiction.physical_design.path_finding import (
    chebyshev_distance,
    euclidean_distance,
    manhattan_distance,
    squared_euclidean_distance,
    twoddwave_distance,
)

ALL_LAYOUTS = [
    pytest.param(lambda: CartesianGateLayout((4, 4)), id="CartesianGateLayout"),
    pytest.param(lambda: ShiftedCartesianGateLayout((4, 4)), id="ShiftedCartesianGateLayout"),
    pytest.param(lambda: HexagonalGateLayout((4, 4)), id="HexagonalGateLayout"),
]


@pytest.mark.parametrize("make_lyt", ALL_LAYOUTS)
def test_manhattan(make_lyt):
    lyt = make_lyt()
    assert manhattan_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 0)) == 0
    assert manhattan_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(1, 0)) == 1
    assert manhattan_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 1)) == 1
    assert manhattan_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(1, 1)) == 2
    assert manhattan_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(2, 2)) == 4
    assert manhattan_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(3, 3)) == 6
    assert manhattan_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(4, 4)) == 8


@pytest.mark.parametrize("make_lyt", ALL_LAYOUTS)
def test_euclidean(make_lyt):
    lyt = make_lyt()
    assert euclidean_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 0)) == 0
    assert euclidean_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(1, 0)) == 1
    assert euclidean_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 1)) == 1
    assert euclidean_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(1, 1)) == pytest.approx(2**0.5, abs=1e-7)
    assert euclidean_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(2, 2)) == pytest.approx(
        2 * 2**0.5, abs=1e-7
    )
    assert euclidean_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(3, 3)) == pytest.approx(
        3 * 2**0.5, abs=1e-7
    )
    assert euclidean_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(4, 4)) == pytest.approx(
        4 * 2**0.5, abs=1e-7
    )


@pytest.mark.parametrize("make_lyt", ALL_LAYOUTS)
def test_squared_euclidean(make_lyt):
    lyt = make_lyt()
    assert squared_euclidean_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 0)) == 0
    assert squared_euclidean_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(1, 0)) == 1
    assert squared_euclidean_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 1)) == 1
    assert squared_euclidean_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(1, 1)) == 2
    assert squared_euclidean_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(2, 2)) == 8
    assert squared_euclidean_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(3, 3)) == 18
    assert squared_euclidean_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(4, 4)) == 32


@pytest.mark.parametrize("make_lyt", ALL_LAYOUTS)
def test_twoddwave(make_lyt):
    lyt = make_lyt()
    assert twoddwave_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 0)) == 0
    assert twoddwave_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(1, 0)) == 1
    assert twoddwave_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 1)) == 1
    assert twoddwave_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(1, 1)) == 2
    assert twoddwave_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(2, 2)) == 4
    assert twoddwave_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(3, 3)) == 6
    assert twoddwave_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(4, 4)) == 8


@pytest.mark.parametrize("make_lyt", ALL_LAYOUTS)
def test_chebyshev(make_lyt):
    lyt = make_lyt()
    assert chebyshev_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 0)) == 0
    assert chebyshev_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(1, 0)) == 1
    assert chebyshev_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 1)) == 1
    assert chebyshev_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(1, 1)) == 1
    assert chebyshev_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(2, 2)) == 2
    assert chebyshev_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(3, 3)) == 3
    assert chebyshev_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(4, 4)) == 4
