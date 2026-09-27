# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction.layouts import CartesianGateLayout, HexagonalGateLayout, ShiftedCartesianGateLayout
from mnt.pyfiction.layouts.coords import OffsetCoordinate
from mnt.pyfiction.verification import DesignRuleParams, critical_path_length_and_throughput, gate_level_drvs

OBSTRUCTION_LAYOUTS = [
    pytest.param(
        lambda: CartesianGateLayout((3, 3, 1), "2DDWave", "Layout"),
        id="CartesianGateLayout",
    ),
    pytest.param(
        lambda: ShiftedCartesianGateLayout((3, 3, 1), "2DDWave", "Layout"),
        id="ShiftedCartesianGateLayout",
    ),
    pytest.param(
        lambda: HexagonalGateLayout((3, 3, 1), "2DDWave", "Layout"),
        id="HexagonalGateLayout",
    ),
]


@pytest.mark.parametrize(
    "make_layout",
    [
        pytest.param(
            lambda: CartesianGateLayout((2, 2, 0), "2DDWave", "Layout"),
            id="CartesianGateLayout",
        ),
        pytest.param(
            lambda: ShiftedCartesianGateLayout((2, 2, 0), "2DDWave", "Layout"),
            id="ShiftedCartesianGateLayout",
        ),
        pytest.param(
            lambda: HexagonalGateLayout((2, 2, 0), "2DDWave", "Layout"),
            id="HexagonalGateLayout",
        ),
    ],
)
def test_gate_layout_clocking_inheritance(make_layout):
    layout = make_layout()
    assert layout.incoming_clocked_zones((0, 0)) == []
    assert layout.outgoing_clocked_zones((2, 2)) == []

    for icz in layout.incoming_clocked_zones((1, 1)):
        assert icz in [OffsetCoordinate(1, 0), OffsetCoordinate(0, 1)]

    for icz in layout.outgoing_clocked_zones((1, 1)):
        assert icz in [OffsetCoordinate(1, 2), OffsetCoordinate(2, 1)]


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
    layout = CartesianGateLayout((3, 3, 1), "2DDWave", "Layout")

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
    assert OffsetCoordinate(x1) in pis
    assert OffsetCoordinate(x2) in pis
    assert OffsetCoordinate(x3) in pis
    assert OffsetCoordinate(x4) in pis
    assert layout.get_node(OffsetCoordinate(x1)) == 2
    assert layout.get_node(OffsetCoordinate(x2)) == 3
    assert layout.get_node(OffsetCoordinate(x3)) == 4
    assert layout.get_node(OffsetCoordinate(x4)) == 5
    assert layout.get_tile(2) == OffsetCoordinate(x1)
    assert layout.get_tile(3) == OffsetCoordinate(x2)
    assert layout.get_tile(4) == OffsetCoordinate(x3)
    assert layout.get_tile(5) == OffsetCoordinate(x4)
    assert layout.get_tile(2) == x1
    assert layout.get_tile(3) == x2
    assert layout.get_tile(4) == x3
    assert layout.get_tile(5) == x4

    # POs
    pos = layout.pos()
    assert len(pos) == 2
    assert OffsetCoordinate(f1) in pos
    assert OffsetCoordinate(f2) in pos
    assert layout.get_node(OffsetCoordinate(f1)) == 11
    assert layout.get_node(OffsetCoordinate(f2)) == 12
    assert layout.get_tile(11) == OffsetCoordinate(f1)
    assert layout.get_tile(12) == OffsetCoordinate(f2)
    assert layout.get_tile(11) == f1
    assert layout.get_tile(12) == f2

    # gates
    gates = layout.gates()
    assert len(gates) == 7
    assert OffsetCoordinate(a1) in gates
    assert OffsetCoordinate(a2) in gates
    assert OffsetCoordinate(b1) in gates
    assert OffsetCoordinate(b2) in gates
    assert OffsetCoordinate(c) in gates
    assert OffsetCoordinate(f1) in gates
    assert OffsetCoordinate(f2) in gates
    assert layout.get_node(OffsetCoordinate(a1)) == 6
    assert layout.get_node(OffsetCoordinate(b1)) == 7
    assert layout.get_node(OffsetCoordinate(b2)) == 8
    assert layout.get_node(OffsetCoordinate(a2)) == 9
    assert layout.get_node(OffsetCoordinate(c)) == 10
    assert layout.get_tile(6) == OffsetCoordinate(a1)
    assert layout.get_tile(7) == OffsetCoordinate(b1)
    assert layout.get_tile(8) == OffsetCoordinate(b2)
    assert layout.get_tile(9) == OffsetCoordinate(a2)
    assert layout.get_tile(10) == OffsetCoordinate(c)
    assert layout.get_tile(6) == a1
    assert layout.get_tile(7) == b1
    assert layout.get_tile(8) == b2
    assert layout.get_tile(9) == a2
    assert layout.get_tile(10) == c

    # wires
    wires = layout.wires()
    assert len(wires) == 9
    assert OffsetCoordinate(x1) in wires
    assert OffsetCoordinate(x2) in wires
    assert OffsetCoordinate(x3) in wires
    assert OffsetCoordinate(x4) in wires
    assert OffsetCoordinate(b1) in wires
    assert OffsetCoordinate(b2) in wires
    assert OffsetCoordinate(c) in wires
    assert OffsetCoordinate(f1) in wires
    assert OffsetCoordinate(f2) in wires

    # incoming data flow
    inx1 = layout.fanins(OffsetCoordinate(x1))
    assert len(inx1) == 0

    inf1 = layout.fanins(OffsetCoordinate(f1))
    assert len(inf1) == 1
    assert OffsetCoordinate(c) in inf1

    ina2 = layout.fanins(OffsetCoordinate(a2))
    assert len(ina2) == 2
    assert OffsetCoordinate(b1) in ina2
    assert OffsetCoordinate(b2) in ina2

    # outgoing data flow
    outx1 = layout.fanouts(OffsetCoordinate(x1))
    assert len(outx1) == 1
    assert OffsetCoordinate(a1) in outx1

    outf1 = layout.fanouts(OffsetCoordinate(f1))
    assert len(outf1) == 0

    outa2 = layout.fanouts(OffsetCoordinate(a2))
    assert len(outa2) == 1
    assert OffsetCoordinate(f2) in outa2

    cp, tp = critical_path_length_and_throughput(layout)
    assert cp == 4
    assert tp == 1

    drv_params = DesignRuleParams()
    result = gate_level_drvs(layout, params=drv_params)
    assert (result.warnings, result.drvs) == (0, 0)


