from __future__ import annotations

import pytest

from mnt.pyfiction import (
    cartesian_gate_layout,
    cartesian_layout,
    chebyshev_distance,
    clocked_cartesian_layout,
    clocked_hexagonal_layout,
    clocked_shifted_cartesian_layout,
    euclidean_distance,
    hexagonal_gate_layout,
    hexagonal_layout,
    manhattan_distance,
    offset_coordinate,
    shifted_cartesian_gate_layout,
    shifted_cartesian_layout,
    squared_euclidean_distance,
    twoddwave_distance,
)

ALL_LAYOUTS = [
    pytest.param(lambda: cartesian_layout((4, 4)), id="cartesian_layout"),
    pytest.param(lambda: clocked_cartesian_layout((4, 4), "2DDWave"), id="clocked_cartesian_layout"),
    pytest.param(lambda: cartesian_gate_layout((4, 4), "2DDWave", "Layout"), id="cartesian_gate_layout"),
    pytest.param(lambda: shifted_cartesian_layout((4, 4)), id="shifted_cartesian_layout"),
    pytest.param(lambda: clocked_shifted_cartesian_layout((4, 4), "2DDWave"), id="clocked_shifted_cartesian_layout"),
    pytest.param(
        lambda: shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"), id="shifted_cartesian_gate_layout"
    ),
    pytest.param(lambda: hexagonal_layout((4, 4)), id="hexagonal_layout"),
    pytest.param(lambda: clocked_hexagonal_layout((4, 4), "2DDWave"), id="clocked_hexagonal_layout"),
    pytest.param(lambda: hexagonal_gate_layout((4, 4), "2DDWave", "Layout"), id="hexagonal_gate_layout"),
]


@pytest.mark.parametrize("make_lyt", ALL_LAYOUTS)
def test_manhattan(make_lyt):
    lyt = make_lyt()
    assert manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)) == 0
    assert manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0)) == 1
    assert manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1)) == 1
    assert manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1)) == 2
    assert manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2)) == 4
    assert manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3)) == 6
    assert manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4)) == 8


@pytest.mark.parametrize("make_lyt", ALL_LAYOUTS)
def test_euclidean(make_lyt):
    lyt = make_lyt()
    assert euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)) == 0
    assert euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0)) == 1
    assert euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1)) == 1
    assert euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1)) == pytest.approx(2**0.5, abs=1e-7)
    assert euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2)) == pytest.approx(
        2 * 2**0.5, abs=1e-7
    )
    assert euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3)) == pytest.approx(
        3 * 2**0.5, abs=1e-7
    )
    assert euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4)) == pytest.approx(
        4 * 2**0.5, abs=1e-7
    )


@pytest.mark.parametrize("make_lyt", ALL_LAYOUTS)
def test_squared_euclidean(make_lyt):
    lyt = make_lyt()
    assert squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)) == 0
    assert squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0)) == 1
    assert squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1)) == 1
    assert squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1)) == 2
    assert squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2)) == 8
    assert squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3)) == 18
    assert squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4)) == 32


@pytest.mark.parametrize("make_lyt", ALL_LAYOUTS)
def test_twoddwave(make_lyt):
    lyt = make_lyt()
    assert twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)) == 0
    assert twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0)) == 1
    assert twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1)) == 1
    assert twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1)) == 2
    assert twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2)) == 4
    assert twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3)) == 6
    assert twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4)) == 8


@pytest.mark.parametrize("make_lyt", ALL_LAYOUTS)
def test_chebyshev(make_lyt):
    lyt = make_lyt()
    assert chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)) == 0
    assert chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0)) == 1
    assert chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1)) == 1
    assert chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1)) == 1
    assert chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2)) == 2
    assert chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3)) == 3
    assert chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4)) == 4
