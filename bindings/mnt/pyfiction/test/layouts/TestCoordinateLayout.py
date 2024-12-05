from mnt.pyfiction import (CoordinateLayout)
import unittest


class TestCoordinateLayout(unittest.TestCase):

    def test_cartesian_layout(self):
        layout = CoordinateLayout("cartesian", (9, 9, 1))

        self.assertTrue(layout.x() == 9)
        self.assertTrue(layout.y() == 9)
        self.assertTrue(layout.z() == 1)

        self.assertTrue(layout.area() == 100)
        layout.resize((10, 10, 1))
        self.assertTrue(layout.area() == 121)

        self.assertTrue(layout.north((1, 1, 0)) == (1, 0, 0))
        self.assertTrue(layout.north_east((1, 1, 0)) == (2, 0, 0))
        self.assertTrue(layout.east((1, 1, 0)) == (2, 1, 0))
        self.assertTrue(layout.south_east((1, 1, 0)) == (2, 2, 0))
        self.assertTrue(layout.south((1, 1, 0)) == (1, 2, 0))
        self.assertTrue(layout.south_west((1, 1, 0)) == (0, 2, 0))
        self.assertTrue(layout.west((1, 1, 0)) == (0, 1, 0))
        self.assertTrue(layout.north_west((1, 1, 0)) == (0, 0, 0))

        self.assertTrue(layout.above((1, 1, 0)) == (1, 1, 1))
        self.assertTrue(layout.below((1, 1, 1)) == (1, 1, 0))

        self.assertTrue(layout.is_north_of((1, 1, 0), (1, 0, 0)))
        self.assertTrue(layout.is_east_of((1, 1, 0), (2, 1, 0)))
        self.assertTrue(layout.is_south_of((1, 1, 0), (1, 2, 0)))
        self.assertTrue(layout.is_west_of((1, 1, 0), (0, 1, 0)))

        self.assertTrue(layout.is_above((1, 1, 0), (1, 1, 1)))
        self.assertTrue(layout.is_below((1, 1, 1), (1, 1, 0)))

        self.assertTrue(layout.is_northwards_of((1, 1, 0), (1, 0, 0)))
        self.assertTrue(layout.is_eastwards_of((1, 1, 0), (2, 1, 0)))
        self.assertTrue(layout.is_southwards_of((1, 1, 0), (1, 2, 0)))
        self.assertTrue(layout.is_westwards_of((1, 1, 0), (0, 1, 0)))

        self.assertTrue(layout.is_at_northern_border((0, 0, 0)))
        self.assertTrue(layout.is_at_eastern_border((10, 10, 0)))
        self.assertTrue(layout.is_at_southern_border((10, 10, 0)))
        self.assertTrue(layout.is_at_western_border((0, 0, 0)))
        self.assertTrue(layout.is_at_any_border((0, 0, 0)))

        self.assertTrue(layout.northern_border_of((1, 1, 0)) == (1, 0, 0))
        self.assertTrue(layout.eastern_border_of((1, 1, 0)) == (10, 1, 0))
        self.assertTrue(layout.southern_border_of((1, 1, 0)) == (1, 10, 0))
        self.assertTrue(layout.western_border_of((1, 1, 0)) == (0, 1, 0))

        self.assertTrue(layout.is_ground_layer((0, 0, 0)))
        self.assertTrue(layout.is_crossing_layer((0, 0, 1)))
        self.assertTrue(layout.is_within_bounds((0, 0, 0)))

        for t in layout.coordinates():
            self.assertTrue(t <= (10, 10, 1))
            self.assertTrue(layout.is_within_bounds(t))

        for t in layout.ground_coordinates():
            self.assertTrue(t.z == 0)
            self.assertTrue(t <= (10, 10, 0))
            self.assertTrue(layout.is_within_bounds(t))

        for t in layout.adjacent_coordinates((2, 2)):
            self.assertIn(t, [(1, 2), (2, 1), (3, 2), (2, 3)])

        for t in layout.adjacent_opposite_coordinates((2, 2)):
            self.assertIn(t, [((3, 2), (1, 2)), ((2, 1), (2, 3))])

    def test_shifted_cartesian_layout(self):
        layout = CoordinateLayout("shifted_cartesian", (9, 9, 1))

        self.assertTrue(layout.x() == 9)
        self.assertTrue(layout.y() == 9)
        self.assertTrue(layout.z() == 1)

        self.assertTrue(layout.area() == 100)
        layout.resize((10, 10, 1))
        self.assertTrue(layout.area() == 121)

        self.assertTrue(layout.north((1, 1, 0)) == (1, 0, 0))
        self.assertTrue(layout.north_east((1, 1, 0)) == (2, 1, 0))
        self.assertTrue(layout.east((1, 1, 0)) == (2, 1, 0))
        self.assertTrue(layout.south_east((1, 1, 0)) == (2, 2, 0))
        self.assertTrue(layout.south((1, 1, 0)) == (1, 2, 0))
        self.assertTrue(layout.south_west((1, 1, 0)) == (0, 2, 0))
        self.assertTrue(layout.west((1, 1, 0)) == (0, 1, 0))
        self.assertTrue(layout.north_west((1, 1, 0)) == (0, 1, 0))

        self.assertTrue(layout.above((1, 1, 0)) == (1, 1, 1))
        self.assertTrue(layout.below((1, 1, 1)) == (1, 1, 0))

        self.assertTrue(layout.is_north_of((1, 1, 0), (1, 0, 0)))
        self.assertTrue(layout.is_east_of((1, 1, 0), (2, 1, 0)))
        self.assertTrue(layout.is_south_of((1, 1, 0), (1, 2, 0)))
        self.assertTrue(layout.is_west_of((1, 1, 0), (0, 1, 0)))

        self.assertTrue(layout.is_above((1, 1, 0), (1, 1, 1)))
        self.assertTrue(layout.is_below((1, 1, 1), (1, 1, 0)))

        self.assertTrue(layout.is_northwards_of((1, 1, 0), (1, 0, 0)))
        self.assertTrue(layout.is_eastwards_of((1, 1, 0), (2, 1, 0)))
        self.assertTrue(layout.is_southwards_of((1, 1, 0), (1, 2, 0)))
        self.assertTrue(layout.is_westwards_of((1, 1, 0), (0, 1, 0)))

        self.assertTrue(layout.is_at_northern_border((0, 0, 0)))
        self.assertTrue(layout.is_at_eastern_border((10, 10, 0)))
        self.assertTrue(layout.is_at_southern_border((10, 10, 0)))
        self.assertTrue(layout.is_at_western_border((0, 0, 0)))
        self.assertTrue(layout.is_at_any_border((0, 0, 0)))

        self.assertTrue(layout.northern_border_of((1, 1, 0)) == (1, 0, 0))
        self.assertTrue(layout.eastern_border_of((1, 1, 0)) == (10, 1, 0))
        self.assertTrue(layout.southern_border_of((1, 1, 0)) == (1, 10, 0))
        self.assertTrue(layout.western_border_of((1, 1, 0)) == (0, 1, 0))

        self.assertTrue(layout.is_ground_layer((0, 0, 0)))
        self.assertTrue(layout.is_crossing_layer((0, 0, 1)))
        self.assertTrue(layout.is_within_bounds((0, 0, 0)))

        for t in layout.coordinates():
            self.assertTrue(t <= (10, 10, 1))
            self.assertTrue(layout.is_within_bounds(t))

        for t in layout.ground_coordinates():
            self.assertTrue(t.z == 0)
            self.assertTrue(t <= (10, 10, 0))
            self.assertTrue(layout.is_within_bounds(t))

        for t in layout.adjacent_coordinates((2, 2)):
            self.assertIn(t, [(1, 1), (1, 2), (2, 1), (3, 1), (3, 2), (2, 3)])

        for t in layout.adjacent_opposite_coordinates((2, 2)):
            self.assertIn(t, [((3, 1), (1, 2)), ((1, 1), (3, 2)), ((2, 1), (2, 3))])

    def test_hexagonal_layout(self):
        layout = CoordinateLayout("hexagonal", (9, 9, 1))

        self.assertTrue(layout.x() == 9)
        self.assertTrue(layout.y() == 9)
        self.assertTrue(layout.z() == 1)

        self.assertTrue(layout.area() == 100)
        layout.resize((10, 10, 1))
        self.assertTrue(layout.area() == 121)

        self.assertTrue(layout.north((1, 1, 0)) == (1, 0, 0))
        self.assertTrue(layout.north_east((1, 1, 0)) == (1, 0, 0))
        self.assertTrue(layout.east((1, 1, 0)) == (2, 1, 0))
        self.assertTrue(layout.south_east((1, 1, 0)) == (1, 2, 0))
        self.assertTrue(layout.south((1, 1, 0)) == (1, 2, 0))
        self.assertTrue(layout.south_west((1, 1, 0)) == (0, 2, 0))
        self.assertTrue(layout.west((1, 1, 0)) == (0, 1, 0))
        self.assertTrue(layout.north_west((1, 1, 0)) == (0, 0, 0))

        self.assertTrue(layout.above((1, 1, 0)) == (1, 1, 1))
        self.assertTrue(layout.below((1, 1, 1)) == (1, 1, 0))

        self.assertTrue(layout.is_north_of((1, 1, 0), (1, 0, 0)))
        self.assertTrue(layout.is_east_of((1, 1, 0), (2, 1, 0)))
        self.assertTrue(layout.is_south_of((1, 1, 0), (1, 2, 0)))
        self.assertTrue(layout.is_west_of((1, 1, 0), (0, 1, 0)))

        self.assertTrue(layout.is_above((1, 1, 0), (1, 1, 1)))
        self.assertTrue(layout.is_below((1, 1, 1), (1, 1, 0)))

        self.assertTrue(layout.is_northwards_of((1, 1, 0), (1, 0, 0)))
        self.assertTrue(layout.is_eastwards_of((1, 1, 0), (2, 1, 0)))
        self.assertTrue(layout.is_southwards_of((1, 1, 0), (1, 2, 0)))
        self.assertTrue(layout.is_westwards_of((1, 1, 0), (0, 1, 0)))

        self.assertTrue(layout.is_at_northern_border((0, 0, 0)))
        self.assertTrue(layout.is_at_eastern_border((10, 10, 0)))
        self.assertTrue(layout.is_at_southern_border((10, 10, 0)))
        self.assertTrue(layout.is_at_western_border((0, 0, 0)))
        self.assertTrue(layout.is_at_any_border((0, 0, 0)))

        self.assertTrue(layout.northern_border_of((1, 1, 0)) == (1, 0, 0))
        self.assertTrue(layout.eastern_border_of((1, 1, 0)) == (10, 1, 0))
        self.assertTrue(layout.southern_border_of((1, 1, 0)) == (1, 10, 0))
        self.assertTrue(layout.western_border_of((1, 1, 0)) == (0, 1, 0))

        self.assertTrue(layout.is_ground_layer((0, 0, 0)))
        self.assertTrue(layout.is_crossing_layer((0, 0, 1)))
        self.assertTrue(layout.is_within_bounds((0, 0, 0)))

        for t in layout.coordinates():
            self.assertTrue(t <= (10, 10, 1))
            self.assertTrue(layout.is_within_bounds(t))

        for t in layout.ground_coordinates():
            self.assertTrue(t.z == 0)
            self.assertTrue(t <= (10, 10, 0))
            self.assertTrue(layout.is_within_bounds(t))

        for t in layout.adjacent_coordinates((2, 2)):
            self.assertIn(t, [(2, 1), (3, 1), (3, 2), (3, 3), (2, 3), (1, 2), (2, 1)])

        for t in layout.adjacent_opposite_coordinates((2, 2)):
            self.assertIn(t, [((3, 2), (1, 2)), ((3, 1), (2, 3)), ((2, 1), (3, 3))])


if __name__ == '__main__':
    unittest.main()
