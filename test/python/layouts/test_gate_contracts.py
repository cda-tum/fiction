# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import copy
from operator import attrgetter
from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.layouts import coordinate, layout_input_port, num_adjacent_coordinates
from mnt.pyfiction.synthesis import dynamic_truth_table

if TYPE_CHECKING:
    from collections.abc import Callable


@pytest.mark.parametrize(
    ("kind", "literal"),
    [
        ("and", "8"),
        ("nand", "7"),
        ("or", "e"),
        ("nor", "1"),
        ("lt", "2"),
        ("ge", "d"),
        ("gt", "4"),
        ("le", "b"),
        ("xor", "6"),
        ("xnor", "9"),
    ],
)
def test_elementary_truth_tables_keep_input_order(gate_factory: Callable, kind: str, literal: str) -> None:
    layout = gate_factory((3, 3))
    a = layout.create_pi("a", (0, 0))
    b = layout.create_pi("b", (1, 0))
    gate = getattr(layout, f"create_{kind}")(a, b, (1, 1))
    assert getattr(layout, f"is_{kind}")(gate)
    assert layout.object_function(gate).to_hex() == literal
    assert layout.inputs(gate) == [a, b]
    assert layout.gates() == [gate]
    assert layout.num_gates() == 1
    assert layout.is_gate_tile((1, 1))
    assert not layout.is_wire_tile((1, 1))


def test_wire_and_gate_classification_through_removal(gate_factory: Callable) -> None:
    layout = gate_factory((5, 2))
    source = layout.create_pi("a", (0, 0))
    wire = layout.create_buf(source, (1, 0))
    output = layout.create_po(wire, "f", (2, 0))
    inverter = layout.create_not(source, (3, 0))
    identity = dynamic_truth_table(1)
    identity.create_from_hex_string("2")
    generic_wire = layout.create_gate([source], identity, (4, 0))
    assert layout.object_function(inverter).to_hex() == "1"
    assert layout.is_fanout(source)
    key = attrgetter("index", "generation")
    assert sorted(layout.wires(), key=key) == sorted([wire, generic_wire], key=key)
    assert layout.gates() == [inverter]
    assert not layout.is_wire_tile((0, 0))
    assert not layout.is_wire_tile((2, 0))
    assert layout.is_pi_tile((0, 0))
    assert layout.is_po_tile((2, 0))
    for obj, counts in [(source, (2, 1)), (output, (2, 1)), (wire, (1, 1)), (generic_wire, (0, 1)), (inverter, (0, 0))]:
        layout.remove(obj)
        assert (layout.num_wires(), layout.num_gates()) == counts
    assert layout.is_empty()
    layout.create_pi("replacement", (1, 0))
    assert (layout.num_wires(), layout.num_gates()) == (0, 0)


def test_terminal_removal_and_invalid_order_are_atomic(gate_factory: Callable) -> None:
    layout = gate_factory((4, 2))
    a, b, c = [layout.create_pi(name, (x, 0)) for x, name in enumerate(("a", "b", "c"))]
    output = layout.create_po(c, "result", (2, 1))
    layout.set_input_order([c, a, b])
    for invalid in ([a, a, b], [a], [a, b, output]):
        with pytest.raises(ValueError, match=r"terminal|role"):
            layout.set_input_order(invalid)
        assert layout.pis() == [c, a, b]
    layout.remove(a)
    assert layout.pis() == [c, b]
    assert [layout.pi_at(i) for i in range(2)] == [c, b]
    assert layout.po_at(0) == output
    layout.set_name(b, "")
    assert not layout.get_input_name(1)
    layout.remove(output)
    assert layout.pos() == []
    assert layout.fanout_size(c) == 0
    with pytest.raises(IndexError):
        layout.pi_at(2)
    with pytest.raises(IndexError):
        layout.get_output_name(0)


