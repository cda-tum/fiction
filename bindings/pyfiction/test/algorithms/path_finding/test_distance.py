from fiction.pyfiction import *
import unittest


class TestDistance(unittest.TestCase):
    def test_manhattan(self):
        for lyt in [cartesian_layout((4, 4)), clocked_cartesian_layout((4, 4), "2DDWave"),
                    cartesian_gate_layout((4, 4), "2DDWave", "Layout"), shifted_cartesian_layout((4, 4)),
                    clocked_shifted_cartesian_layout((4, 4), "2DDWave"),
                    shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
                    hexagonal_layout((4, 4)), clocked_hexagonal_layout((4, 4), "2DDWave"),
                    hexagonal_gate_layout((4, 4), "2DDWave", "Layout")]:
            self.assertEqual(manhattan_distance(lyt, coordinate(0, 0), coordinate(0, 0)), 0)
            self.assertEqual(manhattan_distance(lyt, coordinate(0, 0), coordinate(1, 0)), 1)
            self.assertEqual(manhattan_distance(lyt, coordinate(0, 0), coordinate(0, 1)), 1)
            self.assertEqual(manhattan_distance(lyt, coordinate(0, 0), coordinate(1, 1)), 2)
            self.assertEqual(manhattan_distance(lyt, coordinate(0, 0), coordinate(2, 2)), 4)
            self.assertEqual(manhattan_distance(lyt, coordinate(0, 0), coordinate(3, 3)), 6)
            self.assertEqual(manhattan_distance(lyt, coordinate(0, 0), coordinate(4, 4)), 8)

    def test_euclidean(self):
        for lyt in [cartesian_layout((4, 4)), clocked_cartesian_layout((4, 4), "2DDWave"),
                    cartesian_gate_layout((4, 4), "2DDWave", "Layout"), shifted_cartesian_layout((4, 4)),
                    clocked_shifted_cartesian_layout((4, 4), "2DDWave"),
                    shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"), hexagonal_layout((4, 4)),
                    clocked_hexagonal_layout((4, 4), "2DDWave"), hexagonal_gate_layout((4, 4), "2DDWave", "Layout")]:
            self.assertEqual(euclidean_distance(lyt, coordinate(0, 0), coordinate(0, 0)), 0)
            self.assertEqual(euclidean_distance(lyt, coordinate(0, 0), coordinate(1, 0)), 1)
            self.assertEqual(euclidean_distance(lyt, coordinate(0, 0), coordinate(0, 1)), 1)
            self.assertAlmostEqual(euclidean_distance(lyt, coordinate(0, 0), coordinate(1, 1)), 2 ** 0.5)
            self.assertAlmostEqual(euclidean_distance(lyt, coordinate(0, 0), coordinate(2, 2)), 2 * 2 ** 0.5)
            self.assertAlmostEqual(euclidean_distance(lyt, coordinate(0, 0), coordinate(3, 3)), 3 * 2 ** 0.5)
            self.assertAlmostEqual(euclidean_distance(lyt, coordinate(0, 0), coordinate(4, 4)), 4 * 2 ** 0.5)


if __name__ == '__main__':
    unittest.main()
