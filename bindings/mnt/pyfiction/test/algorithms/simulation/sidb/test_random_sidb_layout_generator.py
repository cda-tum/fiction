from mnt.pyfiction import *
import unittest
import os


class TestRandomSiDBLayoutGenerator(unittest.TestCase):

    def test_area_with_one_sidb_100_lattice(self):
        params = generate_random_sidb_layout_params()
        params.number_of_sidbs = 1
        params.coordinate_pair = ((10,10), (10,10))
        result_lyt = generate_random_sidb_layout(sidb_100_lattice(), params)
        self.assertEqual(result_lyt.num_cells(), 1)
        cell = (result_lyt.cells())[0]
        self.assertEqual(cell.x, 10)
        self.assertEqual(cell.y, 10)

    def test_area_with_five_sidb_100_lattice(self):
        params = generate_random_sidb_layout_params()
        params.number_of_sidbs = 5
        print(params.number_of_sidbs)
        params.coordinate_pair = ((0,0), (10,10))
        result_lyt = generate_random_sidb_layout(sidb_100_lattice(), params)
        self.assertEqual(result_lyt.num_cells(), 5)

    def test_area_with_one_coordinate_111_lattice(self):
        params = generate_random_sidb_layout_params()
        params.number_of_sidbs = 1
        params.coordinate_pair = ((10, 10), (10, 10))
        result_lyt = generate_random_sidb_layout(sidb_111_lattice(), params)
        self.assertEqual(result_lyt.num_cells(), 1)
        cell = (result_lyt.cells())[0]
        self.assertEqual(cell.x, 10)
        self.assertEqual(cell.y, 10)

if __name__ == '__main__':
    unittest.main()