def test_duplicate_inputs_and_self_loop_removal(gate_factory: Callable) -> None:
    layout = gate_factory((2, 1))
    source = layout.create_pi("a", (0, 0))
    gate = layout.create_and(source, source, (1, 0))
    assert layout.fanout_size(source) == 2
    assert sorted(layout.sinks(source), key=lambda port: port.index) == [
        layout_input_port(gate, 0),
        layout_input_port(gate, 1),
    ]
    layout.disconnect(layout_input_port(gate, 0))
    assert layout.fanout_size(source) == 1
    assert layout.inputs(gate) == [None, source]
    layout.connect(gate, layout_input_port(gate, 0))
    layout.remove(gate)
    assert layout.fanout_size(source) == 0
    assert layout.size() == 1


def test_large_arity_ports_survive_copy_removal_and_reuse(gate_factory: Callable) -> None:
    layout = gate_factory((6, 3))
    inputs = [layout.create_pi(str(x), (x, 0)) for x in range(5)]
    parity = dynamic_truth_table(5)
    parity.create_from_hex_string("96696996")
    gate = layout.create_gate(inputs, parity, (2, 2))
    layout.disconnect(layout_input_port(gate, 3))
    assert layout.inputs(gate) == [*inputs[:3], None, inputs[4]]
    assert layout.fanin_size(gate) == 4
    assert layout.input_count(gate) == 5
    with pytest.raises(IndexError, match="arity"):
        layout.source(layout_input_port(gate, 5))
    clone = copy.copy(layout)
    clone.disconnect(layout_input_port(gate, 0))
    assert layout.source(layout_input_port(gate, 0)) == inputs[0]
    clone.remove(gate)
    assert [clone.fanout_size(source) for source in inputs] == [0] * 5
    replacement = clone.create_gate([inputs[1]], parity, (2, 2))
    assert replacement != gate
    assert clone.inputs(replacement) == [inputs[1], None, None, None, None]
    clone.remove(replacement)
    wire = clone.create_buf(inputs[0], (2, 2))
    assert clone.input_count(wire) == 1
    assert clone.inputs(wire) == [inputs[0]]


def test_occupied_move_failure_preserves_topology(gate_factory: Callable) -> None:
    layout = gate_factory((1, 1))
    source = layout.create_pi("a", (-(2**31), 0))
    wire = layout.create_buf(source, (99, 0, 3))
    assert layout.inputs(wire) == [source]
    assert layout.sinks(source) == [layout_input_port(wire, 0)]
    with pytest.raises(ValueError, match="occupied"):
        layout.move_object(wire, (-(2**31), 0))
    assert layout.get_tile(wire) == coordinate(99, 0, 3)
    assert layout.size() == 2
    assert layout.inputs(wire) == [source]
    assert layout.find_object((0, 0)) is None
    elevated = layout.create_pi("elevated", (99, 0, 2))
    layout.clear_tile((99, 0, 3))
    assert not layout.contains(wire)
    assert layout.find_object((99, 0, 2)) == elevated
    assert layout.fanout_size(source) == 0


def test_crossing_counts_follow_edits_and_copies(gate_factory: Callable) -> None:
    layout = gate_factory((4, 2, 2))
    upper = layout.create_buf((0, 0, 1))
    assert layout.num_crossings() == 0
    lower = layout.create_pi("a", (0, 0))
    assert layout.num_crossings() == 1
    third = layout.create_buf((0, 0, 2))
    assert layout.num_crossings() == 1
    layout.move_object(third, (1, 0))
    layout.move_object(upper, (1, 0, 1))
    assert layout.num_crossings() == 1
    layout.move_object(third, (1, 0, 2))
    assert layout.num_crossings() == 0
    layout.move_object(third, (1, 0))
    with pytest.raises(ValueError, match="occupied"):
        layout.move_object(lower, (1, 0))
    assert layout.num_crossings() == 1
    clone = layout.clone()
    layout.remove(upper)
    assert layout.num_crossings() == 0
    assert clone.num_crossings() == 1
    clone.remove(third)
    assert clone.num_crossings() == 0


def test_crossing_counts_exclude_elevated_terminals(gate_factory: Callable) -> None:
    layout = gate_factory((4, 2, 2))
    for x in range(4):
        layout.create_buf((x, 0))
    layout.create_pi("a", (0, 0, 1))
    layout.create_po("f", (1, 0, 1))
    layout.create_buf((2, 0, 1))
    identity = dynamic_truth_table(1)
    identity.create_from_hex_string("2")
    layout.create_gate([], identity, (3, 0, 1))
    assert layout.num_wires() == 6
    assert layout.num_crossings() == 2


