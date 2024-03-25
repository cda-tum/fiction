from mnt.pyfiction import *
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestPostLayoutOptimiztaion(unittest.TestCase):

    def test_post_layout_optimization_without_stats(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        layout = orthogonal(network)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

        post_layout_optimization(layout)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

    def test_post_layout_optimization_with_stats(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        layout = orthogonal(network)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

        stats = post_layout_optimization_stats()
        post_layout_optimization(layout, stats)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)
        self.assertGreater(stats.time_total.total_seconds(), 0)


if __name__ == '__main__':
    unittest.main()
