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

from mnt.pyfiction.networks import simulate_outputs
from mnt.pyfiction.networks.io import read_technology_network
from mnt.pyfiction.synthesis import (
    decision_criterion,
    fanout_substitution_params,
    is_balanced,
    is_fanout_substituted,
    network_balancing,
    network_balancing_params,
    output_order,
    planarization,
    planarization_params,
    planarization_stats,
    planarization_strategy,
)

if TYPE_CHECKING:
    from pathlib import Path

    from mnt.pyfiction.networks import technology_network


def _unified() -> network_balancing_params:
    params = network_balancing_params()
    params.unify_outputs = True
    params.buffer_constant_outputs = False
    return params


def _balanced(network: technology_network) -> technology_network:
    return network_balancing(network, _unified())


def _equivalent_with_tied_inputs(
    original: technology_network, planar: technology_network, original_input: list[int]
) -> bool:
    """Compare the truth tables with every input of ``planar`` tied to the original input it stands for.

    ``simulate_outputs`` lists the bits of a truth table from its highest index down, and input ``j`` of a
    network is bit ``j`` of the table index.

    Returns:
        Whether every output agrees for every assignment of the original inputs.
    """
    num_original = original.num_pis()
    original_tables = [bits for _, bits in simulate_outputs(original)]
    planar_tables = [bits for _, bits in simulate_outputs(planar)]

    if len(original_tables) != len(planar_tables):
        return False

    for assignment in range(1 << num_original):
        planar_index = sum(((assignment >> real) & 1) << position for position, real in enumerate(original_input))
        for original_bits, planar_bits in zip(original_tables, planar_tables, strict=True):
            if original_bits[-1 - assignment] != planar_bits[-1 - planar_index]:
                return False

    return True


@pytest.fixture
def full_adder(resources_dir: Path) -> technology_network:
    """A full adder, whose planarization duplicates inputs.

    Returns:
        The ``FA.v`` network as a ``technology_network``.
    """
    return read_technology_network(str(resources_dir / "FA.v"))


def test_unbalanced_networks_are_rejected(mux21: technology_network) -> None:
    """The pipeline requires a balanced network with unified outputs."""
    with pytest.raises(ValueError, match="balanced"):
        planarization(mux21)


@pytest.mark.parametrize("strategy", [planarization_strategy.DUPLICATION, planarization_strategy.HYBRID])
@pytest.mark.parametrize("criterion", [decision_criterion.WEIGHTED_CONE, decision_criterion.LOOKAHEAD])
def test_planarization_is_equivalent(
    full_adder: technology_network, strategy: planarization_strategy, criterion: decision_criterion
) -> None:
    """Every strategy returns a balanced, fanout-substituted network that computes the same functions."""
    balanced = _balanced(full_adder)

    params = planarization_params()
    params.duplication.strategy = strategy
    params.duplication.criterion = criterion

    stats = planarization_stats()
    planar, original_input = planarization(balanced, params, stats)

    assert planar.num_pis() == len(original_input)
    assert planar.num_pis() > balanced.num_pis()  # the full adder needs virtual inputs
    assert sorted(set(original_input)) == list(range(balanced.num_pis()))
    assert planar.num_pos() == balanced.num_pos()
    assert is_balanced(planar, _unified())
    assert is_fanout_substituted(planar)
    assert stats.num_nodes == planar.size()
    assert stats.num_duplications > 0
    assert "num. nodes" in repr(stats)
    assert _equivalent_with_tied_inputs(balanced, planar, original_input)


def test_networks_without_duplicates_keep_their_inputs(mux21: technology_network) -> None:
    """A network that needs no duplicate keeps its inputs, in the order of the planar embedding."""
    balanced = _balanced(mux21)

    planar, original_input = planarization(balanced)

    assert planar.num_pis() == balanced.num_pis()
    assert sorted(original_input) == list(range(balanced.num_pis()))
    assert _equivalent_with_tied_inputs(balanced, planar, original_input)


def test_fanout_degree(full_adder: technology_network) -> None:
    """Fanout nodes respect the requested degree."""
    balanced = _balanced(full_adder)

    params = planarization_params()
    params.fanout_degree = 3

    planar, original_input = planarization(balanced, params)

    degree_three = fanout_substitution_params()
    degree_three.degree = 3
    assert is_fanout_substituted(planar, degree_three)
    assert _equivalent_with_tied_inputs(balanced, planar, original_input)


def test_random_output_order_is_equivalent(full_adder: technology_network) -> None:
    """A seeded random output order gives a reproducible, equivalent planar network."""
    balanced = _balanced(full_adder)

    params = planarization_params()
    params.duplication.po_order = output_order.RANDOM_PO_ORDER
    params.duplication.seed = 7

    planar, original_input = planarization(balanced, params)
    again, original_input_again = planarization(balanced, params)

    assert planar.size() == again.size()
    assert original_input == original_input_again
    assert _equivalent_with_tied_inputs(balanced, planar, original_input)
