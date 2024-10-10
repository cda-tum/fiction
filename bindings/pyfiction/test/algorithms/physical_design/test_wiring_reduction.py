from mnt.pyfiction import *
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestWiringReduction(unittest.TestCase):

    def test_wiring_reduction_default(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        layout = orthogonal(network)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

        wiring_reduction(layout)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

    def test_wiring_reduction_with_parameters(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        layout = orthogonal(network)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

        params = wiring_reduction_params()
        wiring_reduction(layout, params)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

    def test_wiring_reduction_with_stats(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        layout = orthogonal(network)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

        stats = wiring_reduction_stats()
        wiring_reduction(layout, statistics=stats)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)
        self.assertGreater(stats.time_total.total_seconds(), 0)
        self.assertEqual(stats.x_size_before, 6)
        self.assertEqual(stats.y_size_before, 8)
        self.assertEqual(stats.x_size_after, 6)
        self.assertEqual(stats.y_size_after, 5)
        self.assertEqual(stats.num_wires_before, 21)
        self.assertEqual(stats.num_wires_after, 15)
        self.assertEqual(stats.wiring_improvement, 28.57)
        self.assertEqual(stats.area_improvement, 37.50)

    def test_wiring_reduction_with_stats_and_parameters(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        layout = orthogonal(network)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

        params = wiring_reduction_params()
        params.timeout = 1000000

        stats = wiring_reduction_stats()
        wiring_reduction(layout, stats)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)
        self.assertGreater(stats.time_total.total_seconds(), 0)
        self.assertEqual(stats.x_size_before, 6)
        self.assertEqual(stats.y_size_before, 8)
        self.assertEqual(stats.x_size_after, 6)
        self.assertEqual(stats.y_size_after, 5)
        self.assertEqual(stats.num_wires_before, 21)
        self.assertEqual(stats.num_wires_after, 15)
        self.assertEqual(stats.wiring_improvement, 28.57)
        self.assertEqual(stats.area_improvement, 37.50)


if __name__ == '__main__':
    unittest.main()
