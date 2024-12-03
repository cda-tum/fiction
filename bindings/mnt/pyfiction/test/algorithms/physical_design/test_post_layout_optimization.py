from mnt.pyfiction import (
    read_technology_network,
    orthogonal,
    equivalence_checking,
    eq_type,
    post_layout_optimization,
    post_layout_optimization_params,
    post_layout_optimization_stats,
)
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestPostLayoutOptimiztaion(unittest.TestCase):
    def test_post_layout_optimization_default(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        layout = orthogonal(network)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

        post_layout_optimization(layout)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

    def test_post_layout_optimization_with_parameters(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        layout = orthogonal(network)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

        params = post_layout_optimization_params()
        post_layout_optimization(layout, params)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

    def test_post_layout_optimization_with_stats(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        layout = orthogonal(network)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

        stats = post_layout_optimization_stats()
        post_layout_optimization(layout, statistics=stats)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)
        self.assertGreater(stats.time_total.total_seconds(), 0)
        self.assertEqual(stats.x_size_before, 6)
        self.assertEqual(stats.y_size_before, 8)
        self.assertEqual(stats.x_size_after, 6)
        self.assertEqual(stats.y_size_after, 4)
        self.assertEqual(stats.area_improvement, 50.0)

    def test_post_layout_optimization_with_stats_and_parameters(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        layout = orthogonal(network)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

        params = post_layout_optimization_params()
        params.max_gate_relocations = 1
        params.timeout = 1000000

        stats = post_layout_optimization_stats()
        post_layout_optimization(layout, params, statistics=stats)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)
        self.assertGreater(stats.time_total.total_seconds(), 0)
        self.assertEqual(stats.x_size_before, 6)
        self.assertEqual(stats.y_size_before, 8)
        self.assertEqual(stats.x_size_after, 6)
        self.assertEqual(stats.y_size_after, 4)
        self.assertEqual(stats.area_improvement, 50.0)


if __name__ == "__main__":
    unittest.main()
