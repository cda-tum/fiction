from fiction.pyfiction import *
import unittest


class TestAStar(unittest.TestCase):
    def test_path_finding(self):
        for lyt in [clocked_cartesian_layout((4, 4), "2DDWave"), cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
                    clocked_hexagonal_layout((4, 4), "2DDWave"), hexagonal_gate_layout((4, 4), "2DDWave", "Layout")]:
            self.assertListEqual(a_star(lyt, coordinate(0, 0), coordinate(0, 0)), [(0, 0)])
            self.assertEqual(len(a_star(lyt, coordinate(0, 0), coordinate(1, 1))), 3)
            self.assertEqual(len(a_star(lyt, coordinate(0, 0), coordinate(2, 2))), 5)
            self.assertEqual(len(a_star(lyt, coordinate(0, 0), coordinate(3, 3))), 7)
            self.assertEqual(len(a_star(lyt, coordinate(0, 0), coordinate(4, 4))), 9)
            self.assertEqual(len(a_star(lyt, coordinate(1, 1), coordinate(0, 0))), 0)
            self.assertEqual(len(a_star(lyt, coordinate(2, 2), coordinate(1, 1))), 0)

    def test_distance(self):
        for lyt in [clocked_cartesian_layout((4, 4), "2DDWave"), cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
                    clocked_hexagonal_layout((4, 4), "2DDWave"), hexagonal_gate_layout((4, 4), "2DDWave", "Layout")]:
            self.assertEqual(a_star_distance(lyt, coordinate(0, 0), coordinate(0, 0)), 0)
            self.assertEqual(a_star_distance(lyt, coordinate(0, 0), coordinate(1, 0)), 1)
            self.assertEqual(a_star_distance(lyt, coordinate(0, 0), coordinate(0, 1)), 1)
            self.assertEqual(a_star_distance(lyt, coordinate(0, 0), coordinate(1, 1)), 2)
            self.assertEqual(a_star_distance(lyt, coordinate(0, 0), coordinate(2, 2)), 4)
            self.assertEqual(a_star_distance(lyt, coordinate(0, 0), coordinate(3, 3)), 6)
            self.assertEqual(a_star_distance(lyt, coordinate(0, 0), coordinate(4, 4)), 8)
            self.assertEqual(a_star_distance(lyt, coordinate(1, 1), coordinate(0, 0)), float('inf'))
            self.assertEqual(a_star_distance(lyt, coordinate(2, 2), coordinate(1, 1)), float('inf'))


if __name__ == '__main__':
    unittest.main()
