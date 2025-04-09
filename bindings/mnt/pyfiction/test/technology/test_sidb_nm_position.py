import unittest

from mnt.pyfiction import sidb_lattice, sidb_layout, sidb_nm_position


class TestSiDBNmPosition(unittest.TestCase):
    def test_layout_without_lattice_orientation(self):
        layout_one = sidb_layout((10, 10))

        result = sidb_nm_position(layout_one, (0, 0))

        self.assertEqual(result, (0, 0))

    def test_layout_with_100_lattice_orientation(self):
        layout_one = sidb_lattice((10, 10), orientation="100")

        result = sidb_nm_position(layout_one, (0, 0))

        self.assertEqual(result, (0, 0))

    def test_layout_with_111_lattice_orientation(self):
        layout_one = sidb_lattice((10, 10), orientation="111")

        result = sidb_nm_position(layout_one, (0, 0))

        self.assertEqual(result, (0, 0))


if __name__ == "__main__":
    unittest.main()
