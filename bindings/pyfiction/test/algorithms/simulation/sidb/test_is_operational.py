from mnt.pyfiction import *
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))

class TestIsOperational(unittest.TestCase):

    def test_is_operational(self):
        lyt = sidb_100_lattice()

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

    def test_and_gate_kinks(self):

        lyt = read_sqd_layout_100(dir_path + "/../../../resources/AND_mu_032_kinks.sqd")

        params = is_operational_params()
        params.simulation_parameters = sidb_simulation_parameters(2, -0.32)

        [op_status, evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], params)

        self.assertEqual(op_status, operational_status.OPERATIONAL)

        params.op_condition = operational_condition.FORBID_KINKS

        [op_status, evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], params)

        self.assertEqual(op_status, operational_status.NON_OPERATIONAL)

    def test_and_gate_111_lattice(self):
        lyt = read_sqd_layout_111(dir_path + "/../../../resources/AND_mu_032_0.sqd")

        params = is_operational_params()
        params.simulation_parameters = sidb_simulation_parameters(2, -0.32)

        [op_status, evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], params)

        self.assertEqual(op_status, operational_status.OPERATIONAL)

        params.simulation_parameters = sidb_simulation_parameters(2, -0.1)

        self.assertEqual(params.simulation_parameters.mu_minus, -0.1)

        [op_status, evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], params)

        self.assertEqual(op_status, operational_status.NON_OPERATIONAL)

    def test_and_gate_111_lattice_operational_input_pattern(self):
        lyt = read_sqd_layout_111(dir_path + "/../../../resources/AND_mu_032_0.sqd")

        params = is_operational_params()
        params.simulation_parameters = sidb_simulation_parameters(2, -0.30)

        operational_patterns = operational_input_patterns(lyt, [create_and_tt()], params)

        print(operational_patterns)
        self.assertEqual(len(operational_patterns), 2)

        self.assertEqual(operational_patterns, {0, 3})

if __name__ == '__main__':
    unittest.main()
