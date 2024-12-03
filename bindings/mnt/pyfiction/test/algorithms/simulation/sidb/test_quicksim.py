import unittest

from mnt.pyfiction import (
    charge_distribution_surface,
    charge_distribution_surface_111,
    determine_groundstate_from_simulation_results,
    quicksim,
    quicksim_params,
    sidb_100_lattice,
    sidb_111_lattice,
    sidb_charge_state,
    sidb_simulation_parameters,
    sidb_technology,
)


class TestQuicksim(unittest.TestCase):
    def test_perturber_and_sidb_pair(self):
        layout = sidb_100_lattice((10, 10))
        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((4, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((6, 1), sidb_technology.cell_type.NORMAL)

        params = quicksim_params()
        params.simulation_parameters = sidb_simulation_parameters()
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

        charge_distribution_surface(layout)

        result = quicksim(layout, params)

        self.assertEqual(result.algorithm_name, "QuickSim")
        self.assertLessEqual(len(result.charge_distributions), 80)

        groundstate = result.charge_distributions[0]

        self.assertEqual(groundstate.get_charge_state((0, 1)), sidb_charge_state.NEGATIVE)
        self.assertEqual(groundstate.get_charge_state((4, 1)), sidb_charge_state.NEUTRAL)
        self.assertEqual(groundstate.get_charge_state((6, 1)), sidb_charge_state.NEGATIVE)

    def test_perturber_and_sidb_pair_111(self):
        layout = sidb_111_lattice((4, 1))
        layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((1, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((2, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((3, 0), sidb_technology.cell_type.NORMAL)

        params = quicksim_params()
        params.simulation_parameters = sidb_simulation_parameters()
        params.simulation_parameters.mu_minus = -0.32
        params.iteration_steps = 80
        params.alpha = 0.7
        self.assertEqual(params.iteration_steps, 80)
        self.assertEqual(params.alpha, 0.7)
        self.assertEqual(params.simulation_parameters.mu_minus, -0.32)

        charge_distribution_surface_111(layout)

        result = quicksim(layout, params)

        self.assertEqual(result.algorithm_name, "QuickSim")

        groundstate = determine_groundstate_from_simulation_results(result)

        self.assertEqual(len(groundstate), 1)

        self.assertEqual(groundstate[0].get_charge_state((0, 0)), sidb_charge_state.NEGATIVE)
        self.assertEqual(groundstate[0].get_charge_state((1, 0)), sidb_charge_state.NEUTRAL)
        self.assertEqual(groundstate[0].get_charge_state((2, 0)), sidb_charge_state.NEUTRAL)
        self.assertEqual(groundstate[0].get_charge_state((3, 0)), sidb_charge_state.NEGATIVE)


if __name__ == "__main__":
    unittest.main()
