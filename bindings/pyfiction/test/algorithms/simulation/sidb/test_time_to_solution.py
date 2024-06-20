from mnt.pyfiction import *
import unittest
import os

class TestTimeToSolution(unittest.TestCase):

    def test_one_sidb_100_lattice(self):
        layout = sidb_100_lattice((0, 0))
        layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)

        quicksim_parameter = quicksim_params()
        quicksim_parameter.simulation_parameters = sidb_simulation_parameters(3, -0.3)

        tts_params = time_to_solution_params()
        tts_params.engine = exhaustive_sidb_simulation_engine.QUICKEXACT
        stats = time_to_solution_stats()
        #
        # cds = charge_distribution_surface_100(layout)
        #
        # time_to_solution(cds, quicksim_parameter, tts_params, stats)
        #
        # self.assertEqual(stats.acc, 100)
        # self.assertGreater(stats.time_to_solution, 0.0)
        # self.assertGreater(stats.mean_single_runtime, 0.0)

    # def test_one_DBs_111_lattice(self):
    #     layout = sidb_111_lattice((0, 0))
    #     layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
    #
    #     quicksim_parameter = quicksim_params()
    #     quicksim_parameter.simulation_parameters = sidb_simulation_parameters(3, -0.3)
    #
    #     tts_params = time_to_solution_params()
    #     tts_params.engine = exhaustive_sidb_simulation_engine.QUICKEXACT
    #     stats = time_to_solution_stats()
    #
    #     cds = charge_distribution_surface_111(layout)
    #
    #     time_to_solution(cds, quicksim_parameter, tts_params, stats)
    #
    #     self.assertEqual(stats.acc, 100)
    #     self.assertGreater(stats.time_to_solution, 0.0)
    #     self.assertGreater(stats.mean_single_runtime, 0.0)

if __name__ == '__main__':
    unittest.main()
