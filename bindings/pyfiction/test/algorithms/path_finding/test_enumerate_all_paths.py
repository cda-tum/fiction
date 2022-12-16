from fiction.pyfiction import *
import unittest


class TestEnumerateAllPaths(unittest.TestCase):
    def test_clocking_paths(self):
        for lyt in [clocked_cartesian_layout((4, 4), "2DDWave"), cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
                    clocked_hexagonal_layout((4, 4), "2DDWave"), hexagonal_gate_layout((4, 4), "2DDWave", "Layout")]:
            self.assertEqual(enumerate_all_clocking_paths(lyt, coordinate(0, 0), coordinate(0, 0)), [[(0, 0)]])
            self.assertEqual(enumerate_all_clocking_paths(lyt, coordinate(0, 0), coordinate(1, 0)), [[(0, 0), (1, 0)]])
            self.assertEqual(enumerate_all_clocking_paths(lyt, coordinate(0, 0), coordinate(0, 1)), [[(0, 0), (0, 1)]])

            paths = enumerate_all_clocking_paths(lyt, coordinate(0, 0), coordinate(1, 1))

            self.assertIn([(0, 0), (0, 1), (1, 1)], paths)
            self.assertIn([(0, 0), (1, 0), (1, 1)], paths)


if __name__ == '__main__':
    unittest.main()
