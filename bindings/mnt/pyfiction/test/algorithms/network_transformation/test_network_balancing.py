import os

from mnt.pyfiction import is_balanced, network_balancing, network_balancing_params, read_technology_network

dir_path = os.path.dirname(os.path.realpath(__file__))


def test_balancing():
    network = read_technology_network(dir_path + "/../../resources/mux21.v")
    assert network.num_gates() == 5

    assert not is_balanced(network)

    balanced_default = network_balancing(network)
    assert balanced_default.num_gates() == 7
    assert is_balanced(balanced_default)

    unify_params = network_balancing_params()
    unify_params.unify_outputs = True
    balanced_unify = network_balancing(network, unify_params)
    assert balanced_unify.num_gates() == 7
    assert is_balanced(balanced_unify, unify_params)
