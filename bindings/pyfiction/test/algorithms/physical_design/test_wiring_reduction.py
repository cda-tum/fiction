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

    def test_wiring_reduction_with_stats(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        layout = orthogonal(network)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)

        stats = wiring_reduction_stats()
        wiring_reduction(layout, stats)

        self.assertEqual(equivalence_checking(network, layout), eq_type.STRONG)
        self.assertGreater(stats.time_total.total_seconds(), 0)


if __name__ == '__main__':
    unittest.main()
