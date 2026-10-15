# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING, Any

from mnt.pyfiction.synthesis import fanout_substitution, is_fanout_substituted, substitution_strategy

if TYPE_CHECKING:
    from mnt.pyfiction.networks import technology_network


def test_substitution(mux21: technology_network) -> None:
    """Fanout substitution adds the required fanout node without adding an output buffer."""
    assert mux21.num_gates() == 4

    assert not is_fanout_substituted(mux21)

    substituted_default = fanout_substitution(mux21)
    assert substituted_default.num_gates() == 5
    assert is_fanout_substituted(substituted_default)

    depth_options: dict[str, Any] = {
        "strategy": substitution_strategy.DEPTH,
    }
    substituted_depth = fanout_substitution(mux21, **depth_options)
    assert substituted_depth.num_gates() == 5
    assert is_fanout_substituted(substituted_depth, **depth_options)

    breadth_options: dict[str, Any] = {
        "strategy": substitution_strategy.BREADTH,
    }
    substituted_breadth = fanout_substitution(mux21, **breadth_options)
    assert substituted_breadth.num_gates() == 5
    assert is_fanout_substituted(substituted_breadth, **breadth_options)

    random_options: dict[str, Any] = {
        "strategy": substitution_strategy.RANDOM,
    }
    substituted_random = fanout_substitution(mux21, **random_options)
    assert substituted_random.num_gates() == 5
    assert is_fanout_substituted(substituted_random, **random_options)

    random_seed_options: dict[str, Any] = {
        "strategy": substitution_strategy.RANDOM,
        "seed": 42,
    }
    substituted_random_seed = fanout_substitution(mux21, **random_seed_options)
    assert substituted_random_seed.num_gates() == 5
    assert is_fanout_substituted(substituted_random_seed, **random_seed_options)

    threshold_options: dict[str, Any] = {
        "threshold": 2,
    }
    substituted_threshold = fanout_substitution(mux21, **threshold_options)
    assert substituted_threshold.num_gates() == 4
    assert is_fanout_substituted(substituted_threshold, **threshold_options)
    assert not is_fanout_substituted(substituted_threshold, **depth_options)
    assert not is_fanout_substituted(substituted_threshold, **breadth_options)
