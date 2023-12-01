from mnt.pyfiction import *
import unittest
import os


class TestQuickExact(unittest.TestCase):

    def test_three_DBs(self):
        layout = sidb_layout((2, 1))
        layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((1, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((2, 0), sidb_technology.cell_type.NORMAL)

        params = quickexact_params()
        params.simulation_parameters.base = 2
        params.simulation_parameters.mu_minus = -0.25
        params.base_number_detection = automatic_base_number_detection.OFF
        self.assertEqual(params.simulation_parameters.mu_minus, -0.25)
        self.assertEqual(params.base_number_detection, automatic_base_number_detection.OFF)

        cds = charge_distribution_surface(layout)

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


if __name__ == '__main__':
    unittest.main()
