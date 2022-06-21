from fiction import pyfiction
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestOrthogonalPhysicalDesign(unittest.TestCase):

    def test_orthogonal_default(self):
        network = pyfiction.read_logic_network(dir_path + "/../../resources/mux21.v")
        layout = pyfiction.orthogonal(network)
        self.assertEqual(pyfiction.equivalence_checking(network, layout), pyfiction.eq_type.STRONG)

    def test_orthogonal_with_parameters(self):
        network = pyfiction.read_logic_network(dir_path + "/../../resources/mux21.v")

        params = pyfiction.orthogonal_params()

        layout = pyfiction.orthogonal(network, params)

        self.assertEqual(pyfiction.equivalence_checking(network, layout), pyfiction.eq_type.STRONG)

    def test_orthogonal_with_stats(self):
        network = pyfiction.read_logic_network(dir_path + "/../../resources/mux21.v")

        stats = pyfiction.orthogonal_stats()

        layout = pyfiction.orthogonal(network, statistics=stats)

        self.assertEqual(pyfiction.equivalence_checking(network, layout), pyfiction.eq_type.STRONG)
