import os

import pytest

from mnt.pyfiction import (
    eq_type,
    equivalence_checking,
    orthogonal,
    post_layout_optimization,
    post_layout_optimization_params,
    post_layout_optimization_stats,
    read_technology_network,
)

dir_path = os.path.dirname(os.path.realpath(__file__))


def test_post_layout_optimization_default():
    network = read_technology_network(dir_path + "/../../resources/mux21.v")

    layout = orthogonal(network)

    assert equivalence_checking(network, layout) == eq_type.STRONG

    post_layout_optimization(layout)

    assert equivalence_checking(network, layout) == eq_type.STRONG


def test_post_layout_optimization_with_parameters():
    network = read_technology_network(dir_path + "/../../resources/mux21.v")

    layout = orthogonal(network)

    assert equivalence_checking(network, layout) == eq_type.STRONG

    params = post_layout_optimization_params()
    post_layout_optimization(layout, params)

    assert equivalence_checking(network, layout) == eq_type.STRONG


def test_post_layout_optimization_with_stats():
    network = read_technology_network(dir_path + "/../../resources/mux21.v")

    layout = orthogonal(network)

    assert equivalence_checking(network, layout) == eq_type.STRONG

    stats = post_layout_optimization_stats()
    post_layout_optimization(layout, statistics=stats)

    assert equivalence_checking(network, layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    assert stats.x_size_before == 6
    assert stats.y_size_before == 8
    assert stats.x_size_after == 6
    assert stats.y_size_after == 4
    assert stats.area_improvement == pytest.approx(50.0)


def test_post_layout_optimization_with_stats_and_parameters():
    network = read_technology_network(dir_path + "/../../resources/mux21.v")

    layout = orthogonal(network)

    assert equivalence_checking(network, layout) == eq_type.STRONG

    params = post_layout_optimization_params()
    params.max_gate_relocations = 1
    params.timeout = 1000000

    stats = post_layout_optimization_stats()
    post_layout_optimization(layout, params, statistics=stats)

    assert equivalence_checking(network, layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    assert stats.x_size_before == 6
    assert stats.y_size_before == 8
    assert stats.x_size_after == 6
    assert stats.y_size_after == 4
    assert stats.area_improvement == pytest.approx(50.0)
