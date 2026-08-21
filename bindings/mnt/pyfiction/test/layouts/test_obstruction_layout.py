# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction import (
    cartesian_gate_layout,
    cartesian_obstruction_layout,
    critical_path_length_and_throughput,
    gate_level_drv_params,
    gate_level_drvs,
    hexagonal_gate_layout,
    hexagonal_obstruction_layout,
    offset_coordinate,
    shifted_cartesian_gate_layout,
    shifted_cartesian_obstruction_layout,
)

OBSTRUCTION_LAYOUTS = [
    pytest.param(
        lambda: cartesian_obstruction_layout(cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout")),
        id="cartesian_obstruction_layout",
    ),
    pytest.param(
        lambda: shifted_cartesian_obstruction_layout(shifted_cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout")),
        id="shifted_cartesian_obstruction_layout",
    ),
    pytest.param(
        lambda: hexagonal_obstruction_layout(hexagonal_gate_layout((3, 3, 1), "2DDWave", "Layout")),
        id="hexagonal_obstruction_layout",
    ),
]


@pytest.mark.parametrize(
    "make_layout",
    [
        pytest.param(
            lambda: cartesian_obstruction_layout(cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout")),
            id="cartesian_obstruction_layout",
        ),
        pytest.param(
            lambda: shifted_cartesian_obstruction_layout(shifted_cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout")),
            id="shifted_cartesian_obstruction_layout",
        ),
        pytest.param(
            lambda: hexagonal_obstruction_layout(hexagonal_gate_layout((2, 2, 0), "2DDWave", "Layout")),
            id="hexagonal_obstruction_layout",
        ),
    ],
)
def test_obstruction_layout_clocking_inheritance(make_layout):
    layout = make_layout()
    assert layout.incoming_clocked_zones((0, 0)) == []
    assert layout.outgoing_clocked_zones((2, 2)) == []

    for icz in layout.incoming_clocked_zones((1, 1)):
        assert icz in [offset_coordinate(1, 0), offset_coordinate(0, 1)]

    for icz in layout.outgoing_clocked_zones((1, 1)):
        assert icz in [offset_coordinate(1, 2), offset_coordinate(2, 1)]


@pytest.mark.parametrize("make_layout", OBSTRUCTION_LAYOUTS)
def test_obstructed_coordinates(make_layout):
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
def test_obstructed_connections(make_layout):
    layout = make_layout()
    for c1 in layout.coordinates():
        for c2 in layout.coordinates():
            assert not layout.is_obstructed_connection(c1, c2)

    layout.obstruct_connection((0, 0), (1, 1))
    layout.obstruct_connection((1, 1), (2, 2))

    assert layout.is_obstructed_connection((0, 0), (1, 1))
    assert layout.is_obstructed_connection((1, 1), (2, 2))


@pytest.mark.parametrize("make_layout", OBSTRUCTION_LAYOUTS)
def test_obstruction_via_gates(make_layout):
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


def test_cartesian_obstruction_layout_gate_level_inheritance():
    layout = cartesian_obstruction_layout(cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout"))

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

    # Pis
    pis = layout.pis()
    assert len(pis) == 4
    assert offset_coordinate(x1) in pis
    assert offset_coordinate(x2) in pis
    assert offset_coordinate(x3) in pis
    assert offset_coordinate(x4) in pis
    assert layout.get_node(offset_coordinate(x1)) == 2
    assert layout.get_node(offset_coordinate(x2)) == 3
    assert layout.get_node(offset_coordinate(x3)) == 4
    assert layout.get_node(offset_coordinate(x4)) == 5
    assert layout.get_tile(2) == offset_coordinate(x1)
    assert layout.get_tile(3) == offset_coordinate(x2)
    assert layout.get_tile(4) == offset_coordinate(x3)
    assert layout.get_tile(5) == offset_coordinate(x4)
    assert layout.make_signal(2) == x1
    assert layout.make_signal(3) == x2
    assert layout.make_signal(4) == x3
    assert layout.make_signal(5) == x4

    # POs
    pos = layout.pos()
    assert len(pos) == 2
    assert offset_coordinate(f1) in pos
    assert offset_coordinate(f2) in pos
    assert layout.get_node(offset_coordinate(f1)) == 11
    assert layout.get_node(offset_coordinate(f2)) == 12
    assert layout.get_tile(11) == offset_coordinate(f1)
    assert layout.get_tile(12) == offset_coordinate(f2)
    assert layout.make_signal(11) == f1
    assert layout.make_signal(12) == f2

    # gates
    gates = layout.gates()
    assert len(gates) == 7
    assert offset_coordinate(a1) in gates
    assert offset_coordinate(a2) in gates
    assert offset_coordinate(b1) in gates
    assert offset_coordinate(b2) in gates
    assert offset_coordinate(c) in gates
    assert offset_coordinate(f1) in gates
    assert offset_coordinate(f2) in gates
    assert layout.get_node(offset_coordinate(a1)) == 6
    assert layout.get_node(offset_coordinate(b1)) == 7
    assert layout.get_node(offset_coordinate(b2)) == 8
    assert layout.get_node(offset_coordinate(a2)) == 9
    assert layout.get_node(offset_coordinate(c)) == 10
    assert layout.get_tile(6) == offset_coordinate(a1)
    assert layout.get_tile(7) == offset_coordinate(b1)
    assert layout.get_tile(8) == offset_coordinate(b2)
    assert layout.get_tile(9) == offset_coordinate(a2)
    assert layout.get_tile(10) == offset_coordinate(c)
    assert layout.make_signal(6) == a1
    assert layout.make_signal(7) == b1
    assert layout.make_signal(8) == b2
    assert layout.make_signal(9) == a2
    assert layout.make_signal(10) == c

    # wires
    wires = layout.wires()
    assert len(wires) == 9
    assert offset_coordinate(x1) in wires
    assert offset_coordinate(x2) in wires
    assert offset_coordinate(x3) in wires
    assert offset_coordinate(x4) in wires
    assert offset_coordinate(b1) in wires
    assert offset_coordinate(b2) in wires
    assert offset_coordinate(c) in wires
    assert offset_coordinate(f1) in wires
    assert offset_coordinate(f2) in wires

    # incoming data flow
    inx1 = layout.fanins(offset_coordinate(x1))
    assert len(inx1) == 0

    inf1 = layout.fanins(offset_coordinate(f1))
    assert len(inf1) == 1
    assert offset_coordinate(c) in inf1

    ina2 = layout.fanins(offset_coordinate(a2))
    assert len(ina2) == 2
    assert offset_coordinate(b1) in ina2
    assert offset_coordinate(b2) in ina2

    # outgoing data flow
    outx1 = layout.fanouts(offset_coordinate(x1))
    assert len(outx1) == 1
    assert offset_coordinate(a1) in outx1

    outf1 = layout.fanouts(offset_coordinate(f1))
    assert len(outf1) == 0

    outa2 = layout.fanouts(offset_coordinate(a2))
    assert len(outa2) == 1
    assert offset_coordinate(f2) in outa2

    cp, tp = critical_path_length_and_throughput(layout)
    assert cp == 4
    assert tp == 1

    drv_params = gate_level_drv_params()
    assert gate_level_drvs(layout, drv_params) == (0, 0)


def test_hexagonal_obstruction_layout_gate_level_inheritance():
    layout = hexagonal_obstruction_layout(hexagonal_gate_layout((3, 3, 1), "2DDWave", "Layout"))

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

    # Pis
    pis = layout.pis()
    assert len(pis) == 4
    assert offset_coordinate(x1) in pis
    assert offset_coordinate(x2) in pis
    assert offset_coordinate(x3) in pis
    assert offset_coordinate(x4) in pis
    assert layout.get_node(offset_coordinate(x1)) == 2
    assert layout.get_node(offset_coordinate(x2)) == 3
    assert layout.get_node(offset_coordinate(x3)) == 4
    assert layout.get_node(offset_coordinate(x4)) == 5
    assert layout.get_tile(2) == offset_coordinate(x1)
    assert layout.get_tile(3) == offset_coordinate(x2)
    assert layout.get_tile(4) == offset_coordinate(x3)
    assert layout.get_tile(5) == offset_coordinate(x4)
    assert layout.make_signal(2) == x1
    assert layout.make_signal(3) == x2
    assert layout.make_signal(4) == x3
    assert layout.make_signal(5) == x4

    # POs
    pos = layout.pos()
    assert len(pos) == 2
    assert offset_coordinate(f1) in pos
    assert offset_coordinate(f2) in pos
    assert layout.get_node(offset_coordinate(f1)) == 11
    assert layout.get_node(offset_coordinate(f2)) == 12
    assert layout.get_tile(11) == offset_coordinate(f1)
    assert layout.get_tile(12) == offset_coordinate(f2)
    assert layout.make_signal(11) == f1
    assert layout.make_signal(12) == f2

    # gates
    gates = layout.gates()
    assert len(gates) == 7
    assert offset_coordinate(a1) in gates
    assert offset_coordinate(a2) in gates
    assert offset_coordinate(b1) in gates
    assert offset_coordinate(b2) in gates
    assert offset_coordinate(c) in gates
    assert offset_coordinate(f1) in gates
    assert offset_coordinate(f2) in gates
    assert layout.get_node(offset_coordinate(a1)) == 6
    assert layout.get_node(offset_coordinate(b1)) == 7
    assert layout.get_node(offset_coordinate(b2)) == 8
    assert layout.get_node(offset_coordinate(a2)) == 9
    assert layout.get_node(offset_coordinate(c)) == 10
    assert layout.get_tile(6) == offset_coordinate(a1)
    assert layout.get_tile(7) == offset_coordinate(b1)
    assert layout.get_tile(8) == offset_coordinate(b2)
    assert layout.get_tile(9) == offset_coordinate(a2)
    assert layout.get_tile(10) == offset_coordinate(c)
    assert layout.make_signal(6) == a1
    assert layout.make_signal(7) == b1
    assert layout.make_signal(8) == b2
    assert layout.make_signal(9) == a2
    assert layout.make_signal(10) == c

    # wires
    wires = layout.wires()
    assert len(wires) == 9
    assert offset_coordinate(x1) in wires
    assert offset_coordinate(x2) in wires
    assert offset_coordinate(x3) in wires
    assert offset_coordinate(x4) in wires
    assert offset_coordinate(b1) in wires
    assert offset_coordinate(b2) in wires
    assert offset_coordinate(c) in wires
    assert offset_coordinate(f1) in wires
    assert offset_coordinate(f2) in wires

    # incoming data flow
    inx1 = layout.fanins(offset_coordinate(x1))
    assert len(inx1) == 0

    inf1 = layout.fanins(offset_coordinate(f1))
    assert len(inf1) == 1
    assert offset_coordinate(c) in inf1

    ina2 = layout.fanins(offset_coordinate(a2))
    assert len(ina2) == 2
    assert offset_coordinate(b1) in ina2
    assert offset_coordinate(b2) in ina2

    # outgoing data flow
    outx1 = layout.fanouts(offset_coordinate(x1))
    assert len(outx1) == 1
    assert offset_coordinate(a1) in outx1

    outf1 = layout.fanouts(offset_coordinate(f1))
    assert len(outf1) == 0

    outa2 = layout.fanouts(offset_coordinate(a2))
    assert len(outa2) == 1
    assert offset_coordinate(f2) in outa2

    cp, tp = critical_path_length_and_throughput(layout)
    assert cp == 4
    assert tp == 1

    drv_params = gate_level_drv_params()
    assert gate_level_drvs(layout, drv_params) == (0, 0)
