# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Truth-table simulation and layout extraction through the public Python API."""

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
from mnt.pyfiction.networks import aig_network, mig_network, simulate, simulate_outputs, technology_network, xag_network
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.synthesis import dynamic_truth_table

if TYPE_CHECKING:
    from pathlib import Path

GateLayout = cartesian_gate_layout | shifted_cartesian_gate_layout | hexagonal_gate_layout
Network = technology_network | aig_network | xag_network | mig_network
NETWORK_TYPES = [technology_network, aig_network, xag_network, mig_network]


@pytest.fixture(params=[cartesian_gate_layout, shifted_cartesian_gate_layout, hexagonal_gate_layout])
def layout(request: pytest.FixtureRequest) -> GateLayout:
    """Create an independent layout for each supported simulation geometry.

    Returns:
        An empty layout with the selected geometry.
    """
    if request.param is cartesian_gate_layout:
        return cartesian_gate_layout()
    return request.param(arrangement.EVEN_ROW)


@pytest.mark.parametrize("cls", NETWORK_TYPES)
@pytest.mark.parametrize(
    ("filename", "expected"),
    [
        ("xor2.v", [("out", [False, True, True, False])]),
        ("xnor2.v", [("out", [True, False, False, True])]),
        (
            "FA.v",
            [
                ("s", [bool(assignment.bit_count() % 2) for assignment in range(7, -1, -1)]),
                ("cout", [assignment.bit_count() >= 2 for assignment in range(7, -1, -1)]),
            ],
        ),
    ],
)
def test_logic_simulation(
    resources_dir: Path, cls: type[Network], filename: str, expected: list[tuple[str, list[bool]]]
) -> None:
    """All representations retain complete XOR, XNOR, and full-adder functions."""
    network = read_network(str(resources_dir / filename), network_type=cls)
    assert simulate_outputs(network=network) == expected
    assert simulate(network=network) == dict(expected)
    values = simulate_outputs(network)
    values[0][1].clear()
    assert simulate_outputs(network) == expected


def test_layout_logic_simulation(layout: GateLayout) -> None:
    """Directly built layouts simulate AND, OR, XOR, and majority without a solver."""
    a = layout.create_pi("a", (0, 0))
    b = layout.create_pi("b", (1, 0))
    c = layout.create_pi("c", (2, 0))
    gates = [
        layout.create_and(a, b, (0, 1)),
        layout.create_or(a, b, (1, 1)),
        layout.create_xor(a, b, (2, 1)),
        layout.create_maj(a, b, c, (3, 1)),
    ]
    for index, (gate, name) in enumerate(zip(gates, ["and", "or", "xor", "maj"], strict=True)):
        layout.create_po(gate, name, (index, 2))
    expected = [
        ("and", [True, False, False, False] * 2),
        ("or", [True, True, True, False] * 2),
        ("xor", [False, True, True, False] * 2),
        ("maj", [True, True, True, False, True, False, False, False]),
    ]
    assert simulate_outputs(layout=layout) == expected
    assert simulate(layout=layout) == dict(expected)


@pytest.mark.parametrize("cls", NETWORK_TYPES)
def test_duplicate_output_names_preserve_order(tmp_path: Path, cls: type[Network]) -> None:
    """Ordered simulation retains duplicate labels; dictionary simulation keeps the last output."""
    path = tmp_path / "outputs.v"
    path.write_text(
        "module top(a, f, g);\ninput a;\noutput f, g;\nassign f = a;\nassign g = ~a;\nendmodule\n", encoding="utf-8"
    )
    network = read_network(str(path), network_type=cls)
    network.set_output_name(0, "same")
    network.set_output_name(1, "same")
    assert simulate_outputs(network) == [("same", [True, False]), ("same", [False, True])]
    assert simulate(network) == {"same": [False, True]}


def test_layout_simulation_preserves_input_slots_and_unused_inputs(layout: GateLayout) -> None:
    """Extraction follows argument slots and explicit interface orders, including unused inputs."""
    unused = layout.create_pi("unused", (10, 0))
    left = layout.create_pi("left", (0, 0))
    right = layout.create_pi("right", (7, 0))
    less = layout.create_lt(right, left, (20, 0))
    result = layout.create_po(layout.create_buf(less, (30, 0)), "same", (40, 0))
    passed = layout.create_po(left, "same", (50, 0))
    layout.create_buf((3, 1))
    layout.set_input_order([left, right, unused])
    layout.set_output_order([result, passed])
    assert simulate_outputs(layout) == [
        ("same", [False, True, False, False] * 2),
        ("same", [True, False] * 4),
    ]
    assert simulate(layout) == {"same": [True, False] * 4}
    layout.set_input_order([right, left, unused])
    layout.set_output_order([passed, result])
    assert simulate_outputs(layout) == [
        ("same", [True, True, False, False] * 2),
        ("same", [False, False, True, False] * 2),
    ]
    layout.disconnect(layout_input_port(less, 0))
    with pytest.raises(ValueError, match="disconnected input"):
        simulate_outputs(layout)


