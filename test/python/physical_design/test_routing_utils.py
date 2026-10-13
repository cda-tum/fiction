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
    layout_input_port,
    arrangement,
    cartesian_gate_layout,
    hexagonal_gate_layout,
    shifted_cartesian_gate_layout,
)
from mnt.pyfiction.physical_design import clear_routing, route_path
from mnt.pyfiction.physical_design.path_finding import a_star


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
    "layout",
    [
        pytest.param(cartesian_gate_layout((3, 1)), id="cartesian"),
        pytest.param(shifted_cartesian_gate_layout(arrangement.EVEN_ROW, (3, 1)), id="shifted"),
        pytest.param(hexagonal_gate_layout(arrangement.EVEN_ROW, (3, 1)), id="hexagonal"),
    ],
)
def test_clear_routing(layout: cartesian_gate_layout | shifted_cartesian_gate_layout | hexagonal_gate_layout) -> None:
    """Remove routing wires and preserve disconnected terminal identities."""
    source = layout.create_pi("source", (0, 0))
    wire = layout.create_buf(source, (1, 0))
    output = layout.create_po(wire, "output", (2, 0))

    clear_routing(layout)

    assert layout.find_object((0, 0)) == source
    assert layout.find_object((1, 0)) is None
    assert layout.find_object((2, 0)) == output
    assert layout.input_count(output) == 1
    assert layout.source(layout_input_port(output, 0)) is None
