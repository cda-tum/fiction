import os
import unittest

from mnt.pyfiction import (
    automatic_base_number_detection,
    charge_distribution_surface_100,
    charge_distribution_surface_111,
    groundstate_from_simulation_result,
    offset_coordinate,
    quickexact,
    quickexact_params,
    read_sqd_layout_100,
    sidb_100_lattice,
    sidb_111_lattice,
    sidb_charge_state,
    sidb_simulation_parameters,
    sidb_technology,
)

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestQuickExact(unittest.TestCase):
    def test_three_sidbs(self):
        layout = sidb_100_lattice((2, 1))
        layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((1, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((2, 0), sidb_technology.cell_type.NORMAL)

        params = quickexact_params()
        params.simulation_parameters.base = 2
        params.simulation_parameters.mu_minus = -0.25
        params.base_number_detection = automatic_base_number_detection.OFF
        self.assertEqual(params.simulation_parameters.mu_minus, -0.25)
        self.assertEqual(params.base_number_detection, automatic_base_number_detection.OFF)

        cds = charge_distribution_surface_100(layout)

        result = quickexact(cds, params)

        self.assertEqual(result.algorithm_name, "QuickExact")
        self.assertLessEqual(len(result.charge_distributions), 3)

        params.base_number_detection = automatic_base_number_detection.ON
        self.assertEqual(params.base_number_detection, automatic_base_number_detection.ON)

        result = quickexact(cds, params)
        self.assertLessEqual(len(result.charge_distributions), 4)

        params.simulation_parameters.epsilon_r = 2
        params.simulation_parameters.lambda_tf = 2
        result = quickexact(cds, params)
        self.assertLessEqual(len(result.charge_distributions), 2)

    def test_perturber_and_sidb_pair_111(self):
        layout = sidb_111_lattice((4, 1))
        layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((1, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((2, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((3, 0), sidb_technology.cell_type.NORMAL)

        params = quickexact_params()
        params.simulation_parameters.base = 2
        params.simulation_parameters.mu_minus = -0.32
        params.base_number_detection = automatic_base_number_detection.OFF
        self.assertEqual(params.simulation_parameters.mu_minus, -0.32)
        self.assertEqual(params.base_number_detection, automatic_base_number_detection.OFF)

        cds = charge_distribution_surface_111(layout)

        result = quickexact(cds, params)

        self.assertEqual(result.algorithm_name, "QuickExact")

        groundstate = groundstate_from_simulation_result(result)

        self.assertEqual(len(groundstate), 1)

        self.assertEqual(groundstate[0].get_charge_state((0, 0)), sidb_charge_state.NEGATIVE)
        self.assertEqual(groundstate[0].get_charge_state((1, 0)), sidb_charge_state.NEUTRAL)
        self.assertEqual(groundstate[0].get_charge_state((2, 0)), sidb_charge_state.NEUTRAL)
        self.assertEqual(groundstate[0].get_charge_state((3, 0)), sidb_charge_state.NEGATIVE)

    def test_simulate_all_inputs_of_and_gate(self):
        and_gate = read_sqd_layout_100(dir_path + "/../../../resources/and_v0.sqd")
        physical_parameters = sidb_simulation_parameters()
        physical_parameters.base = 2
        physical_parameters.epsilon_r = 5.6
        physical_parameters.lambda_tf = 5.0  # (nm)
        physical_parameters.mu_minus = (
            -0.25
        )  # (eV) (energy threshold to change the charge state of a negatively charged SiDB to a neutrally charged SiDB)
        quickexact_parameter = quickexact_params()
        quickexact_parameter.simulation_parameters = physical_parameters

        # input 00
        and_gate.assign_cell_type(
            offset_coordinate(2, 2), sidb_technology.cell_type.EMPTY
        )  # delete one SiDB of the input BDL pair
        and_gate.assign_cell_type(
            offset_coordinate(24, 2), sidb_technology.cell_type.EMPTY
        )  # delete one SiDB of the input BDL pair

        self.assertTrue(len(quickexact(and_gate, quickexact_parameter).charge_distributions) > 0)

        and_gate.assign_cell_type(
            offset_coordinate(2, 2), sidb_technology.cell_type.INPUT
        )  # add SiDB of the input BDL pair again to have the original layout
        and_gate.assign_cell_type(
            offset_coordinate(24, 2), sidb_technology.cell_type.INPUT
        )  # add SiDB of the input BDL pair again to have the original layout

        # input 01
        and_gate.assign_cell_type(offset_coordinate(2, 2), sidb_technology.cell_type.EMPTY)
        and_gate.assign_cell_type(offset_coordinate(26, 0), sidb_technology.cell_type.EMPTY)

        self.assertTrue(len(quickexact(and_gate, quickexact_parameter).charge_distributions) > 0)

        and_gate.assign_cell_type(offset_coordinate(2, 2), sidb_technology.cell_type.INPUT)
        and_gate.assign_cell_type(offset_coordinate(26, 0), sidb_technology.cell_type.INPUT)

        # input 10
        and_gate.assign_cell_type(offset_coordinate(0, 0), sidb_technology.cell_type.EMPTY)
        and_gate.assign_cell_type(offset_coordinate(24, 2), sidb_technology.cell_type.EMPTY)

        self.assertTrue(len(quickexact(and_gate, quickexact_parameter).charge_distributions) > 0)

        and_gate.assign_cell_type(offset_coordinate(0, 0), sidb_technology.cell_type.INPUT)
        and_gate.assign_cell_type(offset_coordinate(24, 2), sidb_technology.cell_type.INPUT)

        # input 11
        and_gate.assign_cell_type(offset_coordinate(0, 0), sidb_technology.cell_type.EMPTY)
        and_gate.assign_cell_type(offset_coordinate(26, 0), sidb_technology.cell_type.EMPTY)

        self.assertTrue(len(quickexact(and_gate, quickexact_parameter).charge_distributions) > 0)

        and_gate.assign_cell_type(offset_coordinate(0, 0), sidb_technology.cell_type.INPUT)
        and_gate.assign_cell_type(offset_coordinate(26, 0), sidb_technology.cell_type.INPUT)


if __name__ == "__main__":
    unittest.main()
