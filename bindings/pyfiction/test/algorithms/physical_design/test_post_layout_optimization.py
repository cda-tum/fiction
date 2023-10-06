from fiction.pyfiction import *
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestPostLayoutOptimiztaion(unittest.TestCase):

    def test_post_layout_optimization_with_stats(self):
        network = read_logic_network(dir_path + "/../../resources/mux21.v")

        layout = orthogonal(network)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

        stats = post_layout_optimization_stats()
        post_layout_optimization(layout, statistics=stats)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)


if __name__ == '__main__':
    unittest.main()
