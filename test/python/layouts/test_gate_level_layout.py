# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import copy
from typing import TYPE_CHECKING, cast

import pytest

from mnt.pyfiction.layouts import (
    LayoutInputPort,
    LayoutObjectId,
    arrangement,
    cartesian_gate_layout,
    coordinate,
    hexagonal_gate_layout,
    shifted_cartesian_gate_layout,
)
from mnt.pyfiction.synthesis import dynamic_truth_table
from mnt.pyfiction.verification import critical_path_length_and_throughput, gate_level_drv_params, gate_level_drvs

if TYPE_CHECKING:
    from collections.abc import Callable
    from typing import TypeAlias

GateLayout: TypeAlias = cartesian_gate_layout | shifted_cartesian_gate_layout | hexagonal_gate_layout


@pytest.mark.parametrize(
    "make_layout",
    [
        pytest.param(
            lambda: cartesian_gate_layout(extent=(3, 3, 1), clocking_scheme="2DDWave", layout_name="Layout"),
            id="cartesian_gate_layout",
        ),
        pytest.param(
            lambda: shifted_cartesian_gate_layout(
                arrangement.ODD_COLUMN, extent=(3, 3, 1), clocking_scheme="2DDWave", layout_name="Layout"
            ),
            id="shifted_cartesian_gate_layout",
        ),
        pytest.param(
            lambda: hexagonal_gate_layout(
                arrangement.EVEN_ROW, extent=(3, 3, 1), clocking_scheme="2DDWave", layout_name="Layout"
            ),
            id="hexagonal_gate_layout",
        ),
    ],
)
def test_gate_level_layout_inheritance(make_layout: Callable[[], GateLayout]) -> None:
    """Expose clocked geometry through the placed layout."""
    layout = make_layout()
    assert layout.incoming_clocked_zones((0, 0)) == []
    assert layout.outgoing_clocked_zones((2, 2)) == []

    for icz in layout.incoming_clocked_zones((1, 1)):
        assert icz in [layout.coord(1, 0), layout.coord(0, 1)]

    for icz in layout.outgoing_clocked_zones((1, 1)):
        assert icz in [layout.coord(1, 2), layout.coord(2, 1)]


@pytest.mark.parametrize(
    "make_layout",
    [
        pytest.param(lambda: cartesian_gate_layout((4, 4, 2), "2DDWave", "Layout"), id="cartesian_gate_layout"),
        pytest.param(
            lambda: shifted_cartesian_gate_layout(arrangement.ODD_COLUMN, (4, 4, 2), "2DDWave", "Layout"),
            id="shifted_cartesian_gate_layout",
        ),
        pytest.param(
            lambda: hexagonal_gate_layout(arrangement.EVEN_ROW, (4, 4, 2), "2DDWave", "Layout"),
            id="hexagonal_gate_layout",
        ),
    ],
)
def test_gate_level_layout_iteration(make_layout: Callable[[], GateLayout]) -> None:
    """Iterate declared objects without depending on allocation indices."""
    layout = make_layout()
    assert layout.is_empty()
    assert layout.size() == 0
    assert layout.num_gates() == 0
    assert layout.num_wires() == 0

    # layout creation
    x1 = layout.create_pi("x1", (1, 0))
    x2 = layout.create_pi("x2", (0, 1))
    x3 = layout.create_pi("x3", (2, 0))
    x4 = layout.create_pi("x4", (0, 2))

    a1 = layout.create_and(x1, x2, (1, 1))

    b1 = layout.create_buf(x3, (2, 1))
    b2 = layout.create_buf(x4, (1, 2))

    a2 = layout.create_and(b1, b2, (2, 2))

    c = layout.create_buf(a1, (2, 1, 1))

    f1 = layout.create_po(c, "f1", (3, 1))
    f2 = layout.create_po(a2, "f2", (3, 2))

    assert not layout.is_empty()

    assert layout.pis() == [x1, x2, x3, x4]
    assert layout.pos() == [f1, f2]
    gates = layout.gates()
    assert len(gates) == 2
    assert a1 in gates
    assert a2 in gates
    wires = layout.wires()
    assert len(wires) == 9
    for port in (x1, x2, x3, x4, b1, b2, c, f1, f2):
        assert port in wires

    for port, position in [
        (x1, coordinate(1, 0)),
        (x2, coordinate(0, 1)),
        (x3, coordinate(2, 0)),
        (x4, coordinate(0, 2)),
        (a1, coordinate(1, 1)),
        (b1, coordinate(2, 1)),
        (b2, coordinate(1, 2)),
        (a2, coordinate(2, 2)),
        (c, coordinate(2, 1, 1)),
        (f1, coordinate(3, 1)),
        (f2, coordinate(3, 2)),
    ]:
        assert isinstance(port, LayoutObjectId)
        assert layout.find_object(position) == port
        assert layout.get_tile(port) == position

    for port, name in ((x1, "x1"), (x2, "x2"), (x3, "x3"), (x4, "x4"), (f1, "f1"), (f2, "f2")):
        assert layout.get_name(port) == name

    # Declared inputs retain logical argument order.
    assert layout.inputs(x1) == []
    assert layout.inputs(f1) == [c]
    assert layout.inputs(a2) == [b1, b2]

    # Sink ports identify each destination input.
    assert layout.sinks(x1) == [LayoutInputPort(a1, 0)]
    assert layout.sinks(f1) == []
    assert layout.sinks(a2) == [LayoutInputPort(f2, 0)]

    cp, tp = critical_path_length_and_throughput(layout)
    assert cp == 4
    assert tp == 1

    drv_params = gate_level_drv_params()
    assert gate_level_drvs(layout, drv_params) == (0, 0)


