from mnt.pyfiction import *
import unittest
import os


class TestIsOperational(unittest.TestCase):

    def test_is_operational(self):
        lyt = sidb_layout((7, 0))

        lyt = charge_distribution_surface(lyt)

        lyt.assign_cell_type((0, 0, 1), sidb_technology.cell_type.INPUT)
        lyt.assign_cell_type((2, 1, 1), sidb_technology.cell_type.INPUT)

        lyt.assign_cell_type((20, 0, 1), sidb_technology.cell_type.INPUT)
        lyt.assign_cell_type((19, 1, 1), sidb_technology.cell_type.INPUT)

        lyt.assign_cell_type((4, 2, 1), sidb_technology.cell_type.NORMAL)
        lyt.assign_cell_type((6, 3, 1), sidb_technology.cell_type.NORMAL)

        lyt.assign_cell_type((14, 3, 1), sidb_technology.cell_type.NORMAL)
        lyt.assign_cell_type((16, 2, 1), sidb_technology.cell_type.NORMAL)

        lyt.assign_cell_type((10, 6, 0), sidb_technology.cell_type.OUTPUT)
        lyt.assign_cell_type((10, 7, 0), sidb_technology.cell_type.OUTPUT)

        lyt.assign_cell_type((10, 9, 1), sidb_technology.cell_type.NORMAL)

        params = is_operational_params()
        params.sim_params = sidb_simulation_parameters(2, -0.28)

        [op_status, evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], params)

        self.assertEqual(op_status, operational_status.OPERATIONAL)

        params.sim_params = sidb_simulation_parameters(2, -0.1)

        [op_status, evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], params)

        self.assertEqual(op_status, operational_status.NON_OPERATIONAL)


if __name__ == '__main__':
    unittest.main()
