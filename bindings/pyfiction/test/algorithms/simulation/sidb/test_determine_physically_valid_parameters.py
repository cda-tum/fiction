import unittest

from mnt.pyfiction import *


class TestDeterminePhysicallyValidParameters(unittest.TestCase):

    def test_one_DB_100_lattice(self):
        layout = sidb_100_lattice((10, 10))
        layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
        cds = charge_distribution_surface_100(layout)

        valid_parameters = determine_physically_valid_parameters(cds)

    def test_one_DB_111_lattice(self):
        layout = sidb_111_lattice((10, 10))
        layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
        cds = charge_distribution_surface_111(layout)

        valid_parameters = determine_physically_valid_parameters(cds)

if __name__ == '__main__':
    unittest.main()