def test_gate_level_layout_gate_types() -> None:
    """Classify placed gates and identity objects for every topology."""
    layouts: list[cartesian_gate_layout | shifted_cartesian_gate_layout | hexagonal_gate_layout] = [
        cartesian_gate_layout((3, 9, 1), "2DDWave", "Layout"),
        shifted_cartesian_gate_layout(arrangement.ODD_COLUMN, (3, 9, 1), "2DDWave", "Layout"),
        hexagonal_gate_layout(arrangement.EVEN_ROW, (3, 9, 1), "2DDWave", "Layout"),
    ]
    for layout in layouts:
        assert layout.is_empty()

        # layout creation
        # pis
        x1 = layout.create_pi("x1", (0, 0))
        x2 = layout.create_pi("x2", (0, 1))
        x3 = layout.create_pi("x3", (0, 2))
        x4 = layout.create_pi("x4", (0, 3))
        x5 = layout.create_pi("x5", (0, 4))
        x6 = layout.create_pi("x6", (0, 5))
        x7 = layout.create_pi("x7", (0, 6))

        # gates
        inv = layout.create_not(x1, (1, 0))
        and_gate = layout.create_and(x2, inv, (1, 1))
        nand_gate = layout.create_nand(x3, and_gate, (1, 2))
        or_gate = layout.create_or(x4, nand_gate, (1, 3))
        nor_gate = layout.create_nor(x5, or_gate, (1, 4))
        xor_gate = layout.create_xor(x6, nor_gate, (1, 5))
        xnor_gate = layout.create_xnor(x7, xor_gate, (1, 6))
        fanout = layout.create_buf(xnor_gate, (1, 7))
        buf = layout.create_buf(fanout, (2, 7))

        # pos
        f1 = layout.create_po(fanout, "f1", (1, 8))
        f2 = layout.create_po(buf, "f2", (2, 8))

        # check gate type
        # pis
        assert layout.is_pi(x1)
        assert layout.is_pi(x2)
        assert layout.is_pi(x3)
        assert layout.is_pi(x4)
        assert layout.is_pi(x5)
        assert layout.is_pi(x6)
        assert layout.is_pi(x7)

        # gates
        assert layout.is_inv(inv)
        assert layout.is_and(and_gate)
        assert layout.is_nand(nand_gate)
        assert layout.is_or(or_gate)
        assert layout.is_nor(nor_gate)
        assert layout.is_xor(xor_gate)
        assert layout.is_xnor(xnor_gate)
        assert layout.is_fanout(fanout)
        assert layout.is_wire(buf)

        # pos
        assert layout.is_po(f1)
        assert layout.is_po(f2)

    layouts = [
        cartesian_gate_layout((3, 3, 1), "RES", "Layout"),
        shifted_cartesian_gate_layout(arrangement.ODD_COLUMN, (3, 3, 1), "RES", "Layout"),
        hexagonal_gate_layout(arrangement.EVEN_ROW, (3, 3, 1), "RES", "Layout"),
    ]
    for layout in layouts:
        assert layout.is_empty()

        # pis
        x1 = layout.create_pi("x1", (0, 1))
        x2 = layout.create_pi("x2", (1, 0))
        x3 = layout.create_pi("x3", (2, 1))

        # maj
        maj = layout.create_maj(x1, x2, x3, (1, 1))

        # po
        f1 = layout.create_po(maj, "f1", (1, 2))

        # check gate type
        # pis
        assert layout.is_pi(x1)
        assert layout.is_pi(x2)
        assert layout.is_pi(x3)

        # maj
        assert layout.is_maj(maj)

        # po
        assert layout.is_po(f1)


