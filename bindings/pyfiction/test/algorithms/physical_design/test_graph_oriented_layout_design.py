from mnt.pyfiction import *
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestGraphOrientedLayoutDesign(unittest.TestCase):

    def test_a_star_pr(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        params = graph_oriented_layout_design_params()
        params.return_first = True

        stats = graph_oriented_layout_design_stats()

        layout = graph_oriented_layout_design(network, params, statistics=stats)

        self.assertNotEqual(equivalence_checking(network, layout), eq_type.STRONG)


if __name__ == '__main__':
    unittest.main()
