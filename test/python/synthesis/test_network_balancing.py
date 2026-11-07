# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.networks import simulate_outputs
from mnt.pyfiction.synthesis import is_balanced, network_balancing

if TYPE_CHECKING:
    from collections.abc import Callable

    from mnt.pyfiction.networks import technology_network


def _levels(network: technology_network) -> dict[int, int]:
    """Compute depths from public edges and check every nonconstant gate path.

    Returns:
        Depth of every node.
    """
    levels = dict.fromkeys(network.pis(), 0)
    levels.update((n, 0) for n in network.nodes() if network.is_constant(n))
    for gate in network.gates():
        depths = [levels[n] for n in network.fanins(gate) if not network.is_constant(n)]
        assert len(set(depths)) <= 1, (gate, depths)
        levels[gate] = max(depths, default=0) + 1
    return levels


@pytest.mark.parametrize("unify_outputs", [False, True])
def test_balancing(mux21: technology_network, *, unify_outputs: bool) -> None:
    assert not is_balanced(mux21)
    balanced = network_balancing(mux21, unify_outputs=unify_outputs)
    assert is_balanced(balanced, unify_outputs=unify_outputs)
    _levels(balanced)
    assert simulate_outputs(balanced) == [("out", [True, True, False, False, True, False, True, False])]
    assert not is_balanced(mux21)


@pytest.mark.parametrize("unify_outputs", [False, True])
@pytest.mark.parametrize("buffer_constant_outputs", [False, True])
def test_balancing_preserves_interfaces(
    interface_network: technology_network,
    interface_outputs: list[tuple[str, list[bool]]],
    *,
    unify_outputs: bool,
    buffer_constant_outputs: bool,
) -> None:
    balanced = network_balancing(
        interface_network, unify_outputs=unify_outputs, buffer_constant_outputs=buffer_constant_outputs
    )
    assert is_balanced(balanced, unify_outputs=unify_outputs, buffer_constant_outputs=buffer_constant_outputs)
    levels = _levels(balanced)
    assert [balanced.get_name(pi) for pi in balanced.pis()] == ["apple", "banana", "cherry", "unused"]
    assert simulate_outputs(balanced) == interface_outputs
    assert simulate_outputs(interface_network) == interface_outputs
    if unify_outputs:
        outputs = [n for n in balanced.pos() if buffer_constant_outputs or not balanced.is_constant(n)]
        assert len({levels[n] for n in outputs}) == 1
    if not buffer_constant_outputs:
        assert [balanced.is_constant(n) for n in balanced.pos()[:2]] == [True, True]
    assert is_balanced(balanced, unify_outputs=True) == (unify_outputs and buffer_constant_outputs)


@pytest.mark.parametrize("inputs", ["", "a"])
@pytest.mark.parametrize("unify_outputs", [False, True])
def test_without_outputs(
    blif_network: Callable[[str], technology_network], inputs: str, *, unify_outputs: bool
) -> None:
    declaration = f".inputs {inputs}\n" if inputs else ""
    network = blif_network(f".model empty\n{declaration}.end\n")
    balanced = network_balancing(network, unify_outputs=unify_outputs)
    assert balanced.num_pis() == bool(inputs)
    assert balanced.num_pos() == balanced.num_gates() == 0
    assert is_balanced(balanced, unify_outputs=unify_outputs)
    assert simulate_outputs(balanced) == []
