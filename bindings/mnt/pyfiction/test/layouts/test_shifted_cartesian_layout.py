import unittest

from mnt.pyfiction import shifted_cartesian_layout


class TestShiftedCartesianLayout(unittest.TestCase):
    def test_coordinate_iteration(self):
        for layout in [
            shifted_cartesian_layout((9, 9, 1)),
            shifted_cartesian_layout((9, 9, 1), coordinate_type="cube"),
        ]:
            for t in layout.coordinates():
                self.assertLessEqual(t, (9, 9, 1))
                self.assertTrue(layout.is_within_bounds(t))

            for t in layout.ground_coordinates():
                self.assertEqual(t.z, 0)
                self.assertLessEqual(t, (9, 9, 0))
                self.assertTrue(layout.is_within_bounds(t))

            for t in layout.adjacent_coordinates((2, 2)):
                self.assertIn(t, [(1, 1), (1, 2), (2, 1), (3, 1), (3, 2), (2, 3)])


if __name__ == "__main__":
    unittest.main()
