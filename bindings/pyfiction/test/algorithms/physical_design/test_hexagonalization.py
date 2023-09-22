from fiction.pyfiction import *
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestHexagonalization(unittest.TestCase):

    def test_hexagonalization(self):
        network = read_logic_network(dir_path + "/../../resources/mux21.v")
        layout = orthogonal(network)
        hex_layout = hexagonalization(layout)
        self.assertEqual(equivalence_checking(layout, hex_layout), eq_type.STRONG)
