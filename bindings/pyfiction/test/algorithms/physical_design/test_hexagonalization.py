from mnt.pyfiction import *
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestHexagonalization(unittest.TestCase):

    def test_hexagonalization_default(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")
        cart_layout = orthogonal(network)
        self.assertEqual(equivalence_checking(network, cart_layout), eq_type.STRONG)
        hex_layout = hexagonalization(cart_layout)
        self.assertEqual(equivalence_checking(network, hex_layout), eq_type.STRONG)
        self.assertEqual(equivalence_checking(cart_layout, hex_layout), eq_type.STRONG)


if __name__ == '__main__':
    unittest.main()
