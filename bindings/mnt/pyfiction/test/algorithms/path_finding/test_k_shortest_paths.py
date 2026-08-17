from mnt.pyfiction import (
    cartesian_gate_layout,
    cartesian_layout,
    clocked_cartesian_layout,
    clocked_hexagonal_layout,
    clocked_shifted_cartesian_layout,
    hexagonal_gate_layout,
    hexagonal_layout,
    offset_coordinate,
    shifted_cartesian_gate_layout,
    shifted_cartesian_layout,
    yen_k_shortest_paths,
)


def test_non_clocked_yen_paths():
    for lyt in [cartesian_layout((4, 4)), shifted_cartesian_layout((4, 4)), hexagonal_layout((4, 4))]:
        assert yen_k_shortest_paths(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0), 1) == [[(0, 0)]]


def test_clocked_yen_paths():
    for lyt in [
        clocked_cartesian_layout((4, 4), "2DDWave"),
        cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
        clocked_shifted_cartesian_layout((4, 4), "2DDWave"),
        shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
        clocked_hexagonal_layout((4, 4), "2DDWave"),
        hexagonal_gate_layout((4, 4), "2DDWave", "Layout"),
    ]:
        assert yen_k_shortest_paths(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0), 1) == [[(0, 0)]]
        assert yen_k_shortest_paths(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0), 1) == [[(0, 0), (1, 0)]]
        assert yen_k_shortest_paths(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1), 1) == [[(0, 0), (0, 1)]]

        paths = yen_k_shortest_paths(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1), 2)

        assert [(0, 0), (0, 1), (1, 1)] in paths
        assert [(0, 0), (1, 0), (1, 1)] in paths
