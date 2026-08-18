from mnt.pyfiction import is_balanced, network_balancing, network_balancing_params


def test_balancing(mux21):
    assert mux21.num_gates() == 5

    assert not is_balanced(mux21)

    balanced_default = network_balancing(mux21)
    assert balanced_default.num_gates() == 7
    assert is_balanced(balanced_default)

    unify_params = network_balancing_params()
    unify_params.unify_outputs = True
    balanced_unify = network_balancing(mux21, unify_params)
    assert balanced_unify.num_gates() == 7
    assert is_balanced(balanced_unify, unify_params)
