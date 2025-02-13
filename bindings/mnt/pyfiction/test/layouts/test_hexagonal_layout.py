import unittest

from mnt.pyfiction import hexagonal_layout


class TestHexagonalLayout(unittest.TestCase):
    def test_coordinate_iteration(self):
        for layout in [
            hexagonal_layout((9, 9, 1)),
            hexagonal_layout((9, 9, 1), coordinate_type="cube"),
        ]:
            for t in layout.coordinates():
                self.assertLessEqual(t, (9, 9, 1))
                self.assertTrue(layout.is_within_bounds(t))

            for t in layout.ground_coordinates():
                self.assertEqual(t.z, 0)
                self.assertLessEqual(t, (9, 9, 0))
                self.assertTrue(layout.is_within_bounds(t))

            for t in layout.adjacent_coordinates((2, 2)):
                self.assertIn(t, [(2, 1), (3, 1), (3, 2), (3, 3), (2, 3), (1, 2), (2, 1)])


if __name__ == "__main__":
    unittest.main()
