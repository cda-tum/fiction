import os
import unittest

from mnt.pyfiction import (
    create_and_tt,
    create_xor_tt,
    critical_temperature_domain,
    critical_temperature_domain_contour_tracing,
    critical_temperature_domain_flood_fill,
    critical_temperature_domain_grid_search,
    critical_temperature_domain_random_sampling,
    operational_domain,
    operational_domain_contour_tracing,
    operational_domain_flood_fill,
    operational_domain_grid_search,
    operational_domain_params,
    operational_domain_random_sampling,
    operational_domain_stats,
    operational_domain_value_range,
    operational_status,
    parameter_point,
    read_sqd_layout_100,
    read_sqd_layout_111,
    sidb_simulation_engine,
    sweep_parameter,
)

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestOperationalDomain(unittest.TestCase):
    def test_xor_gate_100_lattice(self):
        lyt = read_sqd_layout_100(dir_path + "/../../../resources/hex_21_inputsdbp_xor_v1.sqd")

        params = operational_domain_params()
        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
        params.operational_params.simulation_parameters.base = 2

        params.sweep_dimensions = [
            operational_domain_value_range(sweep_parameter.EPSILON_R, 5.55, 5.65, 0.01),
            operational_domain_value_range(sweep_parameter.LAMBDA_TF, 4.95, 5.05, 0.01),
        ]

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

    def test_critical_temperature_domain_xor_gate_100_lattice(self):
        lyt = read_sqd_layout_100(dir_path + "/../../../resources/hex_21_inputsdbp_xor_v1.sqd")

        params = operational_domain_params()
        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
        params.operational_params.simulation_parameters.base = 2

        params.sweep_dimensions = [
            operational_domain_value_range(sweep_parameter.EPSILON_R, 5.55, 5.65, 0.01),
            operational_domain_value_range(sweep_parameter.LAMBDA_TF, 4.95, 5.05, 0.01),
        ]

        stats_grid = operational_domain_stats()
        critical_temperature_domain_grid_search(lyt, [create_xor_tt()], params, stats_grid)
        self.assertGreater(stats_grid.num_operational_parameter_combinations, 0)

        stats_flood_fill = operational_domain_stats()
        critical_temperature_domain_flood_fill(lyt, [create_xor_tt()], 100, params, stats_flood_fill)
        self.assertGreater(stats_flood_fill.num_operational_parameter_combinations, 0)

        stats_random_sampling = operational_domain_stats()
        critical_temperature_domain_random_sampling(lyt, [create_xor_tt()], 100, params, stats_random_sampling)
        self.assertGreater(stats_random_sampling.num_operational_parameter_combinations, 0)

        stats_contour_tracing = operational_domain_stats()
        critical_temperature_domain_contour_tracing(lyt, [create_xor_tt()], 100, params, stats_contour_tracing)
        self.assertGreater(stats_contour_tracing.num_operational_parameter_combinations, 0)

    def test_and_gate_111_lattice(self):
        lyt = read_sqd_layout_111(dir_path + "/../../../resources/AND_mu_032_111_surface.sqd")

        params = operational_domain_params()
        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
        params.operational_params.simulation_parameters.base = 2

        params.sweep_dimensions = [
            operational_domain_value_range(sweep_parameter.EPSILON_R, 5.60, 5.64, 0.01),
            operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.00, 5.01, 0.01),
        ]

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

    def test_temperature_operational_domain(self):
        # Create an instance of temperature_operational_domain
        temp_domain = critical_temperature_domain()

        # Create test key and value
        key = parameter_point([1.0, 2.0, 3.0])  # Example parameter point
        value = (operational_status.OPERATIONAL, 0.1)  # Example tuple value

        # Add a value to the domain
        temp_domain.add_value(key, value)

        self.assertEqual(temp_domain.contains(key), value)

        # Test retrieving a value that doesn't exist
        missing_key = parameter_point([4.0, 5.0, 6.0])
        self.assertIsNone(temp_domain.contains(missing_key))

        # Modify dimensions and verify
        temp_domain.set_dimensions([sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])
        self.assertEqual(temp_domain.get_dimensions(), [sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])

    def test_operational_domain(self):
        # Create an instance of operational_domain
        op_domain = operational_domain()

        # Create test key and value
        key = parameter_point([10.0, 20.0, 30.0])  # Example parameter point
        value = (operational_status.NON_OPERATIONAL,)  # Example tuple value

        # Add a value to the domain
        op_domain.add_value(key, value)

        # Test retrieving a value that doesn't exist
        missing_key = parameter_point([7.0, 8.0, 9.0])
        self.assertIsNone(op_domain.contains(missing_key))

        # Modify dimensions and verify
        op_domain.set_dimensions([sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])
        self.assertEqual(op_domain.get_dimensions(), [sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])


if __name__ == "__main__":
    unittest.main()
