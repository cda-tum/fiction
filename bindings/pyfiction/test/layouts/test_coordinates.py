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


if __name__ == '__main__':
    unittest.main()
