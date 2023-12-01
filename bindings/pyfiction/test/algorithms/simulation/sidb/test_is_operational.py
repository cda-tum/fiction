from mnt.pyfiction import *
import unittest
import os


class TestIsOperational(unittest.TestCase):

    def test_is_operational(self):
        lyt = sidb_layout()

        lyt = charge_distribution_surface(lyt)

        lyt.assign_cell_type((0, 1), sidb_technology.cell_type.INPUT)
        lyt.assign_cell_type((2, 3), sidb_technology.cell_type.INPUT)

        lyt.assign_cell_type((20, 1), sidb_technology.cell_type.INPUT)
        lyt.assign_cell_type((19, 3), sidb_technology.cell_type.INPUT)

        lyt.assign_cell_type((4, 5), sidb_technology.cell_type.NORMAL)
        lyt.assign_cell_type((6, 7), sidb_technology.cell_type.NORMAL)

        lyt.assign_cell_type((14, 7), sidb_technology.cell_type.NORMAL)
        lyt.assign_cell_type((16, 5), sidb_technology.cell_type.NORMAL)

        lyt.assign_cell_type((10, 12, 0), sidb_technology.cell_type.OUTPUT)
        lyt.assign_cell_type((10, 14, 0), sidb_technology.cell_type.OUTPUT)

        lyt.assign_cell_type((10, 19), sidb_technology.cell_type.NORMAL)

        params = is_operational_params()
        params.simulation_parameters = sidb_simulation_parameters(2, -0.28)

        [op_status, evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], params)

        self.assertEqual(op_status, operational_status.OPERATIONAL)

        params.simulation_parameters = sidb_simulation_parameters(2, -0.1)

        [op_status, evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], params)

        self.assertEqual(op_status, operational_status.NON_OPERATIONAL)


if __name__ == '__main__':
    unittest.main()
