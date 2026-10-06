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
    LayoutInputPort,
    arrangement,
    cartesian_gate_layout,
    coordinate,
    hexagonal_gate_layout,
    shifted_cartesian_gate_layout,
)
from mnt.pyfiction.verification import critical_path_length_and_throughput, gate_level_drv_params, gate_level_drvs

if TYPE_CHECKING:
    from collections.abc import Callable
    from typing import TypeAlias

GateLayout: TypeAlias = cartesian_gate_layout | shifted_cartesian_gate_layout | hexagonal_gate_layout

OBSTRUCTION_LAYOUTS = [
    pytest.param(
        lambda: cartesian_gate_layout((4, 4, 2), "2DDWave", "Layout"),
        id="cartesian_gate_layout",
    ),
    pytest.param(
        lambda: shifted_cartesian_gate_layout(arrangement.ODD_COLUMN, (4, 4, 2), "2DDWave", "Layout"),
        id="shifted_cartesian_gate_layout",
    ),
    pytest.param(
        lambda: hexagonal_gate_layout(arrangement.EVEN_ROW, (4, 4, 2), "2DDWave", "Layout"),
        id="hexagonal_gate_layout",
    ),
]


@pytest.mark.parametrize(
    "make_layout",
    [
        pytest.param(
            lambda: cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout"),
            id="cartesian_gate_layout",
        ),
        pytest.param(
            lambda: shifted_cartesian_gate_layout(arrangement.ODD_COLUMN, (3, 3, 1), "2DDWave", "Layout"),
            id="shifted_cartesian_gate_layout",
        ),
        pytest.param(
            lambda: hexagonal_gate_layout(arrangement.EVEN_ROW, (3, 3, 1), "2DDWave", "Layout"),
            id="hexagonal_gate_layout",
        ),
    ],
)
def test_gate_layout_clocking_inheritance(make_layout: Callable[[], GateLayout]) -> None:
    """Expose clocked geometry through obstruction-aware gate layouts."""
    layout = make_layout()
    assert layout.incoming_clocked_zones((0, 0)) == []
    assert layout.outgoing_clocked_zones((2, 2)) == []

    for icz in layout.incoming_clocked_zones((1, 1)):
        assert icz in [coordinate(1, 0), coordinate(0, 1)]

    for icz in layout.outgoing_clocked_zones((1, 1)):
        assert icz in [coordinate(1, 2), coordinate(2, 1)]


@pytest.mark.parametrize("make_layout", OBSTRUCTION_LAYOUTS)
def test_obstructed_coordinates(make_layout: Callable[[], GateLayout]) -> None:
    """Keep manual coordinate obstructions."""
    layout = make_layout()
    for c in layout.coordinates():
        assert not layout.is_obstructed_coordinate(c)

    layout.obstruct_coordinate((0, 0))
    layout.obstruct_coordinate((1, 1))
    layout.obstruct_coordinate((2, 2))

    assert layout.is_obstructed_coordinate((0, 0))
    assert layout.is_obstructed_coordinate((1, 1))
    assert layout.is_obstructed_coordinate((2, 2))


@pytest.mark.parametrize("make_layout", OBSTRUCTION_LAYOUTS)
def test_obstructed_connections(make_layout: Callable[[], GateLayout]) -> None:
    """Keep manual connection obstructions."""
    layout = make_layout()
    for c1 in layout.coordinates():
        for c2 in layout.coordinates():
            assert not layout.is_obstructed_connection(c1, c2)

    layout.obstruct_connection((0, 0), (1, 1))
    layout.obstruct_connection((1, 1), (2, 2))

    assert layout.is_obstructed_connection((0, 0), (1, 1))
    assert layout.is_obstructed_connection((1, 1), (2, 2))


@pytest.mark.parametrize("make_layout", OBSTRUCTION_LAYOUTS)
def test_obstruction_via_gates(make_layout: Callable[[], GateLayout]) -> None:
    """Treat placed objects and physical connections as implicit obstructions."""
    layout = make_layout()
    x1 = layout.create_pi("x1", (0, 1))
    x2 = layout.create_pi("x2", (3, 2))
    x3 = layout.create_pi("x3", (2, 0))

    buf1 = layout.create_buf(x3, (2, 1))
    layout.create_buf(buf1, (2, 2))

    layout.create_and(x1, x2, (3, 3))

    assert layout.is_obstructed_coordinate((0, 1))
    assert layout.is_obstructed_coordinate((3, 2))
    assert layout.is_obstructed_coordinate((2, 0))
    assert layout.is_obstructed_coordinate((2, 1))
    assert layout.is_obstructed_coordinate((2, 2))
    assert layout.is_obstructed_coordinate((3, 3))

    assert layout.is_obstructed_connection((2, 0), (2, 1))
    assert layout.is_obstructed_connection((2, 1), (2, 2))
    assert layout.is_obstructed_connection((3, 2), (3, 3))


