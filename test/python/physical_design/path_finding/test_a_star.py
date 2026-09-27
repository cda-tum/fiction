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
    Obstructions,
    ShiftedCartesianGateLayout,
)
from mnt.pyfiction.layouts.coords import OffsetCoordinate
from mnt.pyfiction.physical_design.routing import (
    AStarParams,
    a_star,
    a_star_distance,
    enumerate_all_paths,
    yen_k_shortest_paths,
)


def test_search_constraints_are_local() -> None:
    """Searches combine persistent constraints with independent routing data."""
    layout = CartesianGateLayout((2, 2), "2DDWave")
    layout.obstruct_coordinate((1, 0))
    blocked = Obstructions()
    blocked.obstruct_connection((0, 1), (1, 1))
    expected = [(0, 0), (0, 1), (0, 2), (1, 2), (2, 2)]
    for _ in range(2):
        assert a_star(layout, (0, 0), (2, 2), obstructions=blocked) == expected
        assert a_star_distance(layout, (0, 0), (2, 2), obstructions=blocked) == 4
        assert enumerate_all_paths(layout, (0, 0), (2, 2), obstructions=blocked) == [expected]
        assert yen_k_shortest_paths(layout, (0, 0), (2, 2), 4, obstructions=blocked) == [expected]
        assert layout.is_obstructed_coordinate((1, 0))
        assert not layout.is_obstructed_connection((0, 1), (1, 1))
        assert blocked.is_obstructed_connection((0, 1), (1, 1))
        assert not blocked.is_obstructed_coordinate((1, 0))

    grid = CartesianGateLayout((1, 1))
    blocked.obstruct_coordinate((1, 0))
    assert a_star(grid, (0, 0), (1, 1), obstructions=blocked) == []


CLOCKED_LAYOUTS = [
    pytest.param(lambda: CartesianGateLayout((4, 4), "2DDWave", "Layout"), id="CartesianGateLayout"),
    pytest.param(lambda: ShiftedCartesianGateLayout((4, 4), "2DDWave", "Layout"), id="ShiftedCartesianGateLayout"),
    pytest.param(lambda: HexagonalGateLayout((4, 4), "2DDWave", "Layout"), id="HexagonalGateLayout"),
]


@pytest.mark.parametrize(
    "make_lyt",
    [
        pytest.param(lambda: CartesianGateLayout((4, 4)), id="CartesianGateLayout"),
        pytest.param(lambda: ShiftedCartesianGateLayout((4, 4)), id="ShiftedCartesianGateLayout"),
        pytest.param(lambda: HexagonalGateLayout((4, 4)), id="HexagonalGateLayout"),
    ],
)
def test_non_clocked_path_finding(make_lyt):
    lyt = make_lyt()
    assert a_star(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 0)) == [(0, 0)]


@pytest.mark.parametrize("make_lyt", CLOCKED_LAYOUTS)
def test_clocked_path_finding(make_lyt):
    lyt = make_lyt()
    assert a_star(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 0)) == [(0, 0)]
    assert len(a_star(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(1, 1))) == 3
    assert len(a_star(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(2, 2))) == 5
    assert len(a_star(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(3, 3))) == 7
    assert len(a_star(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(4, 4))) == 9
    assert len(a_star(lyt, OffsetCoordinate(1, 1), OffsetCoordinate(0, 0))) == 0
    assert len(a_star(lyt, OffsetCoordinate(2, 2), OffsetCoordinate(1, 1))) == 0


@pytest.mark.parametrize(
    "make_lyt",
    [
        pytest.param(
            lambda: CartesianGateLayout((4, 4), "2DDWave", "Layout"),
            id="CartesianGateLayout",
        ),
        pytest.param(
            lambda: ShiftedCartesianGateLayout((4, 4), "2DDWave", "Layout"),
            id="ShiftedCartesianGateLayout",
        ),
        pytest.param(
            lambda: HexagonalGateLayout((4, 4), "2DDWave", "Layout"),
            id="HexagonalGateLayout",
        ),
    ],
)
def test_path_finding_with_obstructions(make_lyt):
    lyt = make_lyt()
    lyt.obstruct_coordinate(OffsetCoordinate(1, 0))
    lyt.obstruct_coordinate(OffsetCoordinate(1, 1))
    lyt.obstruct_coordinate(OffsetCoordinate(1, 2))
    lyt.obstruct_coordinate(OffsetCoordinate(1, 3))
    lyt.obstruct_coordinate(OffsetCoordinate(1, 4))

    assert lyt.is_obstructed_coordinate(OffsetCoordinate(1, 0))
    assert lyt.is_obstructed_coordinate(OffsetCoordinate(1, 1))
    assert lyt.is_obstructed_coordinate(OffsetCoordinate(1, 2))
    assert lyt.is_obstructed_coordinate(OffsetCoordinate(1, 3))
    assert lyt.is_obstructed_coordinate(OffsetCoordinate(1, 4))

    assert a_star(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 0)) == [(0, 0)]
    assert len(a_star(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(1, 1))) == 3
    assert len(a_star(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(2, 2))) == 0
    assert len(a_star(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(3, 3))) == 0
    assert len(a_star(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(4, 4))) == 0
    assert len(a_star(lyt, OffsetCoordinate(1, 1), OffsetCoordinate(0, 0))) == 0
    assert len(a_star(lyt, OffsetCoordinate(2, 2), OffsetCoordinate(1, 1))) == 0


