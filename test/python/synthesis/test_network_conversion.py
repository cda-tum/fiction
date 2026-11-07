# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of network conversion and technology mapping."""

from __future__ import annotations

from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction.networks import (
    aig_network,
    get_name,
    mig_network,
    set_name,
    simulate_outputs,
    technology_network,
    xag_network,
)
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.synthesis import convert_network, fanout_substitution, network_balancing, network_target
from mnt.pyfiction.verification import eq_type, equivalence_checking

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path


TARGETS = [
    (network_target.TEC, technology_network),
    (network_target.AIG, aig_network),
    (network_target.XAG, xag_network),
    (network_target.MIG, mig_network),
]


@pytest.mark.parametrize("cls", [technology_network, aig_network, xag_network, mig_network])
def test_default_conversion(
    resources_dir: Path, cls: type[technology_network | aig_network | xag_network | mig_network]
) -> None:
    network = read_network(str(resources_dir / "mux21.v"), network_type=cls)
    converted = convert_network(network)
    assert isinstance(converted, technology_network)
    assert simulate_outputs(converted) == [("out", [True, True, False, False, True, False, True, False])]
    assert equivalence_checking(read_network(str(resources_dir / "mux21.v")), converted) == eq_type.STRONG


@pytest.mark.parametrize("source", list(network_target))
@pytest.mark.parametrize(("target", "cls"), TARGETS)
def test_conversion_preserves_interfaces(
    interface_network: technology_network,
    interface_outputs: list[tuple[str, list[bool]]],
    source: network_target,
    target: network_target,
    cls: type[technology_network | aig_network | xag_network | mig_network],
) -> None:
    """Every bound source/destination path preserves labels, constants, unused inputs, and LUT logic."""
    set_name(interface_network, "interfaces")
    original = convert_network(interface_network, source)
    network = convert_network(original, target)
    assert isinstance(network, cls)
    assert get_name(network) == "interfaces"
    assert [network.get_name(pi) for pi in network.pis()] == ["apple", "banana", "cherry", "unused"]
    assert simulate_outputs(original) == interface_outputs
    assert simulate_outputs(network) == interface_outputs


@pytest.mark.parametrize("variables", [2, 3])
@pytest.mark.parametrize(("target", "cls"), TARGETS)
def test_every_small_lut(
    blif_network: Callable[[str], technology_network],
    variables: int,
    target: network_target,
    cls: type[technology_network | aig_network | xag_network | mig_network],
) -> None:
    """All 16 two-input and 256 three-input functions survive conversion and inversion."""
    inputs = "abc"[:variables]
    names = []
    definitions = []
    expected = []
    for function in range(1 << (1 << variables)):
        output, inverted = f"f{function}", f"g{function}"
        names.extend((output, inverted))
        definitions.append(f".names {' '.join(inputs)} {output}" if function else f".names {output}")
        definitions.extend(
            f"{assignment:0{variables}b}"[::-1] + " 1"
            for assignment in range(1 << variables)
            if function & (1 << assignment)
        )
        definitions.extend((f".names {output} {inverted}", "0 1"))
        bits = [bool(function & (1 << i)) for i in range((1 << variables) - 1, -1, -1)]
        expected.extend(((output, bits), (inverted, [not bit for bit in bits])))
    source = blif_network(
        f".model luts\n.inputs {' '.join(inputs)}\n.outputs {' '.join(names)}\n" + "\n".join(definitions) + "\n.end\n"
    )
    converted = convert_network(source, target)
    assert isinstance(converted, cls)
    assert [converted.get_name(pi) for pi in converted.pis()] == list(inputs)
    assert simulate_outputs(converted) == expected


@pytest.mark.parametrize("transform", [fanout_substitution, network_balancing])
def test_repeated_conversion_after_buffer_insertion(
    interface_network: technology_network,
    interface_outputs: list[tuple[str, list[bool]]],
    transform: Callable[[technology_network], technology_network],
) -> None:
    network = transform(interface_network)
    size = network.size()
    for _ in range(3):
        network = convert_network(network)
        assert network.size() == size
        assert simulate_outputs(network) == interface_outputs


@pytest.mark.parametrize("target", list(network_target))
def test_empty_conversion(target: network_target) -> None:
    converted = convert_network(technology_network(), target)
    assert converted.num_pis() == converted.num_pos() == converted.num_gates() == 0
    assert simulate_outputs(converted) == []


@pytest.mark.parametrize("target", ["AIG", 99, None])
def test_invalid_selector(mux21: technology_network, target: Any) -> None:
    with pytest.raises(TypeError):
        convert_network(mux21, target)
