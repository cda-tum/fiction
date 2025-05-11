import os
import unittest

from mnt.pyfiction import (
    create_and_tt,
    create_id_tt,
    create_xor_tt,
    critical_temperature_domain,
    critical_temperature_domain_contour_tracing,
    critical_temperature_domain_flood_fill,
    critical_temperature_domain_grid_search,
    critical_temperature_domain_random_sampling,
    input_bdl_configuration,
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
    sidb_100_lattice,
    sidb_simulation_engine,
    sidb_technology,
    sweep_parameter,
)

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestOperationalDomain(unittest.TestCase):
    def test_operational_domain_XOR_gate_100_lattice(self):
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

    def test_critical_temperature_domain_XOR_gate_100_lattice(self):
        lyt = read_sqd_layout_100(dir_path + "/../../../resources/hex_21_inputsdbp_xor_v1.sqd")

        params = operational_domain_params()
        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
        params.operational_params.simulation_parameters.base = 2

        params.sweep_dimensions = [
            operational_domain_value_range(sweep_parameter.EPSILON_R, 5.55, 5.65, 0.01),
            operational_domain_value_range(sweep_parameter.LAMBDA_TF, 4.95, 5.05, 0.01),
        ]

        stats_grid = operational_domain_stats()
        ct_domain_grid = critical_temperature_domain_grid_search(lyt, [create_xor_tt()], params, stats_grid)
        self.assertEqual(ct_domain_grid.contains(parameter_point([5.60, 5.00]))[0], operational_status.OPERATIONAL)
        self.assertGreater(ct_domain_grid.contains(parameter_point([5.60, 5.00]))[1], 30)
        self.assertGreater(stats_grid.num_operational_parameter_combinations, 0)
        self.assertGreater(ct_domain_grid.minimum_ct(), 23)
        self.assertLess(ct_domain_grid.maximum_ct(), 38)

        stats_flood_fill = operational_domain_stats()
        ct_domain_flood = critical_temperature_domain_flood_fill(lyt, [create_xor_tt()], 100, params, stats_flood_fill)
        self.assertEqual(ct_domain_flood.contains(parameter_point([5.60, 5.00]))[0], operational_status.OPERATIONAL)
        self.assertGreater(ct_domain_flood.contains(parameter_point([5.60, 5.00]))[1], 30)
        self.assertGreater(stats_flood_fill.num_operational_parameter_combinations, 0)

        stats_contour_tracing = operational_domain_stats()
        ct_domain_contour = critical_temperature_domain_contour_tracing(
            lyt, [create_xor_tt()], 1000, params, stats_contour_tracing
        )
        self.assertEqual(ct_domain_contour.contains(parameter_point([5.60, 5.00]))[0], operational_status.OPERATIONAL)
        self.assertGreater(ct_domain_contour.contains(parameter_point([5.60, 5.00]))[1], 30)
        self.assertGreater(stats_contour_tracing.num_operational_parameter_combinations, 0)

        params.sweep_dimensions = [
            operational_domain_value_range(sweep_parameter.EPSILON_R, 5.60, 5.60, 0.01),
            operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.00, 5.00, 0.01),
        ]

        stats_random_sampling = operational_domain_stats()
        ct_domain_random = critical_temperature_domain_random_sampling(
            lyt, [create_xor_tt()], 1000, params, stats_random_sampling
        )
        self.assertEqual(ct_domain_random.contains(parameter_point([5.60, 5.00]))[0], operational_status.OPERATIONAL)
        self.assertGreater(ct_domain_random.contains(parameter_point([5.60, 5.00]))[1], 30)
        self.assertGreater(stats_random_sampling.num_operational_parameter_combinations, 0)

    def test_operational_domain_AND_gate_111_lattice(self):
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
        operational_domain_contour_tracing(lyt, [create_and_tt()], 1000, params, stats_contour_tracing)
        self.assertGreater(stats_contour_tracing.num_operational_parameter_combinations, 0)

    def test_temperature_operational_domain(self):
        # Create an instance of temperature_operational_domain
        temp_domain = critical_temperature_domain([sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])

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
        self.assertEqual(temp_domain.get_dimension(0), sweep_parameter.EPSILON_R)
        self.assertEqual(temp_domain.get_dimension(1), sweep_parameter.LAMBDA_TF)

    def test_operational_domain(self):
        # Create an instance of operational_domain
        op_domain = operational_domain([sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])

        # Create test key and value
        key = parameter_point([10.0, 20.0])
        value = operational_status.NON_OPERATIONAL

        # Add a value to the domain
        op_domain[key] = value

        # Test retrieving a value that doesn't exist
        missing_key = parameter_point([7.0, 8.0])
        self.assertNotIn(missing_key, op_domain)

        # Modify dimensions and verify
        self.assertEqual(op_domain.get_dimension(0), sweep_parameter.EPSILON_R)
        self.assertEqual(op_domain.get_dimension(1), sweep_parameter.LAMBDA_TF)

        # --- Begin dictionary-like API tests ---

        # __getitem__ should return the value for an existing key
        self.assertEqual(op_domain[key], value)

        # __getitem__ should raise KeyError for missing key
        with self.assertRaises(KeyError):
            _ = op_domain[missing_key]

        # __setitem__ should add or update a value
        new_key = parameter_point([1.1, 2.2])
        new_value = operational_status.OPERATIONAL
        op_domain[new_key] = operational_status.OPERATIONAL
        self.assertEqual(op_domain[new_key], new_value)

        # __contains__ should work for present and missing keys
        self.assertIn(key, op_domain)
        self.assertIn(new_key, op_domain)
        self.assertNotIn(missing_key, op_domain)

        # __len__ should reflect the number of items
        self.assertEqual(len(op_domain), 2)

        # __iter__ should yield all keys
        keys = list(iter(op_domain))
        self.assertIn(key, keys)
        self.assertIn(new_key, keys)
        self.assertEqual(set(keys), set(op_domain.keys()))

        # keys() should return all keys
        keys_method = op_domain.keys()
        self.assertEqual(set(keys_method), {key, new_key})

        # values() should return all values
        values_method = op_domain.values()
        self.assertIn(value, values_method)
        self.assertIn(new_value, values_method)
        self.assertEqual(len(values_method), 2)

        # items() should return all (key, value) pairs
        items_method = op_domain.items()
        self.assertIn((key, value), items_method)
        self.assertIn((new_key, new_value), items_method)
        self.assertEqual(len(items_method), 2)

        # --- End dictionary-like API tests ---

    def test_operational_domain_two_bdl_pair_wire(self):
        bdl_wire = sidb_100_lattice()

        bdl_wire.assign_cell_type((0, 0), sidb_technology.cell_type.INPUT)
        bdl_wire.assign_cell_type((2, 0), sidb_technology.cell_type.INPUT)

        bdl_wire.assign_cell_type((6, 0), sidb_technology.cell_type.NORMAL)
        bdl_wire.assign_cell_type((8, 0), sidb_technology.cell_type.NORMAL)

        bdl_wire.assign_cell_type((12, 0), sidb_technology.cell_type.OUTPUT)
        bdl_wire.assign_cell_type((14, 0), sidb_technology.cell_type.OUTPUT)

        bdl_wire.assign_cell_type((18, 0), sidb_technology.cell_type.NORMAL)

        params = operational_domain_params()
        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
        params.operational_params.simulation_parameters.base = 2
        params.operational_params.input_bdl_iterator_params.input_bdl_config = (
            input_bdl_configuration.PERTURBER_DISTANCE_ENCODED
        )

        params.sweep_dimensions = [
            operational_domain_value_range(sweep_parameter.EPSILON_R, 1.0, 10.0, 0.1),
            operational_domain_value_range(sweep_parameter.LAMBDA_TF, 1.0, 10.0, 0.1),
        ]

        stats_grid = operational_domain_stats()
        op_domain = operational_domain_grid_search(bdl_wire, [create_id_tt()], params, stats_grid)

        self.assertEqual(len(op_domain), 8281)

        self.assertEqual(stats_grid.num_simulator_invocations, 10034)
        self.assertEqual(stats_grid.num_evaluated_parameter_combinations, 8281)
        self.assertEqual(stats_grid.num_operational_parameter_combinations, 0)
        self.assertEqual(stats_grid.num_non_operational_parameter_combinations, 8281)


if __name__ == "__main__":
    unittest.main()
