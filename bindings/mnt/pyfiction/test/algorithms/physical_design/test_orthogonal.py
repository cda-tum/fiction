from mnt.pyfiction import (
    read_technology_network,
    orthogonal,
    equivalence_checking,
    eq_type,
    orthogonal_stats,
    orthogonal_params,
)
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestOrthogonalPhysicalDesign(unittest.TestCase):
    def test_orthogonal_default(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")
        layout = orthogonal(network)
        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

    def test_orthogonal_with_parameters(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        params = orthogonal_params()

        layout = orthogonal(network, params)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

    def test_orthogonal_with_stats(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        stats = orthogonal_stats()

        layout = orthogonal(network, statistics=stats)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)


if __name__ == "__main__":
    unittest.main()