def test_sparse_iteration_preserves_membership(gate_factory: Callable) -> None:
    layout = gate_factory((0, 0, 0))
    objects = [layout.create_buf((x, 0)) for x in range(130)]
    kept = [objects[i] for i in (0, 63, 64, 127, 129)]
    for obj in objects:
        if obj not in kept:
            layout.remove(obj)
    replacement = layout.create_buf((1, 0))
    expected = sorted([*kept, replacement], key=lambda obj: (obj.index, obj.generation))
    clone = layout.clone()
    for candidate in (layout, clone, copy.copy(layout), copy.deepcopy(layout)):
        assert sorted(candidate.wires(), key=lambda obj: (obj.index, obj.generation)) == expected
        assert candidate.size() == len(expected)
    layout.remove(replacement)
    assert clone.contains(replacement)


def test_gate_copies_isolate_clocking_geometry_and_constraints(gate_factory: Callable) -> None:
    layout = gate_factory((6, 6), "2DDWave", "original")
    source = layout.create_pi("a", (0, 0))
    gate = layout.create_not(source, (1, 0))
    layout.assign_clock_number((0, 0), 3)
    layout.assign_synchronization_element((0, 0), 2)
    layout.obstruct_coordinate((2, 2))
    layout.obstruct_connection((0, 0), (0, 1))
    for clone in (layout.clone(), copy.copy(layout), copy.deepcopy(layout)):
        clone.resize((11, 11, 2))
        clone.move_object(gate, (2, 2))
        clone.disconnect(layout_input_port(gate, 0))
        clone.assign_clock_number((0, 0), 1)
        clone.assign_synchronization_element((0, 0), 4)
        clone.clear_obstructed_coordinates()
        clone.clear_obstructed_connections()
        assert layout.width() == 6
        assert layout.get_tile(gate) == coordinate(1, 0)
        assert layout.inputs(gate) == [source]
        assert layout.get_clock_number((0, 0)) == 3
        assert layout.get_synchronization_element((0, 0)) == 2
        assert layout.is_obstructed_coordinate((2, 2))
        assert layout.is_obstructed_connection((0, 0), (0, 1))
        clone.replace_clocking_scheme("USE")
        assert clone.is_clocking_scheme("USE")
        assert layout.is_clocking_scheme("2DDWAVE")


@pytest.mark.parametrize(
    "direction", ["north", "north_east", "east", "south_east", "south", "south_west", "west", "north_west"]
)
def test_physical_signal_directions(gate_factory: Callable, direction: str) -> None:
    layout = gate_factory((5, 5, 2), "OPEN")
    center = coordinate(2, 2)
    target = getattr(layout, direction)(center)
    assert target is not None
    source = layout.create_pi("a", center)
    wire = layout.create_buf(source, target)
    layout.assign_clock_number(center, 0)
    layout.assign_clock_number(target, 1)
    adjacent = layout.is_adjacent_of(center, target)
    assert layout.outgoing_data_flow(center) == ([target] if adjacent else [])
    assert layout.incoming_data_flow(target) == ([center] if adjacent else [])
    assert layout.is_outgoing_signal(center, target) == adjacent
    assert layout.is_incoming_signal(target, center) == adjacent
    assert layout.has_no_outgoing_signal(center) == (not adjacent)
    assert layout.has_no_incoming_signal(target) == (not adjacent)
    adjective = {"north": "northern", "east": "eastern", "south": "southern", "west": "western"}.get(
        direction, direction + "ern"
    )
    assert getattr(layout, f"has_{adjective}_outgoing_signal")(center) == adjacent
    assert not layout.is_incoming_signal(center, None)
    assert not layout.is_outgoing_signal(target, None)
    assert num_adjacent_coordinates(layout, center) == (4 if type(layout).__name__ == "cartesian_gate_layout" else 6)
    layout.disconnect(layout_input_port(wire, 0))
    assert layout.outgoing_data_flow(center) == []
    assert layout.incoming_data_flow(target) == []
