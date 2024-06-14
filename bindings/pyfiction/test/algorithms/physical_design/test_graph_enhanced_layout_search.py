from mnt.pyfiction import *
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestGraphEnhancedLayoutSearch(unittest.TestCase):

    def test_a_star_pr(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        params = graph_enhanced_layout_search_params()
        params.return_first = True

        stats = graph_enhanced_layout_search_stats()

        layout = graph_enhanced_layout_search(network, params, statistics=stats)

        self.assertNotEqual(equivalence_checking(network, layout), eq_type.STRONG)


if __name__ == '__main__':
    unittest.main()
