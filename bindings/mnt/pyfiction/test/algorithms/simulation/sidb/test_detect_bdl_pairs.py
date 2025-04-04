import unittest

from mnt.pyfiction import (
    charge_distribution_surface,
    detect_bdl_pairs,
    detect_bdl_pairs_params,
    sidb_lattice,
    sidb_technology,
)


class TestDetectBDLPairs(unittest.TestCase):
    def test_detect_bdl_pairs_100_lattice(self):
        lyt = sidb_lattice((7, 0), orientation="100")

        lyt = charge_distribution_surface(lyt)

        lyt.assign_cell_type((0, 0, 0), sidb_technology.cell_type.INPUT)
        lyt.assign_cell_type((1, 0, 0), sidb_technology.cell_type.INPUT)

        lyt.assign_cell_type((2, 0, 0), sidb_technology.cell_type.NORMAL)
        lyt.assign_cell_type((3, 0, 0), sidb_technology.cell_type.NORMAL)
        lyt.assign_cell_type((4, 0, 0), sidb_technology.cell_type.NORMAL)
        lyt.assign_cell_type((5, 0, 0), sidb_technology.cell_type.NORMAL)

        lyt.assign_cell_type((6, 0, 0), sidb_technology.cell_type.OUTPUT)
        lyt.assign_cell_type((7, 0, 0), sidb_technology.cell_type.OUTPUT)

        params = detect_bdl_pairs_params()

        input_bdl_pairs = detect_bdl_pairs(lyt, sidb_technology.cell_type.INPUT, params)
        output_bdl_pairs = detect_bdl_pairs(lyt, sidb_technology.cell_type.OUTPUT, params)
        normal_bdl_pairs = detect_bdl_pairs(lyt, sidb_technology.cell_type.NORMAL, params)

        self.assertEqual(len(input_bdl_pairs), 0)
        self.assertEqual(len(output_bdl_pairs), 0)
        self.assertEqual(len(normal_bdl_pairs), 2)

    def test_detect_bdl_pairs_111_lattice(self):
        lyt = sidb_lattice((7, 0), orientation="111")

        lyt = charge_distribution_surface(lyt)

        lyt.assign_cell_type((0, 0, 0), sidb_technology.cell_type.INPUT)
        lyt.assign_cell_type((1, 0, 0), sidb_technology.cell_type.INPUT)

        lyt.assign_cell_type((2, 0, 0), sidb_technology.cell_type.NORMAL)
        lyt.assign_cell_type((3, 0, 0), sidb_technology.cell_type.NORMAL)
        lyt.assign_cell_type((4, 0, 0), sidb_technology.cell_type.NORMAL)
        lyt.assign_cell_type((5, 0, 0), sidb_technology.cell_type.NORMAL)

        lyt.assign_cell_type((6, 0, 0), sidb_technology.cell_type.OUTPUT)
        lyt.assign_cell_type((7, 0, 0), sidb_technology.cell_type.OUTPUT)

        params = detect_bdl_pairs_params()

        input_bdl_pairs = detect_bdl_pairs(lyt, sidb_technology.cell_type.INPUT, params)
        output_bdl_pairs = detect_bdl_pairs(lyt, sidb_technology.cell_type.OUTPUT, params)
        normal_bdl_pairs = detect_bdl_pairs(lyt, sidb_technology.cell_type.NORMAL, params)

        self.assertEqual(len(input_bdl_pairs), 0)
        self.assertEqual(len(output_bdl_pairs), 0)
        self.assertEqual(len(normal_bdl_pairs), 2)


if __name__ == "__main__":
    unittest.main()
