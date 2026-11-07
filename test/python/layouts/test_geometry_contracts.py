# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import copy
from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.layouts import arrangement, cartesian_layout, coordinate, extent, hexagonal_layout

if TYPE_CHECKING:
    from collections.abc import Callable


def test_geometry_sizes_and_empty_axes(geometry_factory: Callable) -> None:
    layout = geometry_factory((5, 4, 2))
    assert (layout.width(), layout.height(), layout.layers(), layout.area()) == (5, 4, 2, 20)
    assert layout.last_coordinate() == coordinate(4, 3, 1)
    assert layout.coord(-1, 2, 7) == coordinate(-1, 2, 7)
    for size in [(0, 4, 2), (5, 0, 2), (5, 4, 0), (0, 0, 0)]:
        empty = geometry_factory(size)
        assert empty.coordinates() == []
        assert empty.ground_coordinates() == []
        assert empty.last_coordinate() is None
        assert not empty.contains_coordinate((0, 0))
        assert not empty.is_at_any_border((0, 0))
        assert empty.adjacent_coordinates((0, 0)) == []
        for direction in ("north", "north_east", "east", "south_east", "south", "south_west", "west", "north_west"):
            assert getattr(empty, direction)((0, 0)) is None, (size, direction)
        for side in ("northern", "eastern", "southern", "western"):
            assert getattr(empty, f"{side}_border_of")((0, 0)) is None


def test_geometry_ranges_cross_rows_and_layers(geometry_factory: Callable) -> None:
    layout = geometry_factory((2, 2, 2))
    expected = [coordinate(x, y, z) for z in range(2) for y in range(2) for x in range(2)]
    assert layout.coordinates() == expected
    assert layout.ground_coordinates() == expected[:4]
    assert layout.coordinates(start=(1, 0), stop=(1, 1, 1)) == expected[1:7]
    assert layout.coordinates(start=(0, 0), stop=(0, 0)) == []
    assert layout.coordinates(start=(-1, -1, -1)) == expected
    assert layout.coordinates(start=(0, 0, 9)) == []
    assert layout.coordinates(stop=(2, 0)) == expected[:2]
    with pytest.raises(ValueError, match="layer zero"):
        layout.ground_coordinates(start=(0, 0, 1))
    wide = geometry_factory((2**31, 1))
    assert wide.coordinates(start=(2**31 - 1, 0)) == [coordinate(2**31 - 1, 0)]


def test_geometry_borders_and_neighbors(geometry_factory: Callable) -> None:
    layout = geometry_factory((5, 4, 2))
    for c in layout.coordinates():
        assert layout.contains_coordinate(c)
        assert layout.is_ground_layer(c) == (c.z == 0)
        for direction, missing in [
            ("north", c.y == 0),
            ("east", c.x == 4),
            ("south", c.y == 3),
            ("west", c.x == 0),
            ("above", c.z == 1),
            ("below", c.z == 0),
        ]:
            assert (getattr(layout, direction)(c) is None) == missing, (c, direction)
        for side, boundary, expected in [
            ("northern", c.y == 0, (c.x, 0, c.z)),
            ("eastern", c.x == 4, (4, c.y, c.z)),
            ("southern", c.y == 3, (c.x, 3, c.z)),
            ("western", c.x == 0, (0, c.y, c.z)),
        ]:
            assert getattr(layout, f"is_at_{side}_border")(c) == boundary, (c, side)
            assert getattr(layout, f"{side}_border_of")(c) == coordinate(expected)
        assert layout.is_at_any_border(c) == (c.x in (0, 4) or c.y in (0, 3))
        neighbors = layout.adjacent_coordinates(c)
        assert len(neighbors) == len(set(neighbors))
        for neighbor in neighbors:
            assert neighbor != c
            assert layout.contains_coordinate(neighbor)
            assert c in layout.adjacent_coordinates(neighbor)
            assert layout.is_adjacent_of(c, neighbor)
        for direction in ("north_east", "south_east", "south_west", "north_west"):
            neighbor = getattr(layout, direction)(c)
            if neighbor is not None:
                assert neighbor != c
                assert layout.contains_coordinate(neighbor)
                if not isinstance(layout, cartesian_layout):
                    assert layout.is_adjacent_of(c, neighbor)
    for c in [(-1, 0), (5, 0), (0, 4), (0, 0, -1), (0, 0, 2)]:
        assert not layout.is_within_bounds(c)
        assert not layout.contains_coordinate(c)
        assert not layout.is_at_any_border(c)


def test_geometry_predicates_ignore_the_frame(geometry_factory: Callable) -> None:
    layout = geometry_factory((0, 0, 0))
    for direction, target, distant in [
        ("north", (-3, -3), (-3, -8)),
        ("east", (-2, -2), (4, -2)),
        ("south", (-3, -1), (-3, 4)),
        ("west", (-4, -2), (-8, -2)),
    ]:
        assert getattr(layout, f"is_{direction}_of")((-3, -2), target)
        assert not getattr(layout, f"is_{direction}_of")((-3, -2), distant)
        assert getattr(layout, f"is_{direction}wards_of")((-3, -2), distant)
    assert layout.is_above((-3, -2, 8), (-3, -2, 9))
    assert layout.is_below((-3, -2, -8), (-3, -2, -9))
    assert layout.is_adjacent_of((-3, -2), (-2, -2))
    assert layout.is_adjacent_elevation_of((-3, -2, 8), (-2, -2, 9))
    assert not layout.is_adjacent_elevation_of((-3, -2, 8), (-2, -2, 10))
    assert not layout.is_east_of((2**31 - 1, 0), (-(2**31), 0))
    assert not layout.is_adjacent_of((7, 0), (0, 0))
    assert not layout.is_adjacent_of((-1, -2), (-1, -2))


