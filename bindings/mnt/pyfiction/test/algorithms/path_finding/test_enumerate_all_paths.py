from mnt.pyfiction import (cartesian_layout, shifted_cartesian_layout, hexagonal_layout, enumerate_all_paths,
                           offset_coordinate, clocked_cartesian_layout, cartesian_gate_layout,
                           clocked_shifted_cartesian_layout, shifted_cartesian_gate_layout, clocked_hexagonal_layout,
                           hexagonal_gate_layout)
import unittest


class TestEnumerateAllPaths(unittest.TestCase):
    def test_non_clocked_paths(self):
        for lyt in [cartesian_layout((4, 4)), shifted_cartesian_layout((4, 4)), hexagonal_layout((4, 4))]:
            self.assertListEqual(enumerate_all_paths(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)), [[(0, 0)]])

    def test_clocking_paths(self):
        for lyt in [clocked_cartesian_layout((4, 4), "2DDWave"), cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
                    clocked_shifted_cartesian_layout((4, 4), "2DDWave"),
                    shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
                    clocked_hexagonal_layout((4, 4), "2DDWave"), hexagonal_gate_layout((4, 4), "2DDWave", "Layout")]:
            self.assertEqual(enumerate_all_paths(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)), [[(0, 0)]])
            self.assertEqual(enumerate_all_paths(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0)),
                             [[(0, 0), (1, 0)]])
            self.assertEqual(enumerate_all_paths(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1)),
                             [[(0, 0), (0, 1)]])

            paths = enumerate_all_paths(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1))

            self.assertIn([(0, 0), (0, 1), (1, 1)], paths)
            self.assertIn([(0, 0), (1, 0), (1, 1)], paths)


if __name__ == '__main__':
    unittest.main()
