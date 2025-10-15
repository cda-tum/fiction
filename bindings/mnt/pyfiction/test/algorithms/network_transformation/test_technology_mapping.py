import os
import unittest

from mnt.pyfiction import (
    all_standard_2_input_functions,
    all_standard_3_input_functions,
    all_supported_standard_functions,
    and_or_not,
    and_or_not_maj,
    eq_type,
    equivalence_checking,
    read_technology_network,
    technology_mapping,
    technology_mapping_params,
    technology_mapping_stats,
)

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestTechnologyMapping(unittest.TestCase):
    def test_mapping_default(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")
        self.assertEqual(network.num_gates(), 5)

        mapped_network = technology_mapping(network)

        self.assertEqual(equivalence_checking(network, mapped_network), eq_type.NO)

    def test_mapping_with_parameters(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        for params in [
            and_or_not(),
            and_or_not_maj(),
            all_standard_2_input_functions(),
            all_standard_3_input_functions(),
            all_supported_standard_functions(),
        ]:
            mapped_network = technology_mapping(network, params)

            self.assertEqual(equivalence_checking(network, mapped_network), eq_type.STRONG)

    def test_mapping_with_stats(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        for params in [
            and_or_not(),
            and_or_not_maj(),
            all_standard_2_input_functions(),
            all_standard_3_input_functions(),
            all_supported_standard_functions(),
        ]:
            stats = technology_mapping_stats()
            mapped_network = technology_mapping(network, params=params, stats=stats)

            self.assertEqual(equivalence_checking(network, mapped_network), eq_type.STRONG)

    def test_missing_required_gates_exception(self):
        """Test that missing required gates throw an exception in Python."""
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        # Create params with no gates enabled (will fail for any network type)
        params = technology_mapping_params()

        # Should raise an exception
        with self.assertRaises(RuntimeError) as context:
            technology_mapping(network, params)

        # Check that the error message mentions missing gates
        self.assertIn("missing required gates", str(context.exception).lower())


if __name__ == "__main__":
    unittest.main()
