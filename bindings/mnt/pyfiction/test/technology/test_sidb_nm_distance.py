from mnt.pyfiction import *
import unittest
import os


class TestSiDBNmDistance(unittest.TestCase):
    def test_sidb_nm_distance_100_lattice(self):
        layout_one = sidb_100_lattice((10, 10))

        # Replace the placeholders with actual values or function calls
        self.assertEqual(sidb_nm_distance_100(layout_one, (0, 0), (0, 0)), 0)
        self.assertEqual(sidb_nm_distance_100(layout_one, (1, 0), (1, 0)), 0)
        self.assertEqual(sidb_nm_distance_100(layout_one, (1, 1), (1, 1)), 0)

        # Replace the placeholders with actual values or function calls
        self.assertAlmostEqual(sidb_nm_distance_100(layout_one, (0, 0), (2, 0)), 2 * 0.384)
        self.assertAlmostEqual(sidb_nm_distance_100(layout_one, (0, 0), (0, 1)), 0.225)

    def test_sidb_nm_distance_100_lattice(self):
        layout_one = sidb_111_lattice((10, 10))

        # Replace the placeholders with actual values or function calls
        self.assertEqual(sidb_nm_distance_111(layout_one, (0, 0), (0, 0)), 0)
        self.assertEqual(sidb_nm_distance_111(layout_one, (1, 0), (1, 0)), 0)
        self.assertEqual(sidb_nm_distance_111(layout_one, (1, 1), (1, 1)), 0)

        # Replace the placeholders with actual values or function calls
        self.assertAlmostEqual(sidb_nm_distance_111(layout_one, (0, 0), (2, 0)), 1.33)
        self.assertAlmostEqual(sidb_nm_distance_111(layout_one, (0, 0), (0, 1)), 0.3839967)


if __name__ == '__main__':
    unittest.main()
