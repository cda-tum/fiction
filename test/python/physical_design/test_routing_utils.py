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
from mnt.pyfiction.physical_design import clear_routing, extract_routing_objectives, is_crossable_wire, route_path
from mnt.pyfiction.physical_design.path_finding import a_star

if TYPE_CHECKING:
    from collections.abc import Callable


def test_empty_layout() -> None:
    lyt = cartesian_gate_layout((5, 3), "2DDWave")

    x1 = lyt.create_pi("x1", (0, 1))
    po = lyt.create_po(x1, "f1", (4, 1))

    route_path(lyt, [(0, 1), (1, 1), (2, 1), (3, 1), (4, 1)], layout_input_port(po, 0))

    for x, y in [(1, 1), (2, 1), (3, 1)]:
        assert lyt.is_wire_tile((x, y))


def test_empty_layout_a_star() -> None:
    lyt = cartesian_gate_layout((5, 3), "2DDWave")

    x1 = lyt.create_pi("x1", (0, 1))
    po = lyt.create_po(x1, "f1", (4, 1))

    route_path(lyt, a_star(lyt, (0, 1), (4, 1)), layout_input_port(po, 0))

    for x, y in [(1, 1), (2, 1), (3, 1)]:
        assert lyt.is_wire_tile((x, y))


@pytest.mark.parametrize(
    "make_layout",
    [
        pytest.param(lambda: cartesian_gate_layout((3, 1)), id="cartesian"),
        pytest.param(lambda: shifted_cartesian_gate_layout(arrangement.EVEN_ROW, (3, 1)), id="shifted"),
        pytest.param(lambda: hexagonal_gate_layout(arrangement.EVEN_ROW, (3, 1)), id="hexagonal"),
    ],
)
def test_clear_routing(
    make_layout: Callable[[], cartesian_gate_layout | shifted_cartesian_gate_layout | hexagonal_gate_layout],
) -> None:
    """Remove routing wires and preserve disconnected terminal identities."""
    layout = make_layout()
    source = layout.create_pi("source", (0, 0))
    wire = layout.create_buf(source, (1, 0))
    output = layout.create_po(wire, "output", (2, 0))

    clear_routing(layout)

    assert layout.find_object((0, 0)) == source
    assert layout.find_object((1, 0)) is None
    assert layout.find_object((2, 0)) == output
    assert layout.input_count(output) == 1
    assert layout.source(layout_input_port(output, 0)) is None


@pytest.mark.parametrize("repeated", [False, True])
def test_routing_retains_ordered_ports_and_identities(*, repeated: bool) -> None:
    """Reroute each logical input, including two occurrences of the same source."""
    layout = cartesian_gate_layout((5, 3, 2), "2DDWave")
    a = layout.create_pi("a", (0, 0))
    b = a if repeated else layout.create_pi("b", (0, 1))
    gate = layout.create_lt(a, b, (4, 1))
    output = layout.create_po(gate, "f", (4, 2))
    source_b = (0, 0) if repeated else (0, 1)
    expected = [((0, 0), (4, 1), 0), (source_b, (4, 1), 1), ((4, 1), (4, 2), 0)]
    assert sorted(extract_routing_objectives(layout)) == sorted(expected)
    clear_routing(layout)
    assert all(layout.contains(node) for node in (a, b, gate, output))
    assert layout.input_count(gate) == 2
    assert layout.source(layout_input_port(gate, 0)) is None
    assert layout.source(layout_input_port(gate, 1)) is None
    route_path(layout, [source_b, (1, 1), (4, 1)], layout_input_port(gate, 1))
    second_wire = layout.source(layout_input_port(gate, 1))
    assert second_wire is not None
    assert layout.source(layout_input_port(second_wire, 0)) == b
    assert layout.source(layout_input_port(gate, 0)) is None
    route_path(layout, [(0, 0), (1, 0), (4, 1)], layout_input_port(gate, 0))
    route_path(layout, [(4, 1), (4, 2)], layout_input_port(output, 0))
    first_wire = layout.source(layout_input_port(gate, 0))
    assert first_wire is not None
    assert first_wire != second_wire
    assert layout.source(layout_input_port(first_wire, 0)) == a
    assert layout.source(layout_input_port(gate, 1)) == second_wire
    assert layout.is_lt(gate)
    assert sorted(extract_routing_objectives(layout)) == sorted(expected)


@pytest.mark.parametrize(
    ("path", "index", "error"),
    [
        ([(0, 0), (1, 0), (4, 0)], 2, IndexError),
        ([(0, 0), (1, 0), (4, 1)], 0, ValueError),
        ([], 0, ValueError),
        ([(0, 0)], 0, ValueError),
        ([(0, 2), (1, 0), (4, 0)], 0, ValueError),
        ([(0, 0), (1, 0), (1, 0), (4, 0)], 0, ValueError),
    ],
)
def test_route_validation_precedes_mutation(path: list[tuple[int, int]], index: int, error: type[Exception]) -> None:
    """Malformed paths preserve the original destination and create no wires."""
    layout = cartesian_gate_layout((5, 3, 2))
    a = layout.create_pi("a", (0, 0))
    b = layout.create_pi("b", (0, 1))
    gate = layout.create_lt(a, b, (4, 0))
    with pytest.raises(error):
        route_path(layout, path, layout_input_port(gate, index))
    assert layout.size() == 3
    assert layout.source(layout_input_port(gate, 0)) == a
    assert layout.source(layout_input_port(gate, 1)) == b


def test_crossable_wire_and_occupied_route() -> None:
    """Cross connected wires, reject a fully occupied crossing, and retain existing wires."""
    layout = cartesian_gate_layout((3, 3, 2), "2DDWave")
    a = layout.create_pi("a", (1, 0))
    wire = layout.create_buf(a, (1, 1))
    layout.create_po(wire, "f", (1, 2))
    assert is_crossable_wire(layout, (0, 1), (1, 1))
    assert not is_crossable_wire(layout, (0, 0), (1, 0))
    source = layout.create_pi("b", (0, 1))
    output = layout.create_po(source, "g", (2, 1))
    route_path(layout, [(0, 1), (1, 1), (2, 1)], layout_input_port(output, 0))
    crossing = layout.find_object((1, 1, 1))
    assert crossing is not None
    assert layout.source(layout_input_port(crossing, 0)) == source
    assert layout.source(layout_input_port(output, 0)) == crossing
    with pytest.raises(ValueError, match="occupied"):
        route_path(layout, [(0, 1), (1, 1), (2, 1)], layout_input_port(output, 0))
    assert layout.find_object((1, 1)) == wire
    assert layout.source(layout_input_port(output, 0)) == crossing
