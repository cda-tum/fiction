import unittest

from mnt.pyfiction import (sidb_100_lattice, sidb_technology, can_positive_charges_occur, sidb_simulation_parameters,
                           sidb_111_lattice)


class TestCanPositiveChargesOccur(unittest.TestCase):

    def test_three_DBs_100_lattice(self):
        layout = sidb_100_lattice((2, 3))
        layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((1, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((2, 0), sidb_technology.cell_type.NORMAL)

        self.assertTrue(can_positive_charges_occur(layout, sidb_simulation_parameters()))

        params = sidb_simulation_parameters()
        params.mu_minus = -0.8
        self.assertFalse(can_positive_charges_occur(layout, params))

    def test_three_DBs_111_lattice(self):
        layout = sidb_111_lattice((2, 3))
        layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((1, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((2, 0), sidb_technology.cell_type.NORMAL)

        params = sidb_simulation_parameters()
        params.mu_minus = -0.05

        self.assertTrue(can_positive_charges_occur(layout, params))

        params.mu_minus = -0.8
        self.assertFalse(can_positive_charges_occur(layout, params))


if __name__ == '__main__':
    unittest.main()
