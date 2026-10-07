# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction.layouts import (
    arrangement,
    cartesian_gate_layout,
    coordinate,
    hexagonal_gate_layout,
    shifted_cartesian_gate_layout,
)
from mnt.pyfiction.verification import critical_path_length_and_throughput, gate_level_drv_params, gate_level_drvs

OBSTRUCTION_LAYOUTS = [
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
]


@pytest.mark.parametrize(
    "make_layout",
    [
        pytest.param(
            lambda: cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout"),
            id="cartesian_gate_layout",
        ),
        pytest.param(
            lambda: shifted_cartesian_gate_layout(arrangement.ODD_COLUMN, (2, 2, 0), "2DDWave", "Layout"),
            id="shifted_cartesian_gate_layout",
        ),
        pytest.param(
            lambda: hexagonal_gate_layout(arrangement.EVEN_ROW, (2, 2, 0), "2DDWave", "Layout"),
            id="hexagonal_gate_layout",
        ),
    ],
)
def test_gate_layout_clocking_inheritance(make_layout):
    layout = make_layout()
    assert layout.incoming_clocked_zones((0, 0)) == []
    assert layout.outgoing_clocked_zones((2, 2)) == []

    for icz in layout.incoming_clocked_zones((1, 1)):
        assert icz in [coordinate(1, 0), coordinate(0, 1)]

    for icz in layout.outgoing_clocked_zones((1, 1)):
        assert icz in [coordinate(1, 2), coordinate(2, 1)]


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


def test_cartesian_gate_layout_gate_level_inheritance():
    layout = cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout")

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
    assert coordinate(1, 0) in pis
    assert coordinate(0, 1) in pis
    assert coordinate(2, 0) in pis
    assert coordinate(0, 2) in pis
    assert layout.get_node(coordinate(1, 0)) == 2
    assert layout.get_node(coordinate(0, 1)) == 3
    assert layout.get_node(coordinate(2, 0)) == 4
    assert layout.get_node(coordinate(0, 2)) == 5
    assert layout.get_tile(2) == coordinate(1, 0)
    assert layout.get_tile(3) == coordinate(0, 1)
    assert layout.get_tile(4) == coordinate(2, 0)
    assert layout.get_tile(5) == coordinate(0, 2)
    assert layout.make_signal(2) == x1
    assert layout.make_signal(3) == x2
    assert layout.make_signal(4) == x3
    assert layout.make_signal(5) == x4

    # POs
    pos = layout.pos()
    assert len(pos) == 2
    assert coordinate(3, 1) in pos
    assert coordinate(3, 2) in pos
    assert layout.get_node(coordinate(3, 1)) == 11
    assert layout.get_node(coordinate(3, 2)) == 12
    assert layout.get_tile(11) == coordinate(3, 1)
    assert layout.get_tile(12) == coordinate(3, 2)
    assert layout.make_signal(11) == f1
    assert layout.make_signal(12) == f2

    # gates
    gates = layout.gates()
    assert len(gates) == 7
    assert coordinate(1, 1) in gates
    assert coordinate(2, 2) in gates
    assert coordinate(2, 1) in gates
    assert coordinate(1, 2) in gates
    assert coordinate(2, 1, 1) in gates
    assert coordinate(3, 1) in gates
    assert coordinate(3, 2) in gates
    assert layout.get_node(coordinate(1, 1)) == 6
    assert layout.get_node(coordinate(2, 1)) == 7
    assert layout.get_node(coordinate(1, 2)) == 8
    assert layout.get_node(coordinate(2, 2)) == 9
    assert layout.get_node(coordinate(2, 1, 1)) == 10
    assert layout.get_tile(6) == coordinate(1, 1)
    assert layout.get_tile(7) == coordinate(2, 1)
    assert layout.get_tile(8) == coordinate(1, 2)
    assert layout.get_tile(9) == coordinate(2, 2)
    assert layout.get_tile(10) == coordinate(2, 1, 1)
    assert layout.make_signal(6) == a1
    assert layout.make_signal(7) == b1
    assert layout.make_signal(8) == b2
    assert layout.make_signal(9) == a2
    assert layout.make_signal(10) == c

    # wires
    wires = layout.wires()
    assert len(wires) == 9
    assert coordinate(1, 0) in wires
    assert coordinate(0, 1) in wires
    assert coordinate(2, 0) in wires
    assert coordinate(0, 2) in wires
    assert coordinate(2, 1) in wires
    assert coordinate(1, 2) in wires
    assert coordinate(2, 1, 1) in wires
    assert coordinate(3, 1) in wires
    assert coordinate(3, 2) in wires

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


