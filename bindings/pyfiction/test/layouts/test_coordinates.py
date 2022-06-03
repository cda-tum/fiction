from fiction import pyfiction
import unittest


class TestCoordinates(unittest.TestCase):

    def test_unsigned_offset_coordinates(self):
        pyfiction.coordinate((1, 0))
        pyfiction.coordinate((1, 0, 0))

        with self.assertRaises(RuntimeError):
            pyfiction.coordinate((0, 0, 1, 1))
        with self.assertRaises(RuntimeError):
            pyfiction.coordinate((0, 0, 1, 1, 3))
        with self.assertRaises(RuntimeError):
            pyfiction.coordinate((0,))

        t0 = pyfiction.coordinate(0, 0, 0)
        t1 = pyfiction.coordinate(1, 2, 0)
        t2 = pyfiction.coordinate(1, 2)

        self.assertTrue(t1.x == 1)
        self.assertTrue(t1.y == 2)
        self.assertTrue(t1.z == 0)

        self.assertTrue(t0 < t1)
        self.assertTrue(t1 > t0)
        self.assertTrue(t1 >= t0)
        self.assertTrue(t0 <= t1)
        self.assertTrue(t1 == t2)
        self.assertTrue(t2 == t1)

        self.assertEqual(pyfiction.coordinate(3, 2, 1).__repr__(), "(3,2,1)")


if __name__ == '__main__':
    unittest.main()
