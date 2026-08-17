import os
import unittest

import pytest

from mnt.pyfiction import (
    eq_type,
    equivalence_checking,
    orthogonal,
    read_technology_network,
    wiring_reduction,
    wiring_reduction_params,
    wiring_reduction_stats,
)

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestWiringReduction(unittest.TestCase):
    def test_wiring_reduction_default(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        layout = orthogonal(network)

        assert equivalence_checking(network, layout) == eq_type.STRONG

        wiring_reduction(layout)

        assert equivalence_checking(network, layout) == eq_type.STRONG

    def test_wiring_reduction_with_parameters(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        layout = orthogonal(network)

        assert equivalence_checking(network, layout) == eq_type.STRONG

        params = wiring_reduction_params()
        wiring_reduction(layout, params)

        assert equivalence_checking(network, layout) == eq_type.STRONG

    def test_wiring_reduction_with_stats(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        layout = orthogonal(network)

        assert equivalence_checking(network, layout) == eq_type.STRONG

        stats = wiring_reduction_stats()
        wiring_reduction(layout, statistics=stats)

        assert equivalence_checking(network, layout) == eq_type.STRONG
        assert stats.time_total.total_seconds() > 0
        assert stats.x_size_before == 6
        assert stats.y_size_before == 8
        assert stats.x_size_after == 6
        assert stats.y_size_after == 5
        assert stats.num_wires_before == 21
        assert stats.num_wires_after == 15
        assert stats.wiring_improvement == pytest.approx(28.57)
        assert stats.area_improvement == pytest.approx(37.5)

    def test_wiring_reduction_with_stats_and_parameters(self):
        network = read_technology_network(dir_path + "/../../resources/mux21.v")

        layout = orthogonal(network)

        assert equivalence_checking(network, layout) == eq_type.STRONG

        params = wiring_reduction_params()
        params.timeout = 1000000

        stats = wiring_reduction_stats()
        wiring_reduction(layout, params, stats)

        assert equivalence_checking(network, layout) == eq_type.STRONG
        assert stats.time_total.total_seconds() > 0
        assert stats.x_size_before == 6
        assert stats.y_size_before == 8
        assert stats.x_size_after == 6
        assert stats.y_size_after == 5
        assert stats.num_wires_before == 21
        assert stats.num_wires_after == 15
        assert stats.wiring_improvement == pytest.approx(28.57)
        assert stats.area_improvement == pytest.approx(37.5)


if __name__ == "__main__":
    unittest.main()
