from fiction.pyfiction import *
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestTechnologyMapping(unittest.TestCase):
    def test_technology_mapping(self):
        network = read_logic_network(dir_path + "/../../resources/mux21.v")
        self.assertEqual(network.num_gates(), 5)

        params = technology_mapping_params()
        params.inv = True
        params.and2 = True
        params.or2 = True
        mapped_network = technology_mapping(network, params)
        self.assertEqual(mapped_network.num_gates(), 4)
        self.assertEqual(equivalence_checking(network, mapped_network), eq_type.STRONG)
        for gate in mapped_network.gates():
            self.assertTrue(network.is_and(gate) | network.is_or(gate) | network.is_inv(gate))

    def test_technology_mapping_and_or_not(self):
        network = read_logic_network(dir_path + "/../../resources/mux21.v")
        self.assertEqual(network.num_gates(), 5)

        params = and_or_not()
        mapped_network = technology_mapping(network, params, technology_mapping_stats())
        self.assertEqual(mapped_network.num_gates(), 4)
        self.assertEqual(equivalence_checking(network, mapped_network), eq_type.STRONG)
        for gate in mapped_network.gates():
            self.assertTrue(network.is_and(gate) | network.is_or(gate) | network.is_inv(gate))


if __name__ == '__main__':
    unittest.main()
