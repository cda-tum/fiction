from fiction.pyfiction import *
import unittest
import os


class TestQuicksim(unittest.TestCase):

    def test_perturber_and_DB_pair(self):
        layout = sidb_layout((10, 10))
        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((4, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((6, 1), sidb_technology.cell_type.NORMAL)

        params = quicksim_params()
        params.phys_params = sidb_simulation_parameters()
        params.iteration_steps = 80
        params.alpha = 0.7
        self.assertEqual(params.iteration_steps, 80)
        self.assertEqual(params.alpha, 0.7)

        params_one = quicksim_params()
        params_one.iteration_steps = 50
        params_one.alpha = 0.4
        params_one.number_threads = 1
        self.assertEqual(params_one.iteration_steps, 50)
        self.assertEqual(params_one.alpha, 0.4)
        self.assertEqual(params_one.number_threads, 1)

        result = quicksim(layout, params)

        self.assertEqual(result.algorithm_name, "quicksim")
        self.assertEqual(len(result.charge_distributions), 1)

        groundstate = result.charge_distributions[0]

        self.assertEqual(groundstate.get_charge_state(0, 0, 1), sidb_charge_state.NEGATIVE)
        self.assertEqual(groundstate.get_charge_state(4, 0, 1), sidb_charge_state.NEUTRAL)
        self.assertEqual(groundstate.get_charge_state(6, 0, 1), sidb_charge_state.NEGATIVE)


if __name__ == '__main__':
    unittest.main()
