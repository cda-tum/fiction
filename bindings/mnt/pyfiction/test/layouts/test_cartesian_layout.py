import unittest

from mnt.pyfiction import cartesian_layout


class TestCartesianLayout(unittest.TestCase):
    def test_coordinate_iteration(self):
        for layout in [
            cartesian_layout((9, 9, 1)),
            cartesian_layout((9, 9, 1), coordinate_type="cube"),
        ]:
            for t in layout.coordinates():
                self.assertLessEqual(t, (9, 9, 1))
                self.assertTrue(layout.is_within_bounds(t))

            for t in layout.ground_coordinates():
                self.assertEqual(t.z, 0)
                self.assertLessEqual(t, (9, 9, 0))
                self.assertTrue(layout.is_within_bounds(t))

            for t in layout.adjacent_coordinates((2, 2)):
                self.assertIn(t, [(1, 2), (2, 1), (3, 2), (2, 3)])

    def test_cube_coordinates(self):
        layout = cartesian_layout((9, 9, 1), coordinate_type="cube")
        for t in layout.adjacent_coordinates((0, 0)):
            self.assertIn(t, [(-1, 0), (0, 1), (1, 0), (0, -1)])


if __name__ == "__main__":
    unittest.main()