def test_hexagonal_gate_layout_gate_level_inheritance():
    layout = hexagonal_gate_layout(arrangement.EVEN_ROW, (3, 3, 1), "2DDWave", "Layout")

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
    assert coordinate(1, 0) in pis
    assert coordinate(0, 1) in pis
    assert coordinate(2, 0) in pis
    assert coordinate(0, 2) in pis
    assert layout.get_node(coordinate(1, 0)) == 2
    assert layout.get_node(coordinate(0, 1)) == 3
    assert layout.get_node(coordinate(2, 0)) == 4
    assert layout.get_node(coordinate(0, 2)) == 5
    assert layout.get_tile(2) == coordinate(1, 0)
    assert layout.get_tile(3) == coordinate(0, 1)
    assert layout.get_tile(4) == coordinate(2, 0)
    assert layout.get_tile(5) == coordinate(0, 2)
    assert layout.make_signal(2) == x1
    assert layout.make_signal(3) == x2
    assert layout.make_signal(4) == x3
    assert layout.make_signal(5) == x4

    # POs
    pos = layout.pos()
    assert len(pos) == 2
    assert coordinate(3, 1) in pos
    assert coordinate(3, 2) in pos
    assert layout.get_node(coordinate(3, 1)) == 11
    assert layout.get_node(coordinate(3, 2)) == 12
    assert layout.get_tile(11) == coordinate(3, 1)
    assert layout.get_tile(12) == coordinate(3, 2)
    assert layout.make_signal(11) == f1
    assert layout.make_signal(12) == f2

    # gates
    gates = layout.gates()
    assert len(gates) == 7
    assert coordinate(1, 1) in gates
    assert coordinate(2, 2) in gates
    assert coordinate(2, 1) in gates
    assert coordinate(1, 2) in gates
    assert coordinate(2, 1, 1) in gates
    assert coordinate(3, 1) in gates
    assert coordinate(3, 2) in gates
    assert layout.get_node(coordinate(1, 1)) == 6
    assert layout.get_node(coordinate(2, 1)) == 7
    assert layout.get_node(coordinate(1, 2)) == 8
    assert layout.get_node(coordinate(2, 2)) == 9
    assert layout.get_node(coordinate(2, 1, 1)) == 10
    assert layout.get_tile(6) == coordinate(1, 1)
    assert layout.get_tile(7) == coordinate(2, 1)
    assert layout.get_tile(8) == coordinate(1, 2)
    assert layout.get_tile(9) == coordinate(2, 2)
    assert layout.get_tile(10) == coordinate(2, 1, 1)
    assert layout.make_signal(6) == a1
    assert layout.make_signal(7) == b1
    assert layout.make_signal(8) == b2
    assert layout.make_signal(9) == a2
    assert layout.make_signal(10) == c

    # wires
    wires = layout.wires()
    assert len(wires) == 9
    assert coordinate(1, 0) in wires
    assert coordinate(0, 1) in wires
    assert coordinate(2, 0) in wires
    assert coordinate(0, 2) in wires
    assert coordinate(2, 1) in wires
    assert coordinate(1, 2) in wires
    assert coordinate(2, 1, 1) in wires
    assert coordinate(3, 1) in wires
    assert coordinate(3, 2) in wires

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