@pytest.mark.parametrize("value", [False, True])
def test_layout_constant_functions(layout: GateLayout, *, value: bool) -> None:
    """Zero-input placed functions yield a single truth-table bit."""
    function = dynamic_truth_table(0)
    function.create_from_hex_string("1" if value else "0")
    constant = layout.create_gate([], function, (0, 0))
    layout.create_po(constant, "constant", (1, 0))
    assert simulate_outputs(layout) == [("constant", [value])]
    assert simulate(layout) == {"constant": [value]}


def test_layout_extraction_follows_connections_and_reused_identities(layout: GateLayout) -> None:
    """Connections determine logic when outputs and gates precede their input allocation."""
    removed = layout.create_pi("removed", (0, 0))
    layout.remove(removed)
    output = layout.create_po("result", (0, 0))
    function = dynamic_truth_table(2)
    function.create_from_hex_string("4")
    gate = layout.create_gate([], function, (1, 0))
    b = layout.create_pi("b", (2, 0))
    a = layout.create_pi("a", (3, 0))
    layout.connect(a, layout_input_port(gate, 0))
    layout.connect(b, layout_input_port(gate, 1))
    layout.connect(gate, layout_input_port(output, 0))
    assert simulate_outputs(layout) == [("result", [False, False, True, False])]


def test_layout_extraction_validates_only_output_dependencies(layout: GateLayout) -> None:
    """Reject connected cycles and missing arguments while ignoring dangling invalid objects."""
    a = layout.create_pi("a", (0, 0))
    cycle = layout.create_buf((1, 0))
    layout.connect(cycle, layout_input_port(cycle, 0))
    function = dynamic_truth_table(2)
    function.create_from_hex_string("8")
    gate = layout.create_gate([a], function, (2, 0))
    layout.disconnect(layout_input_port(gate, 0))
    layout.connect(a, layout_input_port(gate, 1))
    output = layout.create_po(a, "result", (3, 0))
    assert simulate_outputs(layout) == [("result", [True, False])]
    layout.connect(gate, layout_input_port(output, 0))
    with pytest.raises(ValueError, match="disconnected input"):
        simulate_outputs(layout)
    layout.connect(a, layout_input_port(gate, 0))
    assert simulate_outputs(layout) == [("result", [True, False])]
    layout.connect(cycle, layout_input_port(output, 0))
    with pytest.raises(ValueError, match="cycle"):
        simulate_outputs(layout)
    with pytest.raises(ValueError, match="cycle"):
        simulate(layout)
    layout.disconnect(layout_input_port(output, 0))
    with pytest.raises(ValueError, match="disconnected input"):
        simulate_outputs(layout)
    layout.connect(a, layout_input_port(output, 0))
    assert simulate_outputs(layout) == [("result", [True, False])]


def test_layout_extraction_handles_deep_wire_chains() -> None:
    """A deep output dependency chain does not require recursive traversal."""
    layout = cartesian_gate_layout()
    wire = layout.create_pi("a", (0, 0))
    for index in range(1, 10_001):
        wire = layout.create_buf(wire, (index, 0))
    layout.create_po(wire, "result", (10_001, 0))
    assert simulate_outputs(layout) == [("result", [True, False])]


@pytest.mark.parametrize("cls", NETWORK_TYPES)
def test_network_simulation_rejects_unrepresentable_input_count(tmp_path: Path, cls: type[Network]) -> None:
    """The representation bound rejects 38 inputs before truth-table allocation."""
    names = ",".join(f"a{index}" for index in range(38))
    path = tmp_path / "large.v"
    path.write_text(f"module top({names},f); input {names}; output f; assign f = a0; endmodule\n", encoding="utf-8")
    network = read_network(str(path), network_type=cls)
    for simulate_function in [simulate, simulate_outputs]:
        with pytest.raises(ValueError, match="fewer than 38 inputs"):
            simulate_function(network)


def test_layout_simulation_rejects_unrepresentable_input_count(layout: GateLayout) -> None:
    """Layout simulation applies the same input bound as network simulation."""
    for index in range(38):
        layout.create_pi(f"a{index}", (index, 0))
    for simulate_function in [simulate, simulate_outputs]:
        with pytest.raises(ValueError, match="fewer than 38 inputs"):
            simulate_function(layout)
