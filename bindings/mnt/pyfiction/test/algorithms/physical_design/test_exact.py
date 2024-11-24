from mnt.pyfiction import (read_technology_network, exact_cartesian, equivalence_checking, eq_type, exact_params,
                           exact_stats, exact_hexagonal)
import unittest
import os

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


if __name__ == '__main__':
    unittest.main()
