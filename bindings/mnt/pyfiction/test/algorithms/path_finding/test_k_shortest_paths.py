import unittest

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


class TestKShortestPaths(unittest.TestCase):
    def test_non_clocked_yen_paths(self):
        for lyt in [cartesian_layout((4, 4)), shifted_cartesian_layout((4, 4)), hexagonal_layout((4, 4))]:
            self.assertListEqual(
                yen_k_shortest_paths(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0), 1), [[(0, 0)]]
            )

    def test_clocked_yen_paths(self):
        for lyt in [
            clocked_cartesian_layout((4, 4), "2DDWave"),
            cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
            clocked_shifted_cartesian_layout((4, 4), "2DDWave"),
            shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
            clocked_hexagonal_layout((4, 4), "2DDWave"),
            hexagonal_gate_layout((4, 4), "2DDWave", "Layout"),
        ]:
            self.assertEqual(yen_k_shortest_paths(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0), 1), [[(0, 0)]])
            self.assertEqual(
                yen_k_shortest_paths(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0), 1), [[(0, 0), (1, 0)]]
            )
            self.assertEqual(
                yen_k_shortest_paths(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1), 1), [[(0, 0), (0, 1)]]
            )

            paths = yen_k_shortest_paths(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1), 2)

            self.assertIn([(0, 0), (0, 1), (1, 1)], paths)
            self.assertIn([(0, 0), (1, 0), (1, 1)], paths)


if __name__ == "__main__":
    unittest.main()
