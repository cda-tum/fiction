import pytest

from mnt.pyfiction import (
    a_star,
    a_star_distance,
    a_star_params,
    cartesian_gate_layout,
    cartesian_layout,
    cartesian_obstruction_layout,
    clocked_cartesian_layout,
    clocked_hexagonal_layout,
    clocked_shifted_cartesian_layout,
    hexagonal_gate_layout,
    hexagonal_layout,
    hexagonal_obstruction_layout,
    offset_coordinate,
    shifted_cartesian_gate_layout,
    shifted_cartesian_layout,
    shifted_cartesian_obstruction_layout,
)

CLOCKED_LAYOUTS = [
    pytest.param(lambda: clocked_cartesian_layout((4, 4), "2DDWave"), id="clocked_cartesian_layout"),
    pytest.param(lambda: cartesian_gate_layout((4, 4), "2DDWave", "Layout"), id="cartesian_gate_layout"),
    pytest.param(lambda: clocked_shifted_cartesian_layout((4, 4), "2DDWave"), id="clocked_shifted_cartesian_layout"),
    pytest.param(
        lambda: shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"), id="shifted_cartesian_gate_layout"
    ),
    pytest.param(lambda: clocked_hexagonal_layout((4, 4), "2DDWave"), id="clocked_hexagonal_layout"),
    pytest.param(lambda: hexagonal_gate_layout((4, 4), "2DDWave", "Layout"), id="hexagonal_gate_layout"),
]


@pytest.mark.parametrize(
    "make_lyt",
    [
        pytest.param(lambda: cartesian_layout((4, 4)), id="cartesian_layout"),
        pytest.param(lambda: shifted_cartesian_layout((4, 4)), id="shifted_cartesian_layout"),
        pytest.param(lambda: hexagonal_layout((4, 4)), id="hexagonal_layout"),
    ],
)
def test_non_clocked_path_finding(make_lyt):
    lyt = make_lyt()
    assert a_star(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)) == [(0, 0)]


@pytest.mark.parametrize("make_lyt", CLOCKED_LAYOUTS)
def test_clocked_path_finding(make_lyt):
    lyt = make_lyt()
    assert a_star(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)) == [(0, 0)]
    assert len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1))) == 3
    assert len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2))) == 5
    assert len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3))) == 7
    assert len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4))) == 9
    assert len(a_star(lyt, offset_coordinate(1, 1), offset_coordinate(0, 0))) == 0
    assert len(a_star(lyt, offset_coordinate(2, 2), offset_coordinate(1, 1))) == 0


@pytest.mark.parametrize(
    "make_lyt",
    [
        pytest.param(
            lambda: cartesian_obstruction_layout(cartesian_gate_layout((4, 4), "2DDWave", "Layout")),
            id="cartesian_obstruction_layout",
        ),
        pytest.param(
            lambda: shifted_cartesian_obstruction_layout(shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout")),
            id="shifted_cartesian_obstruction_layout",
        ),
        pytest.param(
            lambda: hexagonal_obstruction_layout(hexagonal_gate_layout((4, 4), "2DDWave", "Layout")),
            id="hexagonal_obstruction_layout",
        ),
    ],
)
def test_path_finding_with_obstructions(make_lyt):
    lyt = make_lyt()
    lyt.obstruct_coordinate(offset_coordinate(1, 0))
    lyt.obstruct_coordinate(offset_coordinate(1, 1))
    lyt.obstruct_coordinate(offset_coordinate(1, 2))
    lyt.obstruct_coordinate(offset_coordinate(1, 3))
    lyt.obstruct_coordinate(offset_coordinate(1, 4))

    assert lyt.is_obstructed_coordinate(offset_coordinate(1, 0))
    assert lyt.is_obstructed_coordinate(offset_coordinate(1, 1))
    assert lyt.is_obstructed_coordinate(offset_coordinate(1, 2))
    assert lyt.is_obstructed_coordinate(offset_coordinate(1, 3))
    assert lyt.is_obstructed_coordinate(offset_coordinate(1, 4))

    assert a_star(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)) == [(0, 0)]
    assert len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1))) == 3
    assert len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2))) == 0
    assert len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3))) == 0
    assert len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4))) == 0
    assert len(a_star(lyt, offset_coordinate(1, 1), offset_coordinate(0, 0))) == 0
    assert len(a_star(lyt, offset_coordinate(2, 2), offset_coordinate(1, 1))) == 0


