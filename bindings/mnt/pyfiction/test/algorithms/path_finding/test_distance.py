from mnt.pyfiction import (cartesian_layout, clocked_cartesian_layout, shifted_cartesian_layout, cartesian_gate_layout,
                           clocked_shifted_cartesian_layout, shifted_cartesian_gate_layout, hexagonal_layout,
                           clocked_hexagonal_layout, hexagonal_gate_layout, offset_coordinate, manhattan_distance,
                           euclidean_distance, squared_euclidean_distance, twoddwave_distance, chebyshev_distance)
import unittest


class TestDistance(unittest.TestCase):
    def test_manhattan(self):
        for lyt in [cartesian_layout((4, 4)), clocked_cartesian_layout((4, 4), "2DDWave"),
                    cartesian_gate_layout((4, 4), "2DDWave", "Layout"), shifted_cartesian_layout((4, 4)),
                    clocked_shifted_cartesian_layout((4, 4), "2DDWave"),
                    shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
                    hexagonal_layout((4, 4)), clocked_hexagonal_layout((4, 4), "2DDWave"),
                    hexagonal_gate_layout((4, 4), "2DDWave", "Layout")]:
            self.assertEqual(manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)), 0)
            self.assertEqual(manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0)), 1)
            self.assertEqual(manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1)), 1)
            self.assertEqual(manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1)), 2)
            self.assertEqual(manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2)), 4)
            self.assertEqual(manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3)), 6)
            self.assertEqual(manhattan_distance(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4)), 8)

    def test_euclidean(self):
        for lyt in [cartesian_layout((4, 4)), clocked_cartesian_layout((4, 4), "2DDWave"),
                    cartesian_gate_layout((4, 4), "2DDWave", "Layout"), shifted_cartesian_layout((4, 4)),
                    clocked_shifted_cartesian_layout((4, 4), "2DDWave"),
                    shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"), hexagonal_layout((4, 4)),
                    clocked_hexagonal_layout((4, 4), "2DDWave"), hexagonal_gate_layout((4, 4), "2DDWave", "Layout")]:
            self.assertEqual(euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)), 0)
            self.assertEqual(euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0)), 1)
            self.assertEqual(euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1)), 1)
            self.assertAlmostEqual(euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1)), 2 ** 0.5)
            self.assertAlmostEqual(euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2)),
                                   2 * 2 ** 0.5)
            self.assertAlmostEqual(euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3)),
                                   3 * 2 ** 0.5)
            self.assertAlmostEqual(euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4)),
                                   4 * 2 ** 0.5)

    def test_squared_euclidean(self):
        for lyt in [cartesian_layout((4, 4)), clocked_cartesian_layout((4, 4), "2DDWave"),
                    cartesian_gate_layout((4, 4), "2DDWave", "Layout"), shifted_cartesian_layout((4, 4)),
                    clocked_shifted_cartesian_layout((4, 4), "2DDWave"),
                    shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"), hexagonal_layout((4, 4)),
                    clocked_hexagonal_layout((4, 4), "2DDWave"), hexagonal_gate_layout((4, 4), "2DDWave", "Layout")]:
            self.assertEqual(squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)), 0)
            self.assertEqual(squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0)), 1)
            self.assertEqual(squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1)), 1)
            self.assertEqual(squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1)), 2)
            self.assertEqual(squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2)), 8)
            self.assertEqual(squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3)), 18)
            self.assertEqual(squared_euclidean_distance(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4)), 32)

    def test_twoddwave(self):
        for lyt in [cartesian_layout((4, 4)), clocked_cartesian_layout((4, 4), "2DDWave"),
                    cartesian_gate_layout((4, 4), "2DDWave", "Layout"), shifted_cartesian_layout((4, 4)),
                    clocked_shifted_cartesian_layout((4, 4), "2DDWave"),
                    shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
                    hexagonal_layout((4, 4)), clocked_hexagonal_layout((4, 4), "2DDWave"),
                    hexagonal_gate_layout((4, 4), "2DDWave", "Layout")]:
            self.assertEqual(twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)), 0)
            self.assertEqual(twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0)), 1)
            self.assertEqual(twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1)), 1)
            self.assertEqual(twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1)), 2)
            self.assertEqual(twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2)), 4)
            self.assertEqual(twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3)), 6)
            self.assertEqual(twoddwave_distance(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4)), 8)

    def test_chebyshev(self):
        for lyt in [cartesian_layout((4, 4)), clocked_cartesian_layout((4, 4), "2DDWave"),
                    cartesian_gate_layout((4, 4), "2DDWave", "Layout"), shifted_cartesian_layout((4, 4)),
                    clocked_shifted_cartesian_layout((4, 4), "2DDWave"),
                    shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
                    hexagonal_layout((4, 4)), clocked_hexagonal_layout((4, 4), "2DDWave"),
                    hexagonal_gate_layout((4, 4), "2DDWave", "Layout")]:
            self.assertEqual(chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)), 0)
            self.assertEqual(chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0)), 1)
            self.assertEqual(chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1)), 1)
            self.assertEqual(chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1)), 1)
            self.assertEqual(chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2)), 2)
            self.assertEqual(chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3)), 3)
            self.assertEqual(chebyshev_distance(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4)), 4)


if __name__ == '__main__':
    unittest.main()