@pytest.mark.parametrize("make_layout", OBSTRUCTION_LAYOUTS)
def test_gate_level_inheritance(make_layout: Callable[[], GateLayout]) -> None:
    """Expose placed identities, physical flow, timing, and validation."""
    layout = make_layout()

    assert layout.is_empty()

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

    assert layout.pis() == [x1.object, x2.object, x3.object, x4.object]
    assert layout.pos() == [f1.object, f2.object]
    gates = layout.gates()
    assert len(gates) == 2
    assert a1.object in gates
    assert a2.object in gates
    wires = layout.wires()
    assert len(wires) == 9
    for port in (x1, x2, x3, x4, b1, b2, c, f1, f2):
        assert port.object in wires
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
        assert layout.find_object(position) == port.object
        assert layout.get_tile(port.object) == position
        assert layout.output(port.object) == port

    # incoming data flow
    inx1 = layout.fanins(coordinate(1, 0))
    assert len(inx1) == 0

    inf1 = layout.fanins(coordinate(3, 1))
    assert len(inf1) == 1
    assert coordinate(2, 1, 1) in inf1

    ina2 = layout.fanins(coordinate(2, 2))
    assert len(ina2) == 2
    assert coordinate(2, 1) in ina2
    assert coordinate(1, 2) in ina2

    # outgoing data flow
    outx1 = layout.fanouts(coordinate(1, 0))
    assert len(outx1) == 1
    assert coordinate(1, 1) in outx1

    outf1 = layout.fanouts(coordinate(3, 1))
    assert len(outf1) == 0

    outa2 = layout.fanouts(coordinate(2, 2))
    assert len(outa2) == 1
    assert coordinate(3, 2) in outa2

    cp, tp = critical_path_length_and_throughput(layout)
    assert cp == 4
    assert tp == 1

    drv_params = gate_level_drv_params()
    assert gate_level_drvs(layout, drv_params) == (0, 0)


@pytest.mark.parametrize("make_layout", OBSTRUCTION_LAYOUTS)
def test_disconnect_and_move_update_implicit_obstructions(make_layout: Callable[[], GateLayout]) -> None:
    """Track physical connections and moved placements through typed endpoints."""
    layout = make_layout()
    source = layout.create_pi("a", (0, 0))
    wire = layout.create_buf(source, (1, 0))
    output = layout.create_po(wire, "f", (1, 1))
    assert layout.is_obstructed_connection((0, 0), (1, 0))
    assert layout.is_obstructed_connection((1, 0), (1, 1))
    layout.disconnect(LayoutInputPort(output.object, 0))
    assert layout.source(LayoutInputPort(output.object, 0)) is None
    assert not layout.is_obstructed_connection((1, 0), (1, 1))
    assert layout.is_obstructed_coordinate((1, 1))
    layout.move_node(wire.object, (0, 1))
    assert not layout.is_obstructed_coordinate((1, 0))
    assert layout.is_obstructed_coordinate((0, 1))
    assert not layout.is_obstructed_connection((0, 0), (1, 0))
    assert layout.is_obstructed_connection((0, 0), (0, 1))
    layout.connect(wire, LayoutInputPort(output.object, 0))
    assert layout.is_obstructed_connection((0, 1), (1, 1))
    layout.remove(wire.object)
    assert layout.source(LayoutInputPort(output.object, 0)) is None
    assert not layout.is_obstructed_coordinate((0, 1))
    assert not layout.is_obstructed_connection((0, 1), (1, 1))


@pytest.mark.parametrize("make_layout", OBSTRUCTION_LAYOUTS)
def test_clearing_manual_obstructions_preserves_objects_and_ports(make_layout: Callable[[], GateLayout]) -> None:
    """Clear manual obstructions without changing declared placement or topology."""
    layout = make_layout()
    source = layout.create_pi("a", (0, 0))
    wire = layout.create_buf(source, (1, 0))
    layout.obstruct_coordinate((1, 0))
    layout.obstruct_connection((0, 0), (1, 0))
    layout.clear_obstructed_coordinate((1, 0))
    layout.clear_obstructed_connection((0, 0), (1, 0))
    assert layout.contains(wire.object)
    assert layout.source(LayoutInputPort(wire.object, 0)) == source
    assert layout.is_obstructed_coordinate((1, 0))
    assert layout.is_obstructed_connection((0, 0), (1, 0))
    layout.disconnect(LayoutInputPort(wire.object, 0))
    assert not layout.is_obstructed_connection((0, 0), (1, 0))
    layout.remove(wire.object)
    assert not layout.is_obstructed_coordinate((1, 0))


@pytest.mark.parametrize("make_layout", OBSTRUCTION_LAYOUTS)
def test_clone_owns_obstructions_objects_and_connections(make_layout: Callable[[], GateLayout]) -> None:
    """Keep manual and implicit obstructions independent in a cloned layout."""
    layout = make_layout()
    source = layout.create_pi("a", (0, 0))
    wire = layout.create_buf(source, (1, 0))
    output = layout.create_po(wire, "f", (2, 0))
    layout.obstruct_coordinate((0, 2))
    layout.obstruct_connection((0, 0), (0, 1))
    clone = layout.clone()
    clone.clear_obstructed_coordinates()
    clone.clear_obstructed_connections()
    clone.remove(wire.object)
    assert layout.is_obstructed_coordinate((0, 2))
    assert layout.is_obstructed_connection((0, 0), (0, 1))
    assert not clone.is_obstructed_coordinate((0, 2))
    assert not clone.is_obstructed_connection((0, 0), (0, 1))
    assert layout.is_obstructed_coordinate((1, 0))
    assert not clone.is_obstructed_coordinate((1, 0))
    assert layout.source(LayoutInputPort(output.object, 0)) == wire
    assert clone.source(LayoutInputPort(output.object, 0)) is None
