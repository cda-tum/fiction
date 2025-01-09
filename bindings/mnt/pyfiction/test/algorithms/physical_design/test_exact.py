import os
import unittest

from mnt.pyfiction import (
    eq_type,
    equivalence_checking,
    exact_cartesian,
    exact_hexagonal,
    exact_params,
    exact_stats,
    read_technology_network,
)

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestExactCartesianPhysicalDesign(unittest.TestCase):
    def test_exact_default(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")
        layout = exact_cartesian(network)
        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

    def test_exact_with_parameters(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        params = exact_params()
        params.border_io = True
        params.crossings = True
        params.scheme = "ESR"

        layout = exact_cartesian(network, params)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

    def test_exact_with_stats(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        stats = exact_stats()

        layout = exact_cartesian(network, statistics=stats)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)


class TestExactHexagonalPhysicalDesign(unittest.TestCase):
    def test_exact_default(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")
        layout = exact_hexagonal(network)
        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

    def test_exact_with_parameters(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        params = exact_params()
        params.border_io = True
        params.crossings = True
        params.scheme = "ESR"

        layout = exact_hexagonal(network, params)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

    def test_exact_with_stats(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        stats = exact_stats()

        layout = exact_hexagonal(network, statistics=stats)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)


if __name__ == "__main__":
    unittest.main()
