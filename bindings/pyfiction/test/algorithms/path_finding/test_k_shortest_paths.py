from fiction.pyfiction import *
import unittest


class TestKShortestPaths(unittest.TestCase):
    def test_yen(self):
        for lyt in [clocked_cartesian_layout((4, 4), "2DDWave"), cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
                    clocked_shifted_cartesian_layout((4, 4), "2DDWave"),
                    shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
                    clocked_hexagonal_layout((4, 4), "2DDWave"), hexagonal_gate_layout((4, 4), "2DDWave", "Layout")]:
            self.assertEqual(yen_k_shortest_paths(lyt, coordinate(0, 0), coordinate(0, 0), 1), [[(0, 0)]])
            self.assertEqual(yen_k_shortest_paths(lyt, coordinate(0, 0), coordinate(1, 0), 1), [[(0, 0), (1, 0)]])
            self.assertEqual(yen_k_shortest_paths(lyt, coordinate(0, 0), coordinate(0, 1), 1), [[(0, 0), (0, 1)]])

            paths = yen_k_shortest_paths(lyt, coordinate(0, 0), coordinate(1, 1), 2)

            self.assertIn([(0, 0), (0, 1), (1, 1)], paths)
            self.assertIn([(0, 0), (1, 0), (1, 1)], paths)


if __name__ == '__main__':
    unittest.main()