@pytest.mark.parametrize(
    "make_lyt",
    [
        pytest.param(
            lambda: cartesian_obstruction_layout(cartesian_gate_layout((2, 1, 1), "2DDWave", "Layout")),
            id="cartesian_obstruction_layout",
        ),
        pytest.param(
            lambda: shifted_cartesian_obstruction_layout(shifted_cartesian_gate_layout((2, 1, 1), "2DDWave", "Layout")),
            id="shifted_cartesian_obstruction_layout",
        ),
        pytest.param(
            lambda: hexagonal_obstruction_layout(hexagonal_gate_layout((2, 1, 1), "2DDWave", "Layout")),
            id="hexagonal_obstruction_layout",
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

    params = a_star_params()
    params.crossings = True

    assert len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0), params)) == 1
    assert len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0), params)) == 0
    assert len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1), params)) == 2
    assert len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1), params)) == 0
    assert len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(2, 0), params)) == 0
    assert len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(2, 1), params)) == 0

    assert len(a_star(lyt, offset_coordinate(1, 0), offset_coordinate(0, 0), params)) == 0
    assert len(a_star(lyt, offset_coordinate(1, 0), offset_coordinate(1, 0), params)) == 1
    assert len(a_star(lyt, offset_coordinate(1, 0), offset_coordinate(0, 1), params)) == 0
    assert len(a_star(lyt, offset_coordinate(1, 0), offset_coordinate(1, 1), params)) == 0
    assert len(a_star(lyt, offset_coordinate(1, 0), offset_coordinate(2, 0), params)) == 2
    assert len(a_star(lyt, offset_coordinate(1, 0), offset_coordinate(2, 1), params)) == 3

    assert len(a_star(lyt, offset_coordinate(0, 1), offset_coordinate(0, 0), params)) == 0
    assert len(a_star(lyt, offset_coordinate(0, 1), offset_coordinate(1, 0), params)) == 0
    assert len(a_star(lyt, offset_coordinate(0, 1), offset_coordinate(0, 1), params)) == 1
    assert len(a_star(lyt, offset_coordinate(0, 1), offset_coordinate(1, 1), params)) == 0
    assert len(a_star(lyt, offset_coordinate(0, 1), offset_coordinate(2, 0), params)) == 0
    assert len(a_star(lyt, offset_coordinate(0, 1), offset_coordinate(2, 1), params)) == 0

    assert len(a_star(lyt, offset_coordinate(1, 1), offset_coordinate(0, 0), params)) == 0
    assert len(a_star(lyt, offset_coordinate(1, 1), offset_coordinate(1, 0), params)) == 0
    assert len(a_star(lyt, offset_coordinate(1, 1), offset_coordinate(0, 1), params)) == 0
    assert len(a_star(lyt, offset_coordinate(1, 1), offset_coordinate(1, 1), params)) == 1
    assert len(a_star(lyt, offset_coordinate(1, 1), offset_coordinate(2, 0), params)) == 0
    assert len(a_star(lyt, offset_coordinate(1, 1), offset_coordinate(2, 1), params)) == 2

    assert len(a_star(lyt, offset_coordinate(2, 0), offset_coordinate(0, 0), params)) == 0
    assert len(a_star(lyt, offset_coordinate(2, 0), offset_coordinate(1, 0), params)) == 0
    assert len(a_star(lyt, offset_coordinate(2, 0), offset_coordinate(0, 1), params)) == 0
    assert len(a_star(lyt, offset_coordinate(2, 0), offset_coordinate(1, 1), params)) == 0
    assert len(a_star(lyt, offset_coordinate(2, 0), offset_coordinate(2, 0), params)) == 1
    assert len(a_star(lyt, offset_coordinate(2, 0), offset_coordinate(2, 1), params)) == 2

    assert len(a_star(lyt, offset_coordinate(2, 1), offset_coordinate(0, 0), params)) == 0
    assert len(a_star(lyt, offset_coordinate(2, 1), offset_coordinate(1, 0), params)) == 0
    assert len(a_star(lyt, offset_coordinate(2, 1), offset_coordinate(0, 1), params)) == 0
    assert len(a_star(lyt, offset_coordinate(2, 1), offset_coordinate(1, 1), params)) == 0
    assert len(a_star(lyt, offset_coordinate(2, 1), offset_coordinate(2, 0), params)) == 0
    assert len(a_star(lyt, offset_coordinate(2, 1), offset_coordinate(2, 1), params)) == 1


@pytest.mark.parametrize("make_lyt", CLOCKED_LAYOUTS)
def test_distance(make_lyt):
    lyt = make_lyt()
    assert a_star_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)) == 0
    assert a_star_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0)) == 1
    assert a_star_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1)) == 1
    assert a_star_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1)) == 2
    assert a_star_distance(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2)) == 4
    assert a_star_distance(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3)) == 6
    assert a_star_distance(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4)) == 8
    assert a_star_distance(lyt, offset_coordinate(1, 1), offset_coordinate(0, 0)) == float("inf")
    assert a_star_distance(lyt, offset_coordinate(2, 2), offset_coordinate(1, 1)) == float("inf")
