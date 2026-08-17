import unittest

import pytest

from mnt.pyfiction import cube_coordinate, offset_coordinate, siqad_coordinate


class TestCoordinates(unittest.TestCase):
    def test_unsigned_offset_coordinates(self):
        offset_coordinate((1, 0))
        offset_coordinate((1, 0, 0))

        with pytest.raises(RuntimeError):
            offset_coordinate((0, 0, 1, 1))
        with pytest.raises(RuntimeError):
            offset_coordinate((0, 0, 1, 1, 3))
        with pytest.raises(RuntimeError):
            offset_coordinate((0,))

        t0 = offset_coordinate(0, 0, 0)
        t1 = offset_coordinate(1, 2, 0)
        t2 = offset_coordinate(1, 2)

        assert t1.x == 1
        assert t1.y == 2
        assert t1.z == 0

        assert t0 < t1
        assert t1 > t0
        assert t1 >= t0
        assert t0 <= t1
        assert t1 == t2
        assert t2 == t1

        assert offset_coordinate(3, 2, 1).__repr__() == "(3,2,1)"

    def test_signed_cube_coordinates(self):
        cube_coordinate((1, 0))
        cube_coordinate((1, 0, 0))

        with pytest.raises(RuntimeError):
            cube_coordinate((0, 0, 1, 1))
        with pytest.raises(RuntimeError):
            cube_coordinate((0, 0, 1, 1, 3))
        with pytest.raises(RuntimeError):
            cube_coordinate((0,))

        t0 = cube_coordinate(0, 0, 0)
        t1 = cube_coordinate(1, 2, 0)
        t2 = cube_coordinate(1, 2)

        assert t1.x == 1
        assert t1.y == 2
        assert t1.z == 0

        assert t0 < t1
        assert t1 > t0
        assert t1 >= t0
        assert t0 <= t1
        assert t1 == t2
        assert t2 == t1

        assert cube_coordinate(3, 2, 1).__repr__() == "(3,2,1)"

    def test_signed_siqad_coordinates(self):
        siqad_coordinate((1, 0))
        siqad_coordinate((1, 0, 0))

        with pytest.raises(RuntimeError):
            siqad_coordinate((0, 0, 1, 1))
        with pytest.raises(RuntimeError):
            siqad_coordinate((0, 0, 1, 1, 3))
        with pytest.raises(RuntimeError):
            siqad_coordinate((0,))

        t0 = siqad_coordinate(0, 0, 0)
        t1 = siqad_coordinate(1, 2, 0)
        t2 = siqad_coordinate(1, 2)

        assert t1.x == 1
        assert t1.y == 2
        assert t1.z == 0

        assert t0 < t1
        assert t1 > t0
        assert t1 >= t0
        assert t0 <= t1
        assert t1 == t2
        assert t2 == t1

        assert siqad_coordinate(3, 2, 1).__repr__() == "(3,2,1)"


if __name__ == "__main__":
    unittest.main()
