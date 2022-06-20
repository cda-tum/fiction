from fiction import pyfiction
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestApplyGateLibrary(unittest.TestCase):

    def test_apply_qca_one_library(self):
        network = pyfiction.read_logic_network(dir_path + "/../../ressources/mux21.v")
        layout = pyfiction.orthogonal(network)

        cell_layout = pyfiction.apply_qca_one_library(layout)

    def test_apply_bestagon_library(self):
        network = pyfiction.read_logic_network(dir_path + "/../../ressources/mux21.v")

        params = pyfiction.exact_params()
        params.scheme = "ROW"
        params.crossings = True
        params.border_io = True

        layout = pyfiction.exact_hexagonal(network, params)

        cell_layout = pyfiction.apply_bestagon_library(layout)
