from mnt.pyfiction import (
    fanout_substitution,
    fanout_substitution_params,
    is_fanout_substituted,
    substitution_strategy,
)


def test_substitution(mux21):
    assert mux21.num_gates() == 5

    assert not is_fanout_substituted(mux21)

    substituted_default = fanout_substitution(mux21)
    assert substituted_default.num_gates() == 6
    assert is_fanout_substituted(substituted_default)

    depth_params = fanout_substitution_params()
    depth_params.strategy = substitution_strategy.DEPTH
    substituted_depth = fanout_substitution(mux21, depth_params)
    assert substituted_depth.num_gates() == 6
    assert is_fanout_substituted(substituted_depth, depth_params)

    breadth_params = fanout_substitution_params()
    breadth_params.strategy = substitution_strategy.BREADTH
    substituted_breadth = fanout_substitution(mux21, breadth_params)
    assert substituted_breadth.num_gates() == 6
    assert is_fanout_substituted(substituted_breadth, breadth_params)

    random_params = fanout_substitution_params()
    random_params.strategy = substitution_strategy.RANDOM
    substituted_random = fanout_substitution(mux21, random_params)
    assert substituted_random.num_gates() == 6
    assert is_fanout_substituted(substituted_random, random_params)

    random_seed_params = fanout_substitution_params()
    random_seed_params.strategy = substitution_strategy.RANDOM
    random_seed_params.seed = 42
    substituted_random_seed = fanout_substitution(mux21, random_seed_params)
    assert substituted_random_seed.num_gates() == 6
    assert is_fanout_substituted(substituted_random_seed, random_seed_params)

    threshold_params = fanout_substitution_params()
    threshold_params.threshold = 2
    substituted_threshold = fanout_substitution(mux21, threshold_params)
    assert substituted_threshold.num_gates() == 5
    assert is_fanout_substituted(substituted_threshold, threshold_params)
    assert not is_fanout_substituted(substituted_threshold, depth_params)
    assert not is_fanout_substituted(substituted_threshold, breadth_params)