def test_hexagonal_gate_layout_gate_level_inheritance():
    layout = HexagonalGateLayout((3, 3, 1), "2DDWave", "Layout")

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
    assert OffsetCoordinate(x1) in pis
    assert OffsetCoordinate(x2) in pis
    assert OffsetCoordinate(x3) in pis
    assert OffsetCoordinate(x4) in pis
    assert layout.get_node(OffsetCoordinate(x1)) == 2
    assert layout.get_node(OffsetCoordinate(x2)) == 3
    assert layout.get_node(OffsetCoordinate(x3)) == 4
    assert layout.get_node(OffsetCoordinate(x4)) == 5
    assert layout.get_tile(2) == OffsetCoordinate(x1)
    assert layout.get_tile(3) == OffsetCoordinate(x2)
    assert layout.get_tile(4) == OffsetCoordinate(x3)
    assert layout.get_tile(5) == OffsetCoordinate(x4)
    assert layout.get_tile(2) == x1
    assert layout.get_tile(3) == x2
    assert layout.get_tile(4) == x3
    assert layout.get_tile(5) == x4

    # POs
    pos = layout.pos()
    assert len(pos) == 2
    assert OffsetCoordinate(f1) in pos
    assert OffsetCoordinate(f2) in pos
    assert layout.get_node(OffsetCoordinate(f1)) == 11
    assert layout.get_node(OffsetCoordinate(f2)) == 12
    assert layout.get_tile(11) == OffsetCoordinate(f1)
    assert layout.get_tile(12) == OffsetCoordinate(f2)
    assert layout.get_tile(11) == f1
    assert layout.get_tile(12) == f2

    # gates
    gates = layout.gates()
    assert len(gates) == 7
    assert OffsetCoordinate(a1) in gates
    assert OffsetCoordinate(a2) in gates
    assert OffsetCoordinate(b1) in gates
    assert OffsetCoordinate(b2) in gates
    assert OffsetCoordinate(c) in gates
    assert OffsetCoordinate(f1) in gates
    assert OffsetCoordinate(f2) in gates
    assert layout.get_node(OffsetCoordinate(a1)) == 6
    assert layout.get_node(OffsetCoordinate(b1)) == 7
    assert layout.get_node(OffsetCoordinate(b2)) == 8
    assert layout.get_node(OffsetCoordinate(a2)) == 9
    assert layout.get_node(OffsetCoordinate(c)) == 10
    assert layout.get_tile(6) == OffsetCoordinate(a1)
    assert layout.get_tile(7) == OffsetCoordinate(b1)
    assert layout.get_tile(8) == OffsetCoordinate(b2)
    assert layout.get_tile(9) == OffsetCoordinate(a2)
    assert layout.get_tile(10) == OffsetCoordinate(c)
    assert layout.get_tile(6) == a1
    assert layout.get_tile(7) == b1
    assert layout.get_tile(8) == b2
    assert layout.get_tile(9) == a2
    assert layout.get_tile(10) == c

    # wires
    wires = layout.wires()
    assert len(wires) == 9
    assert OffsetCoordinate(x1) in wires
    assert OffsetCoordinate(x2) in wires
    assert OffsetCoordinate(x3) in wires
    assert OffsetCoordinate(x4) in wires
    assert OffsetCoordinate(b1) in wires
    assert OffsetCoordinate(b2) in wires
    assert OffsetCoordinate(c) in wires
    assert OffsetCoordinate(f1) in wires
    assert OffsetCoordinate(f2) in wires

    # incoming data flow
    inx1 = layout.fanins(OffsetCoordinate(x1))
    assert len(inx1) == 0

    inf1 = layout.fanins(OffsetCoordinate(f1))
    assert len(inf1) == 1
    assert OffsetCoordinate(c) in inf1

    ina2 = layout.fanins(OffsetCoordinate(a2))
    assert len(ina2) == 2
    assert OffsetCoordinate(b1) in ina2
    assert OffsetCoordinate(b2) in ina2

    # outgoing data flow
    outx1 = layout.fanouts(OffsetCoordinate(x1))
    assert len(outx1) == 1
    assert OffsetCoordinate(a1) in outx1

    outf1 = layout.fanouts(OffsetCoordinate(f1))
    assert len(outf1) == 0

    outa2 = layout.fanouts(OffsetCoordinate(a2))
    assert len(outa2) == 1
    assert OffsetCoordinate(f2) in outa2

    cp, tp = critical_path_length_and_throughput(layout)
    assert cp == 4
    assert tp == 1

    drv_params = DesignRuleParams()
    result = gate_level_drvs(layout, params=drv_params)
    assert (result.warnings, result.drvs) == (0, 0)
