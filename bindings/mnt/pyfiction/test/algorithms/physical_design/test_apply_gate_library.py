from mnt.pyfiction import (
    read_technology_network,
    orthogonal,
    apply_qca_one_library,
    exact_params,
    exact_hexagonal,
    apply_bestagon_library,
    technology_constraints,
    exact_shifted_cartesian,
    apply_topolinano_library,
)
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestApplyGateLibrary(unittest.TestCase):
    def test_apply_qca_one_library(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")
        layout = orthogonal(network)

        apply_qca_one_library(layout)

    def test_apply_bestagon_library(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        params = exact_params()
        params.scheme = "ROW"
        params.crossings = True
        params.border_io = True

        layout = exact_hexagonal(network, params)

        apply_bestagon_library(layout)

    def test_apply_topolinano_library(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        params = exact_params()
        params.scheme = "COLUMNAR3"
        params.crossings = True
        params.border_io = True
        params.technology_specifics = technology_constraints.TOPOLINANO

        layout = exact_shifted_cartesian(network, params)

        apply_topolinano_library(layout)


if __name__ == "__main__":
    unittest.main()
