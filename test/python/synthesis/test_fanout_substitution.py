# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from collections import Counter
from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction.networks import simulate_outputs, technology_network
from mnt.pyfiction.synthesis import fanout_substitution, is_fanout_substituted, network_balancing, substitution_strategy

if TYPE_CHECKING:
    from collections.abc import Callable


def _check_degrees(network: technology_network, degree: int, threshold: int) -> None:
    """Count uses independently, including repeated fanins and output consumers."""
    uses = Counter(fanin for gate in network.gates() for fanin in network.fanins(gate))
    uses.update(network.pos())
    for node in network.nodes():
        if not network.is_constant(node):
            assert uses[node] <= (degree if network.is_buf(node) else threshold), (node, uses[node])
    assert is_fanout_substituted(network, degree=degree, threshold=threshold)


@pytest.mark.parametrize(
    "options",
    [
        {},
        {"strategy": substitution_strategy.DEPTH},
        {"strategy": substitution_strategy.BREADTH, "degree": 3},
        {"strategy": substitution_strategy.RANDOM},
        {"strategy": substitution_strategy.RANDOM, "seed": 42, "threshold": 2},
        {"degree": 2**32 - 1},
        {"threshold": 2**32 - 1},
    ],
)
def test_substitution(mux21: technology_network, options: dict[str, Any]) -> None:
    expected = [("out", [True, True, False, False, True, False, True, False])]
    assert simulate_outputs(mux21) == expected
    assert not is_fanout_substituted(mux21)
    substituted = fanout_substitution(mux21, **options)
    _check_degrees(substituted, options.get("degree", 2), options.get("threshold", 1))
    assert simulate_outputs(substituted) == expected
    assert simulate_outputs(mux21) == expected
    if options.get("threshold") == 2**32 - 1:
        assert not any(substituted.is_fanout(n) for n in substituted.nodes())


@pytest.mark.parametrize("strategy", list(substitution_strategy))
def test_repeated_fanins(blif_network: Callable[[str], technology_network], strategy: substitution_strategy) -> None:
    network = blif_network(".model repeated\n.inputs a\n.outputs f\n.names a a a a a f\n11111 1\n.end\n")
    substituted = fanout_substitution(network, strategy=strategy, degree=2, threshold=3, seed=42)
    _check_degrees(substituted, 2, 3)
    assert simulate_outputs(substituted) == [("f", [True, False])]


def test_repeated_substitution_and_balancing(
    interface_network: technology_network, interface_outputs: list[tuple[str, list[bool]]]
) -> None:
    network = interface_network
    for _ in range(3):
        network = fanout_substitution(network)
        _check_degrees(network, 2, 1)
        assert [network.get_name(pi) for pi in network.pis()] == ["apple", "banana", "cherry", "unused"]
        assert simulate_outputs(network) == interface_outputs
    balanced = network_balancing(network)
    _check_degrees(balanced, 2, 1)
    assert simulate_outputs(balanced) == interface_outputs


def test_random_seed_repeats_connections(interface_network: technology_network) -> None:
    first = fanout_substitution(interface_network, strategy=substitution_strategy.RANDOM, seed=42)
    second = fanout_substitution(interface_network, strategy=substitution_strategy.RANDOM, seed=42)
    assert [first.fanins(n) for n in first.nodes()] == [second.fanins(n) for n in second.nodes()]
    assert first.pos() == second.pos()


@pytest.mark.parametrize("options", [{"degree": 0}, {"degree": 1}, {"threshold": 0}])
def test_invalid_branching_is_rejected(options: dict[str, Any]) -> None:
    with pytest.raises(ValueError, match=r"degree|threshold"):
        fanout_substitution(technology_network(), **options)


def test_empty_network() -> None:
    network = fanout_substitution(technology_network())
    assert network.num_pis() == network.num_pos() == network.num_gates() == 0
    _check_degrees(network, 2, 1)


@pytest.mark.parametrize(("degree", "threshold", "expected"), [(3, 2, True), (2, 2, False), (3, 1, False)])
def test_checker_distinguishes_fanout_and_gate_limits(
    blif_network: Callable[[str], technology_network], degree: int, threshold: int, *, expected: bool
) -> None:
    network = blif_network(
        ".model limits\n.inputs a\n.outputs a b c d\n.names a b\n1 1\n.names b c\n1 1\n.names b d\n1 1\n.end\n"
    )
    assert is_fanout_substituted(network, degree=degree, threshold=threshold) is expected
