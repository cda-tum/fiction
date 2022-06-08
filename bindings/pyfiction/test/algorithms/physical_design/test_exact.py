from fiction import pyfiction
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestExactPhysicalDesign(unittest.TestCase):

    def test_exact_default(self):
        network = pyfiction.read_logic_network(dir_path + "/../../ressources/mux21.v")
        layout = pyfiction.exact(network)

    def test_exact_with_parameters(self):
        network = pyfiction.read_logic_network(dir_path + "/../../ressources/mux21.v")

        params = pyfiction.exact_params()
        params.border_io = True
        params.crossings = True
        params.scheme = "ESP"

        layout = pyfiction.exact(network, params)

    def test_exact_with_stats(self):
        network = pyfiction.read_logic_network(dir_path + "/../../ressources/mux21.v")

        stats = pyfiction.exact_stats()

        layout = pyfiction.exact(network, statistics=stats)