@pytest.mark.parametrize(
    "make_layout",
    [
        pytest.param(lambda: cartesian_gate_layout((3, 3, 1)), id="cartesian"),
        pytest.param(lambda: shifted_cartesian_gate_layout(arrangement.ODD_COLUMN, (3, 3, 1)), id="shifted"),
        pytest.param(lambda: hexagonal_gate_layout(arrangement.EVEN_ROW, (3, 3, 1)), id="hexagonal"),
    ],
)
def test_gate_function_owns_truth_table_and_preserves_input_holes(make_layout: Callable[[], GateLayout]) -> None:
    """Keep the declared truth-table arguments when connections are missing."""
    layout = make_layout()
    a = layout.create_pi("a", (0, 0))
    b = layout.create_pi("b", (1, 0))
    c = layout.create_pi("c", (2, 0))
    function = dynamic_truth_table(3)
    function.create_from_hex_string("ac")
    gate = layout.create_gate([a], function, (1, 1))
    function.create_from_hex_string("00")
    assert layout.object_function(gate).to_hex() == "ac"
    returned = layout.object_function(gate)
    returned.create_from_hex_string("ff")
    assert layout.object_function(gate).to_hex() == "ac"
    assert layout.input_count(gate) == 3
    assert layout.fanin_size(gate) == 1
    assert layout.source(LayoutInputPort(gate, 0)) == a
    assert layout.source(LayoutInputPort(gate, 1)) is None
    assert layout.source(LayoutInputPort(gate, 2)) is None
    layout.connect(b, LayoutInputPort(gate, 2))
    layout.connect(c, LayoutInputPort(gate, 1))
    layout.disconnect(LayoutInputPort(gate, 1))
    assert layout.input_count(gate) == 3
    assert layout.fanin_size(gate) == 2
    assert layout.source(LayoutInputPort(gate, 0)) == a
    assert layout.source(LayoutInputPort(gate, 1)) is None
    assert layout.source(LayoutInputPort(gate, 2)) == b
    assert layout.inputs(gate) == [a, None, b]
    layout.connect(a, LayoutInputPort(gate, 2))
    assert layout.inputs(gate) == [a, None, a]
    sinks = layout.sinks(a)
    assert len(sinks) == 2
    assert LayoutInputPort(gate, 0) in sinks
    assert LayoutInputPort(gate, 2) in sinks
    layout.connect(b, LayoutInputPort(gate, 2))
    with pytest.raises(IndexError, match="arity"):
        layout.connect(a, LayoutInputPort(gate, 3))
    assert layout.source(LayoutInputPort(gate, 2)) == b
    assert layout.fanin_size(gate) == 2


def test_gate_move_and_removal_preserve_identity_contract() -> None:
    """Move an object without changing endpoints and reject stale identities after removal."""
    layout = cartesian_gate_layout((3, 2, 1))
    source = layout.create_pi("a", (0, 0))
    wire = layout.create_buf(source, (1, 0))
    output = layout.create_po(wire, "f", (2, 0))
    moved = layout.move_object(wire, (-10, 20, 3))
    assert moved == wire
    assert layout.find_object((1, 0)) is None
    assert layout.find_object((-10, 20, 3)) == wire
    assert layout.source(LayoutInputPort(output, 0)) == wire
    assert layout.source(LayoutInputPort(wire, 0)) == source
    layout.remove(wire)
    assert not layout.contains(wire)
    assert layout.source(LayoutInputPort(output, 0)) is None
    replacement = layout.create_buf(source, (-10, 20, 3))
    assert replacement != wire
    with pytest.raises(ValueError, match="identity"):
        layout.get_tile(wire)
    with pytest.raises(ValueError, match="identity"):
        layout.connect(wire, LayoutInputPort(output, 0))
    layout.connect(replacement, LayoutInputPort(output, 0))
    assert layout.source(LayoutInputPort(output, 0)) == replacement