def test_geometry_signed_neighbor_queries(geometry_factory: Callable) -> None:
    layout = geometry_factory((7, 7, 2))
    for c in [
        (0, 0),
        (2, -(2**31)),
        (2**31 - 1, 0),
        (0, 2**31 - 1),
        (2**31 - 1, 2**31 - 1),
        (-(2**31) + 1, 0),
        (0, -(2**31) + 1),
        (-1, 1),
        (7, 1),
    ]:
        for neighbor in layout.adjacent_coordinates(c):
            assert layout.contains_coordinate(neighbor), c
        for direction in ("north_east", "south_east", "south_west", "north_west"):
            neighbor = getattr(layout, direction)(c)
            if neighbor is not None:
                assert neighbor != coordinate(c)
                assert layout.contains_coordinate(neighbor), (c, direction)
    assert layout.adjacent_coordinates((-(2**31), 0)) == []
    wide = geometry_factory((2**31, 2**31, 2))
    assert wide.eastern_border_of((0, 0)) == coordinate(2**31 - 1, 0)
    assert wide.southern_border_of((0, 0)) == coordinate(0, 2**31 - 1)
    assert wide.east((2**31 - 1, 0)) is None
    assert wide.south((0, 2**31 - 1)) is None


def test_geometry_copies_retain_arrangement(geometry_factory: Callable) -> None:
    original = geometry_factory((3, 3))
    for duplicate in (copy.copy(original), copy.deepcopy(original)):
        duplicate.resize((4, 5, 2))
        assert original.get_extent() == extent(3, 3)
        if not isinstance(original, cartesian_layout):
            assert duplicate.get_arrangement() == original.get_arrangement()


def test_cartesian_direction_and_opposite_pairs() -> None:
    layout = cartesian_layout((3, 3, 2))
    for direction, expected in [
        ("north", (1, 0)),
        ("north_east", (2, 0)),
        ("east", (2, 1)),
        ("south_east", (2, 2)),
        ("south", (1, 2)),
        ("south_west", (0, 2)),
        ("west", (0, 1)),
        ("north_west", (0, 0)),
        ("above", (1, 1, 1)),
    ]:
        assert getattr(layout, direction)((1, 1)) == coordinate(expected)
    assert layout.below((1, 1, 1)) == coordinate(1, 1)
    assert sorted(tuple(sorted(pair)) for pair in layout.adjacent_opposite_coordinates((1, 1))) == sorted([
        (coordinate(1, 0), coordinate(1, 2)),
        (coordinate(0, 1), coordinate(2, 1)),
    ])
    assert layout.adjacent_opposite_coordinates((0, 0)) == []


@pytest.mark.parametrize(
    ("a", "diagonals", "edge", "opposites"),
    [
        (
            arrangement.ODD_ROW,
            [(2, 1), (2, 3), (1, 3), (1, 1)],
            [(0, 1), (1, 0)],
            [((2, 1), (1, 3)), ((3, 2), (1, 2)), ((1, 1), (2, 3))],
        ),
        (
            arrangement.EVEN_ROW,
            [(3, 1), (3, 3), (2, 3), (2, 1)],
            [(0, 1), (1, 0), (1, 1)],
            [((3, 1), (2, 3)), ((3, 2), (1, 2)), ((2, 1), (3, 3))],
        ),
        (
            arrangement.ODD_COLUMN,
            [(3, 1), (3, 2), (1, 2), (1, 1)],
            [(0, 1), (1, 0)],
            [((2, 1), (2, 3)), ((3, 1), (1, 2)), ((1, 1), (3, 2))],
        ),
        (
            arrangement.EVEN_COLUMN,
            [(3, 2), (3, 3), (1, 3), (1, 2)],
            [(0, 1), (1, 0), (1, 1)],
            [((2, 1), (2, 3)), ((3, 2), (1, 3)), ((1, 2), (3, 3))],
        ),
    ],
)
def test_hexagonal_arrangement_neighbors(a, diagonals, edge, opposites) -> None:
    layout = hexagonal_layout(a, (5, 5, 2))
    for direction, expected in zip(("north_east", "south_east", "south_west", "north_west"), diagonals, strict=True):
        assert getattr(layout, direction)((2, 2)) == coordinate(expected)
    assert sorted(layout.adjacent_coordinates((0, 0))) == sorted(coordinate(c) for c in edge)
    assert sorted(tuple(sorted(pair)) for pair in layout.adjacent_opposite_coordinates((2, 2))) == sorted(
        tuple(sorted((coordinate(first), coordinate(second)))) for first, second in opposites
    )
    assert layout.north_west((0, 0)) is None
    assert layout.south_east((4, 4)) is None
