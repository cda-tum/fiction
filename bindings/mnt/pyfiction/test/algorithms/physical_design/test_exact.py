import os

from mnt.pyfiction import (
    eq_type,
    equivalence_checking,
    exact_cartesian,
    exact_hexagonal,
    exact_params,
    exact_stats,
    read_technology_network,
)

dir_path = os.path.dirname(os.path.realpath(__file__))


def test_exact_cartesian_default():
    network = read_technology_network(dir_path + "/../../resources/mux21.v")
    layout = exact_cartesian(network)
    assert equivalence_checking(network, layout) == eq_type.STRONG


def test_exact_cartesian_with_parameters():
    network = read_technology_network(dir_path + "/../../resources/mux21.v")

    params = exact_params()
    params.border_io = True
    params.crossings = True
    params.scheme = "ESR"

    layout = exact_cartesian(network, params)

    assert equivalence_checking(network, layout) == eq_type.STRONG


def test_exact_cartesian_with_stats():
    network = read_technology_network(dir_path + "/../../resources/mux21.v")

    stats = exact_stats()

    layout = exact_cartesian(network, statistics=stats)

    assert equivalence_checking(network, layout) == eq_type.STRONG


def test_exact_hexagonal_default():
    network = read_technology_network(dir_path + "/../../resources/mux21.v")
    layout = exact_hexagonal(network)
    assert equivalence_checking(network, layout) == eq_type.STRONG


def test_exact_hexagonal_with_parameters():
    network = read_technology_network(dir_path + "/../../resources/mux21.v")

    params = exact_params()
    params.border_io = True
    params.crossings = True
    params.scheme = "ESR"

    layout = exact_hexagonal(network, params)

    assert equivalence_checking(network, layout) == eq_type.STRONG


def test_exact_hexagonal_with_stats():
    network = read_technology_network(dir_path + "/../../resources/mux21.v")

    stats = exact_stats()

    layout = exact_hexagonal(network, statistics=stats)

    assert equivalence_checking(network, layout) == eq_type.STRONG
