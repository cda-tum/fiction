from __future__ import annotations

import operator

import pytest

from mnt.pyfiction import cube_coordinate, offset_coordinate, siqad_coordinate

COORDINATE_TYPES = [
    pytest.param(offset_coordinate, id="offset"),
    pytest.param(cube_coordinate, id="cube"),
    pytest.param(siqad_coordinate, id="siqad"),
]


@pytest.mark.parametrize("coordinate", COORDINATE_TYPES)
def test_construction_from_a_tuple(coordinate):
    coordinate((1, 0))
    coordinate((1, 0, 0))


@pytest.mark.parametrize("coordinate", COORDINATE_TYPES)
@pytest.mark.parametrize("dimensions", [(0,), (0, 0, 1, 1), (0, 0, 1, 1, 3)])
def test_construction_from_a_tuple_of_the_wrong_length(coordinate, dimensions):
    with pytest.raises(RuntimeError):
        coordinate(dimensions)


@pytest.mark.parametrize("coordinate", COORDINATE_TYPES)
def test_dimension_access(coordinate):
    t = coordinate(1, 2, 0)

    assert t.x == 1
    assert t.y == 2
    assert t.z == 0


@pytest.mark.parametrize("coordinate", COORDINATE_TYPES)
@pytest.mark.parametrize(
    ("compare", "ascending"),
    [
        pytest.param(operator.lt, True, id="lt"),
        pytest.param(operator.le, True, id="le"),
        pytest.param(operator.gt, False, id="gt"),
        pytest.param(operator.ge, False, id="ge"),
    ],
)
def test_ordering(coordinate, compare, ascending):
    smaller, larger = coordinate(0, 0, 0), coordinate(1, 2, 0)
    operands = (smaller, larger) if ascending else (larger, smaller)

    assert compare(*operands)


@pytest.mark.parametrize("coordinate", COORDINATE_TYPES)
def test_equality_disregards_an_omitted_z(coordinate):
    with_z, without_z = coordinate(1, 2, 0), coordinate(1, 2)

    assert with_z == without_z
    assert without_z == with_z


@pytest.mark.parametrize("coordinate", COORDINATE_TYPES)
def test_repr(coordinate):
    assert repr(coordinate(3, 2, 1)) == "(3,2,1)"
