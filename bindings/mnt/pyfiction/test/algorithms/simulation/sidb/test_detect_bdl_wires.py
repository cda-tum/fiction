import unittest

from mnt.pyfiction import (
    bdl_wire_selection,
    charge_distribution_surface,
    detect_bdl_wires_100,
    detect_bdl_wires_params,
    sidb_lattice,
    sidb_technology,
)


class TestDetectBDLWires(unittest.TestCase):
    def test_detect_bdl_wires_100_lattice(self):
        lyt = sidb_lattice((7, 0), orientation="100")

        lyt = charge_distribution_surface(lyt)

        lyt.assign_cell_type((38, 0, 0), sidb_technology.cell_type.OUTPUT)
        lyt.assign_cell_type((0, 0, 0), sidb_technology.cell_type.OUTPUT)

        lyt.assign_cell_type((36, 2, 0), sidb_technology.cell_type.OUTPUT)
        lyt.assign_cell_type((2, 2, 0), sidb_technology.cell_type.OUTPUT)

        lyt.assign_cell_type((6, 4, 0), sidb_technology.cell_type.NORMAL)
        lyt.assign_cell_type((32, 4, 0), sidb_technology.cell_type.NORMAL)

        lyt.assign_cell_type((30, 6, 0), sidb_technology.cell_type.NORMAL)
        lyt.assign_cell_type((8, 6, 0), sidb_technology.cell_type.NORMAL)

        lyt.assign_cell_type((26, 8, 0), sidb_technology.cell_type.NORMAL)
        lyt.assign_cell_type((12, 8, 0), sidb_technology.cell_type.NORMAL)

        lyt.assign_cell_type((24, 10, 0), sidb_technology.cell_type.NORMAL)
        lyt.assign_cell_type((14, 10, 0), sidb_technology.cell_type.NORMAL)

        lyt.assign_cell_type((24, 30, 0), sidb_technology.cell_type.NORMAL)
        lyt.assign_cell_type((26, 32, 0), sidb_technology.cell_type.NORMAL)

        lyt.assign_cell_type((30, 34, 0), sidb_technology.cell_type.INPUT)
        lyt.assign_cell_type((32, 36, 0), sidb_technology.cell_type.INPUT)

        params = detect_bdl_wires_params()

        all_bdl_wires = detect_bdl_wires_100(lyt, params, bdl_wire_selection.ALL)
        output_bdl_wires = detect_bdl_wires_100(lyt, params, bdl_wire_selection.OUTPUT)
        input_bdl_wires = detect_bdl_wires_100(lyt, params, bdl_wire_selection.INPUT)

        self.assertEqual(len(all_bdl_wires), 3)
        self.assertEqual(len(output_bdl_wires), 2)
        self.assertEqual(len(input_bdl_wires), 1)


if __name__ == "__main__":
    unittest.main()
