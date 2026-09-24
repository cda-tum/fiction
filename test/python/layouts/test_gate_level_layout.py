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
    critical_path_length_and_throughput,
    gate_level_drv_params,
    gate_level_drvs,
    hexagonal_gate_layout,
    offset_coordinate,
    shifted_cartesian_gate_layout,
)


@pytest.mark.parametrize(
    "make_layout",
    [
        pytest.param(lambda: cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout"), id="cartesian_gate_layout"),
        pytest.param(
            lambda: shifted_cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout"), id="shifted_cartesian_gate_layout"
        ),
        pytest.param(lambda: hexagonal_gate_layout((2, 2, 0), "2DDWave", "Layout"), id="hexagonal_gate_layout"),
    ],
)
def test_gate_level_layout_inheritance(make_layout):
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
        pytest.param(lambda: cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout"), id="cartesian_gate_layout"),
        pytest.param(
            lambda: shifted_cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout"), id="shifted_cartesian_gate_layout"
        ),
        pytest.param(lambda: hexagonal_gate_layout((3, 3, 1), "2DDWave", "Layout"), id="hexagonal_gate_layout"),
    ],
)
def test_gate_level_layout_iteration(make_layout):
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
    assert layout.get_name(x1) == "x1"
    assert layout.get_name(x2) == "x2"
    assert layout.get_name(x3) == "x3"
    assert layout.get_name(x4) == "x4"

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
    assert layout.get_name(f1) == "f1"
    assert layout.get_name(f2) == "f2"

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


def test_gate_level_layout_gate_types():
    for layout in [
        cartesian_gate_layout((2, 8, 0), "2DDWave", "Layout"),
        shifted_cartesian_gate_layout((2, 8, 0), "2DDWave", "Layout"),
        hexagonal_gate_layout((2, 8, 0), "2DDWave", "Layout"),
    ]:
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
        layout.create_po(fanout, "f1", (1, 8))
        layout.create_po(buf, "f2", (2, 8))

        # check gate type
        # pis
        assert layout.is_pi(layout.get_node((0, 0)))
        assert layout.is_pi(layout.get_node((0, 1)))
        assert layout.is_pi(layout.get_node((0, 2)))
        assert layout.is_pi(layout.get_node((0, 3)))
        assert layout.is_pi(layout.get_node((0, 4)))
        assert layout.is_pi(layout.get_node((0, 5)))
        assert layout.is_pi(layout.get_node((0, 6)))

        # gates
        assert layout.is_inv(layout.get_node((1, 0)))
        assert layout.is_and(layout.get_node((1, 1)))
        assert layout.is_nand(layout.get_node((1, 2)))
        assert layout.is_or(layout.get_node((1, 3)))
        assert layout.is_nor(layout.get_node((1, 4)))
        assert layout.is_xor(layout.get_node((1, 5)))
        assert layout.is_xnor(layout.get_node((1, 6)))
        assert layout.is_fanout(layout.get_node((1, 7)))
        assert layout.is_wire(layout.get_node((2, 7)))

        # pos
        assert layout.is_po(layout.get_node((1, 8)))
        assert layout.is_po(layout.get_node((2, 8)))

    for layout in [
        cartesian_gate_layout((2, 2, 0), "RES", "Layout"),
        shifted_cartesian_gate_layout((2, 2, 0), "RES", "Layout"),
        hexagonal_gate_layout((2, 2, 0), "RES", "Layout"),
    ]:
        assert layout.is_empty()

        # pis
        x1 = layout.create_pi("x1", (0, 1))
        x2 = layout.create_pi("x2", (1, 0))
        x3 = layout.create_pi("x3", (2, 1))

        # maj
        maj = layout.create_maj(x1, x2, x3, (1, 1))

        # po
        layout.create_po(maj, "f1", (1, 2))

        # check gate type
        # pis
        assert layout.is_pi(layout.get_node((0, 1)))
        assert layout.is_pi(layout.get_node((1, 0)))
        assert layout.is_pi(layout.get_node((2, 1)))

        # maj
        assert layout.is_maj(layout.get_node((1, 1)))

        # po
        assert layout.is_po(layout.get_node((1, 2)))
