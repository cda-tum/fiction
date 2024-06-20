from mnt.pyfiction import *
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestOperationalDomain(unittest.TestCase):

    def test_xor_gate_100_lattice(self):
        lyt = read_sqd_layout_100(dir_path + "/../../../resources/hex_21_inputsdbp_xor_v1.sqd")

        params = operational_domain_params()
        params.sim_engine = sidb_simulation_engine.QUICKEXACT
        params.simulation_parameters.base = 2
        params.x_dimension = sweep_parameter.EPSILON_R
        params.y_dimension = sweep_parameter.LAMBDA_TF
        params.x_min = 5.55
        params.x_max = 5.65
        params.y_min = 4.95
        params.y_max = 5.05
        params.x_step = 0.01
        params.y_step = 0.01

        stats_grid = operational_domain_stats()
        opdomain = operational_domain_grid_search(lyt, [create_xor_tt()], params, stats_grid)
        self.assertGreater(stats_grid.num_operational_parameter_combinations, 0)

        stats_flood_fill = operational_domain_stats()
        opdomain = operational_domain_flood_fill(lyt, [create_xor_tt()], 100, params, stats_flood_fill)
        self.assertGreater(stats_flood_fill.num_operational_parameter_combinations, 0)

        stats_random_sampling = operational_domain_stats()
        opdomain = operational_domain_random_sampling(lyt, [create_xor_tt()], 100, params, stats_random_sampling)
        self.assertGreater(stats_random_sampling.num_operational_parameter_combinations, 0)

        stats_contour_tracing = operational_domain_stats()
        opdomain = operational_domain_contour_tracing(lyt, [create_xor_tt()], 100, params, stats_contour_tracing)
        self.assertGreater(stats_contour_tracing.num_operational_parameter_combinations, 0)

    # def test_and_gate_111_lattice(self):
    #     lyt = read_sqd_layout_111(dir_path + "/../../../resources/AND_mu_032_0.sqd")
    #
    #     params = operational_domain_params()
    #
    #     params.sim_engine = sidb_simulation_engine.QUICKEXACT
    #     params.simulation_parameters.base = 2
    #     params.x_dimension = sweep_parameter.EPSILON_R
    #     params.y_dimension = sweep_parameter.LAMBDA_TF
    #     params.x_min = 5.60
    #     params.x_max = 5.64
    #     params.x_step = 0.01
    #     params.y_min = 5.00
    #     params.y_max = 5.01
    #     params.y_step = 0.01
    #
    #     stats_grid = operational_domain_stats()
    #     opdomain = operational_domain_grid_search(lyt, [create_and_tt()], params, stats_grid)
    #     self.assertGreater(stats_grid.num_operational_parameter_combinations, 0)
    #
    #     stats_flood_fill = operational_domain_stats()
    #     opdomain = operational_domain_flood_fill(lyt, [create_and_tt()], 100, params, stats_flood_fill)
    #     self.assertGreater(stats_flood_fill.num_operational_parameter_combinations, 0)
    #
    #     stats_random_sampling = operational_domain_stats()
    #     opdomain = operational_domain_random_sampling(lyt, [create_and_tt()], 100, params, stats_random_sampling)
    #     self.assertGreater(stats_random_sampling.num_operational_parameter_combinations, 0)
    #
    #     stats_contour_tracing = operational_domain_stats()
    #     opdomain = operational_domain_contour_tracing(lyt, [create_and_tt()], 100, params, stats_contour_tracing)
    #     self.assertGreater(stats_contour_tracing.num_operational_parameter_combinations, 0)


if __name__ == '__main__':
    unittest.main()
