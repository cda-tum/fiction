import os
import unittest

from mnt.pyfiction import (read_sqd_layout_100, operational_domain_params, sidb_simulation_engine,
                           operational_domain_value_range, sweep_parameter, operational_domain_stats,
                           operational_domain_grid_search, create_xor_tt, operational_domain_flood_fill,
                           operational_domain_random_sampling, operational_domain_contour_tracing, read_sqd_layout_111,
                           create_and_tt)

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestOperationalDomain(unittest.TestCase):

    def test_xor_gate_100_lattice(self):
        lyt = read_sqd_layout_100(dir_path + "/../../../resources/hex_21_inputsdbp_xor_v1.sqd")

        params = operational_domain_params()
        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
        params.operational_params.simulation_parameters.base = 2

        params.sweep_dimensions = [operational_domain_value_range(sweep_parameter.EPSILON_R, 5.55, 5.65, 0.01),
                                   operational_domain_value_range(sweep_parameter.LAMBDA_TF, 4.95, 5.05, 0.01)]

        stats_grid = operational_domain_stats()
        operational_domain_grid_search(lyt, [create_xor_tt()], params, stats_grid)
        self.assertGreater(stats_grid.num_operational_parameter_combinations, 0)

        stats_flood_fill = operational_domain_stats()
        operational_domain_flood_fill(lyt, [create_xor_tt()], 100, params, stats_flood_fill)
        self.assertGreater(stats_flood_fill.num_operational_parameter_combinations, 0)

        stats_random_sampling = operational_domain_stats()
        operational_domain_random_sampling(lyt, [create_xor_tt()], 100, params, stats_random_sampling)
        self.assertGreater(stats_random_sampling.num_operational_parameter_combinations, 0)

        stats_contour_tracing = operational_domain_stats()
        operational_domain_contour_tracing(lyt, [create_xor_tt()], 100, params, stats_contour_tracing)
        self.assertGreater(stats_contour_tracing.num_operational_parameter_combinations, 0)

    def test_and_gate_111_lattice(self):
        lyt = read_sqd_layout_111(dir_path + "/../../../resources/AND_mu_032_111_surface.sqd")

        params = operational_domain_params()
        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
        params.operational_params.simulation_parameters.base = 2

        params.sweep_dimensions = [operational_domain_value_range(sweep_parameter.EPSILON_R, 5.60, 5.64, 0.01),
                                   operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.00, 5.01, 0.01)]

        stats_grid = operational_domain_stats()
        operational_domain_grid_search(lyt, [create_and_tt()], params, stats_grid)
        self.assertGreater(stats_grid.num_operational_parameter_combinations, 0)

        stats_flood_fill = operational_domain_stats()
        operational_domain_flood_fill(lyt, [create_and_tt()], 100, params, stats_flood_fill)
        self.assertGreater(stats_flood_fill.num_operational_parameter_combinations, 0)

        stats_random_sampling = operational_domain_stats()
        operational_domain_random_sampling(lyt, [create_and_tt()], 100, params, stats_random_sampling)
        self.assertGreater(stats_random_sampling.num_operational_parameter_combinations, 0)

        stats_contour_tracing = operational_domain_stats()
        operational_domain_contour_tracing(lyt, [create_and_tt()], 100, params, stats_contour_tracing)
        self.assertGreater(stats_contour_tracing.num_operational_parameter_combinations, 0)


if __name__ == '__main__':
    unittest.main()
