# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import operator

import pytest

from mnt.pyfiction.layouts import (
    arrangement,
    cartesian_gate_layout,
    cartesian_layout,
    coordinate,
    hexagonal_gate_layout,
    stacked_cartesian_layout,
)


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


@pytest.mark.parametrize("axes", [(-1, -2, 0), (-(2**31) + 1, 2**31 - 1, 5), (0, -5, -1)])
def test_negative_axes_round_trip(axes):
    x, y, z = axes
    for c in (coordinate(x, y, z), coordinate(axes)):
        assert (c.x, c.y, c.z) == axes
        assert c.is_valid()


def test_default_coordinate_is_invalid():
    assert not coordinate().is_valid()
    assert coordinate((0, 0, 0)).is_valid()
    assert coordinate() != coordinate((0, 0, 0))
    assert coordinate() == coordinate()


@pytest.mark.parametrize("axis", [2**31, -(2**31), -(2**31) - 1, 2**40])
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

    stacked = cartesian_layout((2, 2, 3))

    assert stacked.above((0, 0, 0)) == coordinate(0, 0, 1)
    assert stacked.above((0, 0, 3)) == stacked.above((0, 0, 3))
    assert not stacked.above((0, 0, 4)).is_valid()


def test_layouts_reject_negative_extents():
    with pytest.raises(ValueError, match="negative"):
        cartesian_layout((-1, 0))
    with pytest.raises(ValueError, match="negative"):
        cartesian_layout((1, 1)).resize((0, -2))


def test_gate_layouts_reject_extents_beyond_the_signal_range():
    with pytest.raises(IndexError):
        cartesian_gate_layout((2, 2, 2))
    with pytest.raises(IndexError):
        cartesian_gate_layout((2**30, 0))
    with pytest.raises(IndexError):
        hexagonal_gate_layout(arrangement.EVEN_ROW, (2, 2, 2))

    cartesian_gate_layout((2, 2, 1))
    cartesian_gate_layout((2**30 - 1, 0))
