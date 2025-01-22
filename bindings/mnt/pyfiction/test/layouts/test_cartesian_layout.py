import unittest

from mnt.pyfiction import cartesian_layout, cartesian_layout_cube_coordinates, cartesian_layout_siqad_coordinates


class TestCartesianLayout(unittest.TestCase):
    def test_coordinate_iteration(self):
        for layout in [
            cartesian_layout((9, 9, 1)),
            cartesian_layout_cube_coordinates((9, 9, 1)),
            cartesian_layout_siqad_coordinates((9, 9, 1)),
        ]:
            for t in layout.coordinates():
                self.assertTrue(t <= (9, 9, 1))
                self.assertTrue(layout.is_within_bounds(t))

            for t in layout.ground_coordinates():
                self.assertTrue(t.z == 0)
                self.assertTrue(t <= (9, 9, 0))
                self.assertTrue(layout.is_within_bounds(t))

            for t in layout.adjacent_coordinates((2, 2)):
                self.assertIn(t, [(1, 2), (2, 1), (3, 2), (2, 3)])


if __name__ == "__main__":
    unittest.main()
