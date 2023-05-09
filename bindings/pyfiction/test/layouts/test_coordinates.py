from fiction.pyfiction import *
import unittest


class TestCoordinates(unittest.TestCase):

    def test_unsigned_offset_coordinates(self):
        coordinate((1, 0))
        coordinate((1, 0, 0))

        with self.assertRaises(RuntimeError):
            coordinate((0, 0, 1, 1))
        with self.assertRaises(RuntimeError):
            coordinate((0, 0, 1, 1, 3))
        with self.assertRaises(RuntimeError):
            coordinate((0,))

        t0 = coordinate(0, 0, 0)
        t1 = coordinate(1, 2, 0)
        t2 = coordinate(1, 2)

        self.assertTrue(t1.x == 1)
        self.assertTrue(t1.y == 2)
        self.assertTrue(t1.z == 0)

        self.assertTrue(t0 < t1)
        self.assertTrue(t1 > t0)
        self.assertTrue(t1 >= t0)
        self.assertTrue(t0 <= t1)
        self.assertTrue(t1 == t2)
        self.assertTrue(t2 == t1)

        self.assertEqual(coordinate(3, 2, 1).__repr__(), "(3,2,1)")

    def test_siqad_coordinate(self):
        # Create a siqad_coordinate object using different constructors
        coord1 = siqad_coordinate()
        coord2 = siqad_coordinate(10, 20)
        coord3 = siqad_coordinate(5, 8, 1)
        coord4 = siqad_coordinate(coord2)
        coord5 = siqad_coordinate((2, 4))
        coord6 = siqad_coordinate((-2, -4))

        # Access and modify coordinate properties
        coord1.x = 3
        coord1.y = 6
        coord1.z = 1

        self.assertEqual(coord1.x, 3)
        self.assertEqual(coord1.y, 6)
        self.assertEqual(coord1.z, 1)
        self.assertEqual(coord6.z, 0)
        self.assertEqual(coord6.x, -2)
        self.assertEqual(coord6.y, -4)

        # Perform comparison operations
        self.assertFalse(coord1 == coord2)
        self.assertTrue(coord2 != coord3)
        self.assertFalse(coord3 < coord4)
        self.assertTrue(coord4 > coord5)

        # Get string representation of a siqad_coordinate
        self.assertEqual(coord3.__repr__(), "(5,8,1)")

if __name__ == '__main__':
    unittest.main()
