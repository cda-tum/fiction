from mnt.pyfiction import read_technology_network, is_balanced, network_balancing, network_balancing_params
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestNetworkBalancing(unittest.TestCase):
    def test_balancing(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")
        self.assertEqual(network.num_gates(), 5)

        self.assertFalse(is_balanced(network))

        balanced_default = network_balancing(network)
        self.assertEqual(balanced_default.num_gates(), 7)
        self.assertTrue(is_balanced(balanced_default))

        unify_params = network_balancing_params()
        unify_params.unify_outputs = True
        balanced_unify = network_balancing(network, unify_params)
        self.assertEqual(balanced_unify.num_gates(), 7)
        self.assertTrue(is_balanced(balanced_unify, unify_params))


if __name__ == '__main__':
    unittest.main()
