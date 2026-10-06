# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import copy
import operator
from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.layouts import (
    Extent,
    area,
    arrangement,
    cartesian_layout,
    coordinate,
    hexagonal_layout,
    shifted_cartesian_layout,
    stacked_cartesian_layout,
    volume,
)

if TYPE_CHECKING:
    from collections.abc import Callable


def test_construction_from_a_tuple():
    coordinate((1, 0))
    coordinate((1, 0, 0))


@pytest.mark.parametrize("dimensions", [(0,), (0, 0, 1, 1), (0, 0, 1, 1, 3)])
def test_construction_from_a_tuple_of_the_wrong_length(dimensions):
    with pytest.raises(RuntimeError):
        coordinate(dimensions)


def test_dimension_access():
    t = coordinate(1, 2, 0)

    assert t.x == 1
    assert t.y == 2
    assert t.z == 0


def test_axes_are_writable():
    t = coordinate(1, 2, 0)
    t.x, t.y, t.z = -3, -4, 1

    assert t == coordinate(-3, -4, 1)


@pytest.mark.parametrize("axes", [(-1, -2, 0), (-(2**31), 2**31 - 1, 5), (0, -5, -1)])
def test_negative_axes_round_trip(axes):
    x, y, z = axes
    for c in (coordinate(x, y, z), coordinate(axes)):
        assert (c.x, c.y, c.z) == axes


@pytest.mark.parametrize("axis", [2**31, -(2**31) - 1, 2**40])
def test_axes_outside_of_the_int32_range_raise(axis):
    with pytest.raises(OverflowError):
        coordinate(axis, 0, 0)
    with pytest.raises(OverflowError):
        coordinate((0, axis))
    with pytest.raises(OverflowError):
        coordinate(0, 0, 0).x = axis


@pytest.mark.parametrize(
    ("compare", "ascending"),
    [
        pytest.param(operator.lt, True, id="lt"),
        pytest.param(operator.le, True, id="le"),
        pytest.param(operator.gt, False, id="gt"),
        pytest.param(operator.ge, False, id="ge"),
    ],
)
def test_ordering(compare, ascending):
    smaller, larger = coordinate(0, 0, 0), coordinate(1, 2, 0)
    operands = (smaller, larger) if ascending else (larger, smaller)

    assert compare(*operands)


def test_ordering_with_negative_axes():
    assert coordinate(-2, 0, 0) < coordinate(-1, 0, 0)
    assert coordinate(5, -1, 0) < coordinate(-5, 0, 0)
    assert coordinate(5, 5, -1) < coordinate(-5, -5, 0)


def test_equality_disregards_an_omitted_z():
    with_z, without_z = coordinate(1, 2, 0), coordinate(1, 2)

    assert with_z == without_z
    assert without_z == with_z
    assert hash(with_z) == hash(without_z)


def test_repr():
    assert repr(coordinate(3, 2, 1)) == "(3,2,1)"
    assert repr(coordinate(-3, 2, 1)) == "(-3,2,1)"


def test_stacked_cartesian_layout_is_an_alias_of_cartesian_layout():
    assert stacked_cartesian_layout is cartesian_layout

    stacked = cartesian_layout((3, 3, 4))

    assert stacked.above((0, 0, 0)) == coordinate(0, 0, 1)
    assert stacked.above((0, 0, 3)) is None
    assert stacked.above((0, 0, 4)) is None


def test_layouts_reject_negative_extents():
    with pytest.raises((ValueError, TypeError)):
        cartesian_layout((-1, 0))
    with pytest.raises((ValueError, TypeError)):
        cartesian_layout((1, 1)).resize((0, -2))


@pytest.mark.parametrize("axis", [2**31, 2**40])
@pytest.mark.parametrize(
    "make_layout",
    [
        pytest.param(lambda: cartesian_layout((2, 2)), id="cartesian"),
        pytest.param(lambda: hexagonal_layout(arrangement.EVEN_ROW, (2, 2)), id="hexagonal"),
        pytest.param(lambda: shifted_cartesian_layout(arrangement.EVEN_ROW, (2, 2)), id="shifted_cartesian"),
    ],
)
def test_layout_coord_rejects_axes_outside_of_the_int32_range(make_layout, axis):
    lyt = make_layout()

    with pytest.raises(OverflowError):
        lyt.coord(axis, 5)
    with pytest.raises(OverflowError):
        lyt.coord(5, axis, 0)
    with pytest.raises(OverflowError):
        lyt.coord(5, 5, axis)

    assert lyt.coord(1, 2) == coordinate(1, 2, 0)