@pytest.mark.parametrize("copy_layout", [lambda layout: layout.clone(), copy.copy, copy.deepcopy])
def test_gate_interface_order_and_clone_ownership(copy_layout: Callable[[GateLayout], GateLayout]) -> None:
    """Keep declared interfaces independent of allocation and copied layout edits."""
    layout = cartesian_gate_layout((3, 3, 1), "2DDWave", "original")
    b = layout.create_pi("b", (1, 0))
    a = layout.create_pi("a", (0, 1))
    gate = layout.create_lt(a, b, (1, 1))
    passthrough = layout.create_po(a, "pass", (0, 2))
    result = layout.create_po(gate, "compare", (2, 1))
    layout.set_input_order([a, b])
    layout.set_output_order([result, passthrough])
    assert layout.pis() == [a, b]
    assert layout.pos() == [result, passthrough]
    assert [layout.get_input_name(i) for i in range(2)] == ["a", "b"]
    assert [layout.get_output_name(i) for i in range(2)] == ["compare", "pass"]
    assert layout.source(LayoutInputPort(gate, 0)) == a
    assert layout.source(LayoutInputPort(gate, 1)) == b
    with pytest.raises(ValueError, match="terminal"):
        layout.set_input_order([a, a])
    with pytest.raises(ValueError, match="terminal"):
        layout.set_output_order([result])
    clone = copy_layout(layout)
    clone.set_layout_name("copy")
    clone.set_input_name(0, "copy_a")
    clone.set_output_name(0, "copy_compare")
    clone.disconnect(LayoutInputPort(gate, 0))
    assert layout.get_layout_name() == "original"
    assert layout.get_input_name(0) == "a"
    assert layout.get_output_name(0) == "compare"
    assert layout.source(LayoutInputPort(gate, 0)) == a
    assert clone.source(LayoutInputPort(gate, 0)) is None
    assert clone.object_function(gate).to_hex() == layout.object_function(gate).to_hex()


def test_gate_creation_requires_placement_and_valid_ports() -> None:
    """Reject missing placement, duplicate placement, and invalid endpoints."""
    layout = cartesian_gate_layout((2, 1, 1))
    with pytest.raises(TypeError):
        cast("Callable[..., LayoutObjectId]", layout.create_pi)("unplaced")
    source = layout.create_pi("a", (0, 0))
    with pytest.raises(ValueError, match="occupied"):
        layout.create_pi("occupied", (0, 0))
    with pytest.raises(TypeError):
        layout.create_po(cast("LayoutObjectId", coordinate(0, 0)), "f", (1, 0))
    output = layout.create_po(source, "f", (1, 0))
    with pytest.raises(ValueError, match="identity"):
        layout.get_tile(LayoutObjectId(source.index, 0))
    assert layout.source(LayoutInputPort(output, 0)) == source


def test_gate_constant_function_requires_an_explicit_placed_object() -> None:
    """Keep an empty layout empty and represent a constant with a placed zero-input function."""
    layout = cartesian_gate_layout((1, 1, 1))
    assert layout.size() == 0
    assert layout.gates() == []
    assert layout.wires() == []
    function = dynamic_truth_table(0)
    function.create_from_hex_string("1")
    constant = layout.create_gate([], function, (0, 0))
    assert layout.size() == 1
    assert layout.input_count(constant) == 0
    assert layout.object_function(constant).num_vars() == 0
    assert layout.object_function(constant).to_hex() == "1"
    assert layout.gates() == [constant]
    assert layout.wires() == []


@pytest.mark.parametrize(
    "make_layout",
    [
        pytest.param(lambda: cartesian_gate_layout((2, 2, 1), "unknown-scheme"), id="cartesian"),
        pytest.param(
            lambda: shifted_cartesian_gate_layout(arrangement.ODD_COLUMN, (2, 2, 1), "unknown-scheme"), id="shifted"
        ),
        pytest.param(lambda: hexagonal_gate_layout(arrangement.EVEN_ROW, (2, 2, 1), "unknown-scheme"), id="hexagonal"),
    ],
)
def test_unknown_clocking_scheme_rejects_construction(make_layout: Callable[[], GateLayout]) -> None:
    """Every gate-layout geometry rejects an unknown scheme with ValueError."""
    with pytest.raises(ValueError, match="clocking scheme"):
        make_layout()
