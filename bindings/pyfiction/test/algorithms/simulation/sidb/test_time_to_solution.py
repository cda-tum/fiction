from mnt.pyfiction import *
import unittest
import math

class TestTimeToSolution(unittest.TestCase):

    def test_one_sidb_100_lattice(self):
        layout = sidb_100_lattice((0, 0))
        layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)

        quicksim_parameter = quicksim_params()
        quicksim_parameter.simulation_parameters = sidb_simulation_parameters(3, -0.3)

        tts_params = time_to_solution_params()
        tts_params.engine = exact_sidb_simulation_engine.QUICKEXACT
        stats = time_to_solution_stats()

        cds = charge_distribution_surface_100(layout)

        time_to_solution(cds, quicksim_parameter, tts_params, stats)

        self.assertEqual(stats.acc, 100)
        self.assertGreater(stats.time_to_solution, 0.0)
        self.assertGreater(stats.mean_single_runtime, 0.0)

    def test_one_DBs_111_lattice(self):
        layout = sidb_111_lattice((0, 0))
        layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)

        quicksim_parameter = quicksim_params()
        quicksim_parameter.simulation_parameters = sidb_simulation_parameters(3, -0.3)

        tts_params = time_to_solution_params()
        tts_params.engine = exact_sidb_simulation_engine.QUICKEXACT
        stats = time_to_solution_stats()

        cds = charge_distribution_surface_111(layout)

        time_to_solution(cds, quicksim_parameter, tts_params, stats)

        self.assertEqual(stats.acc, 100)
        self.assertGreater(stats.time_to_solution, 0.0)
        self.assertGreater(stats.mean_single_runtime, 0.0)

    def test_time_to_solution_with_simulation_results(self):
        layout = sidb_100_lattice((0, 0))

        # Assign SiDBs to the layout
        layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((1, 6, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((3, 6, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((5, 6, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((10, 6, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((15, 6, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((18, 6, 0), sidb_technology.cell_type.NORMAL)

        # Define simulation parameters
        params = sidb_simulation_parameters(2, -0.32)
        quicksim_params_inst = quicksim_params()
        quicksim_params_inst.simulation_parameters = params

        number_of_repetitions = 100
        simulation_results_quicksim = []

        # Run the quicksim simulations
        for _ in range(number_of_repetitions):
            simulation_results_quicksim.append(quicksim(layout, quicksim_params_inst))


        quickexact_params_inst = quickexact_params()
        quickexact_params_inst.simulation_parameters = params
        quickexact_params_inst.base_number_detection = automatic_base_number_detection.OFF
        self.assertEqual(quickexact_params_inst.simulation_parameters.mu_minus, -0.32)
        self.assertEqual(quickexact_params_inst.base_number_detection, automatic_base_number_detection.OFF)

        # Run the quickexact simulation
        simulation_results_quickexact = quickexact(layout, quickexact_params_inst)

        # Calculate time-to-solution using the simulation results
        st = time_to_solution_stats()
        time_to_solution_for_given_simulation_results(
            simulation_results_quickexact, simulation_results_quicksim, 0.997, st)

        # Assertions
        self.assertEqual(st.acc, 100.0)
        self.assertGreater(st.time_to_solution, 0.0)
        self.assertGreater(st.mean_single_runtime, 0.0)

        print(st.acc)

        if st.acc == 100:
                tts_calculated = st.mean_single_runtime
        else:
            # To avoid division by zero, ensure st.acc is not 1.0
            tts_calculated = (st.mean_single_runtime * math.log(1.0 - 0.997) /
                      math.log(1.0 - st.acc))

        self.assertAlmostEqual(st.time_to_solution - tts_calculated, 0.0, delta=1e-6)



if __name__ == '__main__':
    unittest.main()
