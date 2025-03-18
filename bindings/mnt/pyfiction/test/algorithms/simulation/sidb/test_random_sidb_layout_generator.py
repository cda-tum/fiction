import unittest

from mnt.pyfiction import (
    charge_distribution_surface_100,
    charge_distribution_surface_111,
    generate_multiple_random_sidb_layouts,
    generate_random_sidb_layout,
    generate_random_sidb_layout_params,
    sidb_lattice,
    sidb_layout,
)


class TestRandomSiDBLayoutGenerator(unittest.TestCase):
    def test_area_with_one_sidb_100_lattice(self):
        params = generate_random_sidb_layout_params()
        params.number_of_sidbs = 1
        params.coordinate_pair = ((10, 10), (10, 10))
        result_lyt = generate_random_sidb_layout(params, sidb_lattice(orienation="100"))
        self.assertEqual(result_lyt.num_cells(), 1)
        cell = (result_lyt.cells())[0]
        self.assertEqual(cell.x, 10)
        self.assertEqual(cell.y, 10)

    def test_area_with_five_sidb_layout(self):
        params = generate_random_sidb_layout_params()
        params.number_of_sidbs = 5
        print(params.number_of_sidbs)
        params.coordinate_pair = ((0, 0), (10, 10))
        result_lyt = generate_random_sidb_layout(params, sidb_layout())
        self.assertEqual(result_lyt.num_cells(), 5)

    def test_area_with_five_sidb_100_lattice(self):
        params = generate_random_sidb_layout_params()
        params.number_of_sidbs = 5
        print(params.number_of_sidbs)
        params.coordinate_pair = ((0, 0), (10, 10))
        result_lyt = generate_random_sidb_layout(params, sidb_lattice(orienation="100"))
        self.assertEqual(result_lyt.num_cells(), 5)

    def test_area_with_five_sidbs_cds_100(self):
        params = generate_random_sidb_layout_params()
        params.number_of_sidbs = 5
        print(params.number_of_sidbs)
        params.coordinate_pair = ((0, 0), (10, 10))
        result_lyt = generate_random_sidb_layout(params, charge_distribution_surface_100())
        self.assertEqual(result_lyt.num_cells(), 5)

    def test_area_with_five_sidbs_cds_111(self):
        params = generate_random_sidb_layout_params()
        params.number_of_sidbs = 5
        print(params.number_of_sidbs)
        params.coordinate_pair = ((0, 0), (10, 10))
        result_lyt = generate_random_sidb_layout(params, charge_distribution_surface_111())
        self.assertEqual(result_lyt.num_cells(), 5)

    def test_area_with_one_coordinate_111_lattice(self):
        params = generate_random_sidb_layout_params()
        params.number_of_sidbs = 1
        params.coordinate_pair = ((10, 10), (10, 10))
        result_lyt = generate_random_sidb_layout(params, sidb_lattice(orienation="111"))
        self.assertEqual(result_lyt.num_cells(), 1)
        cell = (result_lyt.cells())[0]
        self.assertEqual(cell.x, 10)
        self.assertEqual(cell.y, 10)

    def test_impossible_design_of_single_layout(self):
        params = generate_random_sidb_layout_params()
        params.number_of_sidbs = 2
        result_lyt = generate_random_sidb_layout(params, sidb_layout())
        self.assertIsNone(result_lyt)

    def test_impossible_design_of_mutiple_layouts(self):
        params = generate_random_sidb_layout_params()
        params.maximal_attempts_for_multiple_layouts = 5
        params.number_of_sidbs = 2
        result_lyt = generate_multiple_random_sidb_layouts(params, sidb_layout())
        self.assertIsNone(result_lyt)


if __name__ == "__main__":
    unittest.main()