def test_tuples_outside_of_the_int32_range_do_not_convert_to_coordinates():
    lyt = cartesian_layout((2, 2))

    # nanobind drops the OverflowError of the implicit tuple conversion and reports a signature mismatch
    with pytest.raises((TypeError, OverflowError)):
        lyt.north((2**31, 0))


def test_default_coordinate_is_origin() -> None:
    assert coordinate() == coordinate((0, 0, 0))
    c = coordinate()
    c.x = -(2**31)
    assert c.x == -(2**31)


def test_default_geometry_is_empty() -> None:
    layout = cartesian_layout()
    assert layout.coordinates() == []
    assert layout.last_coordinate() is None
    assert layout.north((0, 0)) is None


def test_extent_tuple_conversion_and_checked_axes() -> None:
    size = Extent(2, 3)
    assert (size.width, size.height, size.layers) == (2, 3, 1)
    assert cartesian_layout(size).dimensions() == Extent((2, 3, 1))
    assert cartesian_layout((2, 3, 0)).coordinates() == []
    assert cartesian_layout((2**31, 1)).last_coordinate() == coordinate(2**31 - 1, 0, 0)
    with pytest.raises(ValueError, match=r"negative|coordinate domain"):
        size.width = 2**31 + 1
    assert size.width == 2


@pytest.mark.parametrize(
    "make_layout",
    [
        pytest.param(lambda: cartesian_layout((2, 3)), id="cartesian"),
        pytest.param(lambda: hexagonal_layout(arrangement.EVEN_ROW, (2, 3)), id="hexagonal"),
        pytest.param(lambda: shifted_cartesian_layout(arrangement.EVEN_ROW, (2, 3)), id="shifted_cartesian"),
    ],
)
def test_geometry_copies_have_independent_sizes(
    make_layout: Callable[[], cartesian_layout | hexagonal_layout | shifted_cartesian_layout],
) -> None:
    layout = make_layout()
    duplicate = copy.copy(layout)
    duplicate.resize((4, 5))
    assert layout.width() == 2
    assert duplicate.width() == 4
    deep = copy.deepcopy(layout)
    deep.resize((6, 7))
    assert layout.height() == 3
    assert deep.height() == 7


@pytest.mark.parametrize("value", [-1, 2**31 + 1])
def test_extent_rejects_sizes_outside_the_coordinate_domain(value: int) -> None:
    with pytest.raises(ValueError, match=r"negative|coordinate domain"):
        Extent(value, 1)
    with pytest.raises(ValueError, match=r"negative|coordinate domain"):
        Extent((1, value, 1))
    size = Extent(2, 3)
    with pytest.raises(ValueError, match=r"negative|coordinate domain"):
        size.layers = value
    assert size.layers == 1


def test_sizes_and_coordinates_have_distinct_meanings() -> None:
    assert area((2, 3)) == 6
    assert volume((2, 3)) == 6
    assert volume((2, 3, 0)) == 0
    with pytest.raises(TypeError):
        cartesian_layout(coordinate(2, 3))  # ty: ignore[invalid-argument-type]  # deliberately a coordinate
    with pytest.raises(OverflowError):
        volume(Extent(2**31, 2**31, 4))
    layout = cartesian_layout((2, 3))
    size = layout.dimensions()
    size.width = 4
    assert layout.width() == 2


@pytest.mark.parametrize(
    "make_layout",
    [
        pytest.param(lambda: cartesian_layout((2**31, 2**31, 0)), id="cartesian"),
        pytest.param(lambda: hexagonal_layout(arrangement.EVEN_ROW, (2**31, 2**31, 0)), id="hexagonal"),
        pytest.param(lambda: shifted_cartesian_layout(arrangement.EVEN_ROW, (2**31, 2**31, 0)), id="shifted_cartesian"),
    ],
)
def test_zero_layers_skip_coordinate_allocation(
    make_layout: Callable[[], cartesian_layout | hexagonal_layout | shifted_cartesian_layout],
) -> None:
    layout = make_layout()
    assert layout.coordinates() == []
    assert layout.ground_coordinates() == []
