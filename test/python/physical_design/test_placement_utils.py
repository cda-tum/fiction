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

from mnt.pyfiction.layouts import (
    arrangement,
    cartesian_gate_layout,
    hexagonal_gate_layout,
    layout_input_port,
    shifted_cartesian_gate_layout,
)
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.physical_design import place

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path


@pytest.mark.parametrize(
    "make_layout",
    [
        pytest.param(lambda: cartesian_gate_layout((4, 3)), id="cartesian"),
        pytest.param(lambda: shifted_cartesian_gate_layout(arrangement.ODD_COLUMN, (4, 3)), id="shifted"),
        pytest.param(lambda: hexagonal_gate_layout(arrangement.EVEN_ROW, (4, 3)), id="hexagonal"),
    ],
)
@pytest.mark.parametrize(
    ("inputs", "table", "expected"),
    [
        ("a", "0 1", "1"),
        ("a", "1 1", "2"),
        ("a b", "11 1", "8"),
        ("a b", "1- 1\n-1 1", "e"),
        ("a b", "01 1\n10 1", "6"),
        ("a b", "00 1\n01 1\n10 1", "7"),
        ("a b", "00 1", "1"),
        ("a b", "00 1\n11 1", "9"),
        ("a b", "01 1", "4"),
        ("a b", "10 1", "2"),
        ("a b", "00 1\n01 1\n11 1", "d"),
        ("a b", "00 1\n10 1\n11 1", "b"),
        ("a b c", "11- 1\n1-1 1\n-11 1", "e8"),
        ("a b c", "1-0 1", "0a"),
    ],
    ids=[
        "inverter",
        "buffer",
        "and",
        "or",
        "xor",
        "nand",
        "nor",
        "xnor",
        "greater",
        "less",
        "greater-equal",
        "less-equal",
        "majority",
        "ordered-lut",
    ],
)
def test_place_preserves_functions_and_input_order(
    tmp_path: Path,
    make_layout: Callable[[], cartesian_gate_layout | shifted_cartesian_gate_layout | hexagonal_gate_layout],
    inputs: str,
    table: str,
    expected: str,
) -> None:
    """Place each bound arity with independently specified truth-table bits."""
    source = tmp_path / "placed.blif"
    source.write_text(f".model placed\n.inputs {inputs}\n.outputs f\n.names {inputs} f\n{table}\n.end\n")
    network = read_network(str(source))
    layout = make_layout()
    ports = [place(layout, (i, 0), network, node) for i, node in enumerate(network.pis())]
    args = [*ports, None] if len(ports) == 2 else ports
    placed = place(layout, (1, 1), network, network.gates()[0], *args)
    assert layout.object_function(placed).to_hex() == expected
    assert [layout.source(layout_input_port(placed, i)) for i in range(len(ports))] == ports
    assert [layout.get_name(port) for port in ports] == inputs.split()


@pytest.mark.parametrize("constant", [False, True, None])
def test_place_majority_constant_reduction(tmp_path: Path, *, constant: bool | None) -> None:
    """An explicit Boolean selects the majority cofactor; None cannot omit an input."""
    source = tmp_path / "majority.blif"
    source.write_text(".model majority\n.inputs a b c\n.outputs f\n.names a b c f\n11- 1\n1-1 1\n-11 1\n.end\n")
    network = read_network(str(source))
    layout = cartesian_gate_layout((3, 2))
    left = layout.create_pi("a", (0, 0))
    right = layout.create_pi("b", (1, 0))
    if constant is None:
        with pytest.raises(ValueError, match="constant"):
            place(layout, (1, 1), network, network.gates()[0], left, right, constant)
        assert layout.find_object((1, 1)) is None
    else:
        placed = place(layout, (1, 1), network, network.gates()[0], left, right, constant)
        assert layout.object_function(placed).to_hex() == ("e" if constant else "8")


@pytest.mark.parametrize("constant", [False, True])
def test_place_reduces_third_lut_input(tmp_path: Path, *, constant: bool) -> None:
    """Cofactor an asymmetric LUT without exchanging the remaining inputs."""
    source = tmp_path / "lut.blif"
    source.write_text(".model lut\n.inputs a b c\n.outputs f\n.names a b c f\n1-0 1\n.end\n")
    network = read_network(str(source))
    layout = cartesian_gate_layout((3, 2))
    left = layout.create_pi("a", (0, 0))
    right = layout.create_pi("b", (1, 0))
    placed = place(layout, (1, 1), network, network.gates()[0], left, right, constant)
    assert layout.object_function(placed).to_hex() == ("0" if constant else "a")
    assert [layout.source(layout_input_port(placed, i)) for i in range(2)] == [left, right]


@pytest.mark.parametrize("arity", [0, 1, 3])
def test_place_rejects_unsupported_arity(tmp_path: Path, arity: int) -> None:
    """Reject a two-input gate passed to another placement overload before mutation."""
    source = tmp_path / "and.blif"
    source.write_text(".model conjunction\n.inputs a b\n.outputs f\n.names a b f\n11 1\n.end\n")
    network = read_network(str(source))
    layout = cartesian_gate_layout((4, 2))
    ports = [layout.create_pi(str(i), (i, 0)) for i in range(arity)]
    with pytest.raises(ValueError, match=r"gate|input"):
        place(layout, (1, 1), network, network.gates()[0], *ports)
    assert layout.find_object((1, 1)) is None


def test_place_reduces_internal_constant_without_reordering_variables(tmp_path: Path) -> None:
    """Remove a middle constant fanin while retaining the two variable roles."""
    source = tmp_path / "internal_constant.blif"
    source.write_text(".model reduction\n.inputs a b\n.outputs f\n.names one\n1\n.names a one b f\n1-0 1\n.end\n")
    network = read_network(str(source))
    layout = cartesian_gate_layout((3, 2))
    a = layout.create_pi("a", (0, 0))
    b = layout.create_pi("b", (1, 0))
    gate = place(layout, (1, 1), network, network.gates()[0], a, b, c=True)
    assert layout.object_function(gate).to_hex() == "2"
    assert [layout.source(layout_input_port(gate, i)) for i in range(2)] == [a, b]


def test_place_rejects_four_variable_constant_reduction(tmp_path: Path) -> None:
    """One constant cannot reduce a four-variable LUT to two input ports."""
    source = tmp_path / "four_inputs.blif"
    source.write_text(".model reduction\n.inputs a b c d\n.outputs f\n.names a b c d f\n1001 1\n.end\n")
    network = read_network(str(source))
    layout = cartesian_gate_layout((3, 2))
    a = layout.create_pi("a", (0, 0))
    b = layout.create_pi("b", (1, 0))
    with pytest.raises(ValueError, match="three-input"):
        place(layout, (1, 1), network, network.gates()[0], a, b, c=False)
    assert layout.find_object((1, 1)) is None
