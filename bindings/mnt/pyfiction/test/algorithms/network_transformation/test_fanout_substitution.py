from mnt.pyfiction import (
    read_technology_network,
    is_fanout_substituted,
    fanout_substitution,
    fanout_substitution_params,
    substitution_strategy,
)
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestFanoutSubstitution(unittest.TestCase):
    def test_substitution(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")
        self.assertEqual(network.num_gates(), 5)

        self.assertFalse(is_fanout_substituted(network))

        substituted_default = fanout_substitution(network)
        self.assertEqual(substituted_default.num_gates(), 6)
        self.assertTrue(is_fanout_substituted(substituted_default))

        depth_params = fanout_substitution_params()
        depth_params.strategy = substitution_strategy.DEPTH
        substituted_depth = fanout_substitution(network, depth_params)
        self.assertEqual(substituted_depth.num_gates(), 6)
        self.assertTrue(is_fanout_substituted(substituted_depth, depth_params))

        breadth_params = fanout_substitution_params()
        breadth_params.strategy = substitution_strategy.BREADTH
        substituted_breadth = fanout_substitution(network, breadth_params)
        self.assertEqual(substituted_breadth.num_gates(), 6)
        self.assertTrue(is_fanout_substituted(substituted_breadth, breadth_params))

        threshold_params = fanout_substitution_params()
        threshold_params.threshold = 2
        substituted_threshold = fanout_substitution(network, threshold_params)
        self.assertEqual(substituted_threshold.num_gates(), 5)
        self.assertTrue(is_fanout_substituted(substituted_threshold, threshold_params))
        self.assertFalse(is_fanout_substituted(substituted_threshold, depth_params))
        self.assertFalse(is_fanout_substituted(substituted_threshold, breadth_params))


if __name__ == "__main__":
    unittest.main()
