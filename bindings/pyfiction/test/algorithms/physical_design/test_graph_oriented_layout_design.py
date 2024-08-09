from mnt.pyfiction import *
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestGraphOrientedLayoutDesign(unittest.TestCase):

    def test_graph_oriented_layout_design(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        layout = graph_oriented_layout_design(network)

        self.assertNotEqual(equivalence_checking(network, layout), eq_type.NO)

    def test_graph_oriented_layout_design_with_parameters(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        params = graph_oriented_layout_design_params()
        params.return_first = True

        layout = graph_oriented_layout_design(network, params)

        self.assertNotEqual(equivalence_checking(network, layout), eq_type.NO)

    def test_graph_oriented_layout_design_with_stats(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        stats = graph_oriented_layout_design_stats()

        layout = graph_oriented_layout_design(network, statistics=stats)

        self.assertNotEqual(equivalence_checking(network, layout), eq_type.NO)

    def test_graph_oriented_layout_design_with_stats_and_parameters(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        params = graph_oriented_layout_design_params()
        params.return_first = True

        stats = graph_oriented_layout_design_stats()

        layout = graph_oriented_layout_design(network, params, statistics=stats)

        self.assertNotEqual(equivalence_checking(network, layout), eq_type.NO)


if __name__ == '__main__':
    unittest.main()
