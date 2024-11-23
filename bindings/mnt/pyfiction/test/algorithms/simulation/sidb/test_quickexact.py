from mnt.pyfiction import *
import unittest
import os

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

        groundstate = determine_groundstate_from_simulation_results(result)

        self.assertEqual(len(groundstate), 1)

        self.assertEqual(groundstate[0].get_charge_state((0, 0)), sidb_charge_state.NEGATIVE)
        self.assertEqual(groundstate[0].get_charge_state((1, 0)), sidb_charge_state.NEUTRAL)
        self.assertEqual(groundstate[0].get_charge_state((2, 0)), sidb_charge_state.NEUTRAL)
        self.assertEqual(groundstate[0].get_charge_state((3, 0)), sidb_charge_state.NEGATIVE)


if __name__ == '__main__':
    unittest.main()
