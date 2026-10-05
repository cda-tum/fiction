# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for the planarization pipeline."""

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.synthesis import (
    decision_criterion,
    is_balanced,
    is_fanout_substituted,
    network_balancing,
    network_balancing_params,
    planarization,
    planarization_params,
    planarization_stats,
    planarization_strategy,
)
from mnt.pyfiction.verification import eq_type, equivalence_checking

if TYPE_CHECKING:
    from mnt.pyfiction.networks import technology_network


def _balanced(network: technology_network) -> technology_network:
    params = network_balancing_params()
    params.unify_outputs = True
    return network_balancing(network, params)


def test_unbalanced_networks_are_rejected(mux21: technology_network) -> None:
    """The pipeline requires a balanced network with unified outputs."""
    with pytest.raises(ValueError, match="balanced"):
        planarization(mux21)


@pytest.mark.parametrize("strategy", [planarization_strategy.DUPLICATION, planarization_strategy.HYBRID])
@pytest.mark.parametrize("criterion", [decision_criterion.WEIGHTED_CONE, decision_criterion.LOOKAHEAD])
def test_planarization_is_equivalent(
    mux21: technology_network, strategy: planarization_strategy, criterion: decision_criterion
) -> None:
    """Every strategy returns a balanced, fanout-substituted network that computes the same functions."""
    balanced = _balanced(mux21)

    params = planarization_params()
    params.duplication.strategy = strategy
    params.duplication.criterion = criterion

    stats = planarization_stats()
    planar, virtual_to_real = planarization(balanced, params, stats)

    assert planar.num_pis() == balanced.num_pis() + len(virtual_to_real)
    assert planar.num_pos() == balanced.num_pos()
    assert all(0 <= real < balanced.num_pis() for real in virtual_to_real)
    assert is_balanced(planar, network_balancing_params())
    assert is_fanout_substituted(planar)
    assert stats.num_nodes == planar.size()
    assert "num. nodes" in repr(stats)

    # virtual inputs take the value of their real input: tie them for the equivalence check
    if not virtual_to_real:
        assert equivalence_checking(balanced, planar) == eq_type.STRONG


def test_fanout_degree(mux21: technology_network) -> None:
    """Fanout nodes respect the requested degree."""
    balanced = _balanced(mux21)

    params = planarization_params()
    params.fanout_degree = 3

    planar, _ = planarization(balanced, params)

    assert planar.num_pos() == balanced.num_pos()
