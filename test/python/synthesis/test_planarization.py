# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for the planarization pipeline."""

from __future__ import annotations

from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction.networks import simulate_outputs
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.synthesis import (
    decision_criterion,
    is_balanced,
    is_fanout_substituted,
    network_balancing,
    output_order,
    planarization,
    planarization_stats,
    planarization_strategy,
)

if TYPE_CHECKING:
    from pathlib import Path

    from mnt.pyfiction.networks import technology_network


UNIFIED: dict[str, Any] = {
    "unify_outputs": True,
    "buffer_constant_outputs": False,
}


def _balanced(network: technology_network) -> technology_network:
    return network_balancing(network, **UNIFIED)


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
    return read_network(str(resources_dir / "FA.v"))


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

    stats = planarization_stats()
    planar, original_input = planarization(balanced, strategy=strategy, criterion=criterion, statistics=stats)

    assert planar.num_pis() == len(original_input)
    assert planar.num_pis() > balanced.num_pis()  # the full adder needs virtual inputs
    assert sorted(set(original_input)) == list(range(balanced.num_pis()))
    assert planar.num_pos() == balanced.num_pos()
    assert is_balanced(planar, **UNIFIED)
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

    planar, original_input = planarization(balanced, fanout_degree=3)

    assert is_fanout_substituted(planar, degree=3)
    assert _equivalent_with_tied_inputs(balanced, planar, original_input)


def test_random_output_order_is_equivalent(full_adder: technology_network) -> None:
    """A seeded random output order gives a reproducible, equivalent planar network."""
    balanced = _balanced(full_adder)

    options: dict[str, Any] = {
        "po_order": output_order.RANDOM_PO_ORDER,
        "seed": 7,
    }
    planar, original_input = planarization(balanced, **options)
    again, original_input_again = planarization(balanced, **options)

    assert planar.size() == again.size()
    assert original_input == original_input_again
    assert _equivalent_with_tied_inputs(balanced, planar, original_input)


def test_statistics_and_progress(full_adder: technology_network) -> None:
    """The hybrid strategy reports its crossing decisions, and the progress callback is called."""
    balanced = _balanced(full_adder)

    reports: list[tuple[str, int, int]] = []

    stats = planarization_stats()
    planar, original_input = planarization(
        balanced,
        strategy=planarization_strategy.HYBRID,
        on_progress=lambda task, done, total: reports.append((task, done, total)),
        statistics=stats,
    )

    assert reports
    assert all(done <= total for _, done, total in reports if total > 0)
    assert stats.num_crossing_levels >= 0
    assert (stats.num_crossings > 0) == (stats.num_crossing_levels > 0)
    assert "crossing levels" in repr(stats)
    assert _equivalent_with_tied_inputs(balanced, planar, original_input)


def test_invalid_parameters_and_limits(full_adder: technology_network) -> None:
    """Invalid parameters raise ``ValueError`` before any stage runs; an exceeded limit raises ``RuntimeError``."""
    balanced = _balanced(full_adder)

    with pytest.raises(ValueError, match="degree"):
        planarization(balanced, fanout_degree=1)

    with pytest.raises(ValueError, match="budget"):
        planarization(balanced, lookahead_budget=0)

    # the full adder needs duplicates, so a limit of one is exceeded
    with pytest.raises(RuntimeError, match="duplications"):
        planarization(balanced, max_duplications=1)
