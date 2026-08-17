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


def test_manhattan():
    for lyt in [
        cartesian_layout((4, 4)),
        clocked_cartesian_layout((4, 4), "2DDWave"),
        cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
        shifted_cartesian_layout((4, 4)),
        clocked_shifted_cartesian_layout((4, 4), "2DDWave"),
        shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
        hexagonal_layout((4, 4)),
        clocked_hexagonal_layout((4, 4), "2DDWave"),
        hexagonal_gate_layout((4, 4), "2DDWave", "Layout"),
    ]:
        assert manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)) == 0
        assert manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0)) == 1
        assert manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1)) == 1
        assert manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1)) == 2
        assert manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2)) == 4
        assert manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3)) == 6
        assert manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4)) == 8


def test_euclidean():
    for lyt in [
        cartesian_layout((4, 4)),
        clocked_cartesian_layout((4, 4), "2DDWave"),
        cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
        shifted_cartesian_layout((4, 4)),
        clocked_shifted_cartesian_layout((4, 4), "2DDWave"),
        shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
        hexagonal_layout((4, 4)),
        clocked_hexagonal_layout((4, 4), "2DDWave"),
        hexagonal_gate_layout((4, 4), "2DDWave", "Layout"),
    ]:
        assert euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)) == 0
        assert euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0)) == 1
        assert euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1)) == 1
        assert euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1)) == pytest.approx(2**0.5)
        assert euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2)) == pytest.approx(2 * 2**0.5)
        assert euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3)) == pytest.approx(3 * 2**0.5)
        assert euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4)) == pytest.approx(4 * 2**0.5)


def test_squared_euclidean():
    for lyt in [
        cartesian_layout((4, 4)),
        clocked_cartesian_layout((4, 4), "2DDWave"),
        cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
        shifted_cartesian_layout((4, 4)),
        clocked_shifted_cartesian_layout((4, 4), "2DDWave"),
        shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
        hexagonal_layout((4, 4)),
        clocked_hexagonal_layout((4, 4), "2DDWave"),
        hexagonal_gate_layout((4, 4), "2DDWave", "Layout"),
    ]:
        assert squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)) == 0
        assert squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0)) == 1
        assert squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1)) == 1
        assert squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1)) == 2
        assert squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2)) == 8
        assert squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3)) == 18
        assert squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4)) == 32


def test_twoddwave():
    for lyt in [
        cartesian_layout((4, 4)),
        clocked_cartesian_layout((4, 4), "2DDWave"),
        cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
        shifted_cartesian_layout((4, 4)),
        clocked_shifted_cartesian_layout((4, 4), "2DDWave"),
        shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
        hexagonal_layout((4, 4)),
        clocked_hexagonal_layout((4, 4), "2DDWave"),
        hexagonal_gate_layout((4, 4), "2DDWave", "Layout"),
    ]:
        assert twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)) == 0
        assert twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0)) == 1
        assert twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1)) == 1
        assert twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1)) == 2
        assert twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2)) == 4
        assert twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3)) == 6
        assert twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4)) == 8


def test_chebyshev():
    for lyt in [
        cartesian_layout((4, 4)),
        clocked_cartesian_layout((4, 4), "2DDWave"),
        cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
        shifted_cartesian_layout((4, 4)),
        clocked_shifted_cartesian_layout((4, 4), "2DDWave"),
        shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
        hexagonal_layout((4, 4)),
        clocked_hexagonal_layout((4, 4), "2DDWave"),
        hexagonal_gate_layout((4, 4), "2DDWave", "Layout"),
    ]:
        assert chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)) == 0
        assert chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0)) == 1
        assert chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1)) == 1
        assert chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1)) == 1
        assert chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2)) == 2
        assert chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3)) == 3
        assert chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4)) == 4
