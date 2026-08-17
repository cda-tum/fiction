import os

from mnt.pyfiction import (
    eq_type,
    equivalence_checking,
    orthogonal,
    orthogonal_params,
    orthogonal_stats,
    read_technology_network,
)

dir_path = os.path.dirname(os.path.realpath(__file__))


def test_orthogonal_default():
    network = read_technology_network(dir_path + "/../../resources/mux21.v")
    layout = orthogonal(network)
    assert equivalence_checking(network, layout) == eq_type.STRONG


def test_orthogonal_with_parameters():
    network = read_technology_network(dir_path + "/../../resources/mux21.v")

    params = orthogonal_params()

    layout = orthogonal(network, params)

    assert equivalence_checking(network, layout) == eq_type.STRONG


def test_orthogonal_with_stats():
    network = read_technology_network(dir_path + "/../../resources/mux21.v")

    stats = orthogonal_stats()

    layout = orthogonal(network, statistics=stats)

    assert equivalence_checking(network, layout) == eq_type.STRONG
