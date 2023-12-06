from mnt.pyfiction import *
from dotenv import load_dotenv
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))
load_dotenv()
z3 = os.environ.get("z3", "OFF")


class TestApplyGateLibrary(unittest.TestCase):

    def test_apply_qca_one_library(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")
        layout = orthogonal(network)

        cell_layout = apply_qca_one_library(layout)

    @unittest.skipIf(z3 == "OFF", "Z3 not enabled")
    def test_apply_bestagon_library(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        params = exact_params()
        params.scheme = "ROW"
        params.crossings = True
        params.border_io = True

        layout = exact_hexagonal(network, params)

        cell_layout = apply_bestagon_library(layout)

    @unittest.skipIf(z3 == "OFF", "Z3 not enabled")
    def test_apply_topolinano_library(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        params = exact_params()
        params.scheme = "COLUMNAR3"
        params.crossings = True
        params.border_io = True
        params.technology_specifics = technology_constraints.TOPOLINANO

        layout = exact_shifted_cartesian(network, params)

        cell_layout = apply_topolinano_library(layout)


if __name__ == '__main__':
    unittest.main()