@pytest.mark.parametrize(
    "make_lyt",
    [
        pytest.param(
            lambda: CartesianGateLayout((2, 1, 1), "2DDWave", "Layout"),
            id="CartesianGateLayout",
        ),
        pytest.param(
            lambda: ShiftedCartesianGateLayout((2, 1, 1), "2DDWave", "Layout"),
            id="ShiftedCartesianGateLayout",
        ),
        pytest.param(
            lambda: HexagonalGateLayout((2, 1, 1), "2DDWave", "Layout"),
            id="HexagonalGateLayout",
        ),
    ],
)
def test_path_finding_with_obstructions_and_crossings(make_lyt):
    lyt = make_lyt()
    x1 = lyt.create_pi("x1", (0, 0))
    lyt.obstruct_coordinate((0, 0, 0))
    lyt.obstruct_coordinate((0, 0, 1))

    x2 = lyt.create_pi("x2", (0, 1))
    lyt.obstruct_coordinate((0, 1, 0))
    lyt.obstruct_coordinate((0, 1, 1))

    b = lyt.create_buf(x1, (1, 0))
    lyt.obstruct_coordinate((1, 0, 0))

    lyt.create_and(x2, b, (1, 1))
    lyt.obstruct_coordinate((1, 1, 0))
    lyt.obstruct_coordinate((1, 1, 1))

    params = AStarParams()
    params.crossings = True

    assert len(a_star(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 0), params)) == 1
    assert len(a_star(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(1, 0), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 1), params)) == 2
    assert len(a_star(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(1, 1), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(2, 0), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(2, 1), params)) == 0

    assert len(a_star(lyt, OffsetCoordinate(1, 0), OffsetCoordinate(0, 0), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(1, 0), OffsetCoordinate(1, 0), params)) == 1
    assert len(a_star(lyt, OffsetCoordinate(1, 0), OffsetCoordinate(0, 1), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(1, 0), OffsetCoordinate(1, 1), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(1, 0), OffsetCoordinate(2, 0), params)) == 2
    assert len(a_star(lyt, OffsetCoordinate(1, 0), OffsetCoordinate(2, 1), params)) == 3

    assert len(a_star(lyt, OffsetCoordinate(0, 1), OffsetCoordinate(0, 0), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(0, 1), OffsetCoordinate(1, 0), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(0, 1), OffsetCoordinate(0, 1), params)) == 1
    assert len(a_star(lyt, OffsetCoordinate(0, 1), OffsetCoordinate(1, 1), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(0, 1), OffsetCoordinate(2, 0), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(0, 1), OffsetCoordinate(2, 1), params)) == 0

    assert len(a_star(lyt, OffsetCoordinate(1, 1), OffsetCoordinate(0, 0), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(1, 1), OffsetCoordinate(1, 0), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(1, 1), OffsetCoordinate(0, 1), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(1, 1), OffsetCoordinate(1, 1), params)) == 1
    assert len(a_star(lyt, OffsetCoordinate(1, 1), OffsetCoordinate(2, 0), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(1, 1), OffsetCoordinate(2, 1), params)) == 2

    assert len(a_star(lyt, OffsetCoordinate(2, 0), OffsetCoordinate(0, 0), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(2, 0), OffsetCoordinate(1, 0), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(2, 0), OffsetCoordinate(0, 1), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(2, 0), OffsetCoordinate(1, 1), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(2, 0), OffsetCoordinate(2, 0), params)) == 1
    assert len(a_star(lyt, OffsetCoordinate(2, 0), OffsetCoordinate(2, 1), params)) == 2

    assert len(a_star(lyt, OffsetCoordinate(2, 1), OffsetCoordinate(0, 0), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(2, 1), OffsetCoordinate(1, 0), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(2, 1), OffsetCoordinate(0, 1), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(2, 1), OffsetCoordinate(1, 1), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(2, 1), OffsetCoordinate(2, 0), params)) == 0
    assert len(a_star(lyt, OffsetCoordinate(2, 1), OffsetCoordinate(2, 1), params)) == 1


@pytest.mark.parametrize("make_lyt", CLOCKED_LAYOUTS)
def test_distance(make_lyt):
    lyt = make_lyt()
    assert a_star_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 0)) == 0
    assert a_star_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(1, 0)) == 1
    assert a_star_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 1)) == 1
    assert a_star_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(1, 1)) == 2
    assert a_star_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(2, 2)) == 4
    assert a_star_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(3, 3)) == 6
    assert a_star_distance(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(4, 4)) == 8
    assert a_star_distance(lyt, OffsetCoordinate(1, 1), OffsetCoordinate(0, 0)) == float("inf")
    assert a_star_distance(lyt, OffsetCoordinate(2, 2), OffsetCoordinate(1, 1)) == float("inf")
