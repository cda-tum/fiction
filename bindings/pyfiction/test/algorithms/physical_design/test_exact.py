from fiction import pyfiction
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestExactCartesianPhysicalDesign(unittest.TestCase):

    def test_exact_default(self):
        network = pyfiction.read_logic_network(dir_path + "/../../resources/mux21.v")
        layout = pyfiction.exact_cartesian(network)
        self.assertEqual(pyfiction.equivalence_checking(network, layout), pyfiction.eq_type.STRONG)

    def test_exact_with_parameters(self):
        network = pyfiction.read_logic_network(dir_path + "/../../resources/mux21.v")

        params = pyfiction.exact_params()
        params.border_io = True
        params.crossings = True
        params.scheme = "ESP"

        layout = pyfiction.exact_cartesian(network, params)

        self.assertEqual(pyfiction.equivalence_checking(network, layout), pyfiction.eq_type.STRONG)

    def test_exact_with_stats(self):
        network = pyfiction.read_logic_network(dir_path + "/../../resources/mux21.v")

        stats = pyfiction.exact_stats()

        layout = pyfiction.exact_cartesian(network, statistics=stats)

        self.assertEqual(pyfiction.equivalence_checking(network, layout), pyfiction.eq_type.STRONG)


class TestExactHexagonalPhysicalDesign(unittest.TestCase):

    def test_exact_default(self):
        network = pyfiction.read_logic_network(dir_path + "/../../resources/mux21.v")
        layout = pyfiction.exact_hexagonal(network)
        self.assertEqual(pyfiction.equivalence_checking(network, layout), pyfiction.eq_type.STRONG)

    def test_exact_with_parameters(self):
        network = pyfiction.read_logic_network(dir_path + "/../../resources/mux21.v")

        params = pyfiction.exact_params()
        params.border_io = True
        params.crossings = True
        params.scheme = "ESP"

        layout = pyfiction.exact_hexagonal(network, params)

        self.assertEqual(pyfiction.equivalence_checking(network, layout), pyfiction.eq_type.STRONG)

    def test_exact_with_stats(self):
        network = pyfiction.read_logic_network(dir_path + "/../../resources/mux21.v")

        stats = pyfiction.exact_stats()

        layout = pyfiction.exact_hexagonal(network, statistics=stats)

        self.assertEqual(pyfiction.equivalence_checking(network, layout), pyfiction.eq_type.STRONG)
