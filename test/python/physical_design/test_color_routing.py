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
    layout_input_port,
)
from mnt.pyfiction.physical_design import color_routing, graph_coloring_engine


@pytest.mark.parametrize("conduct_partial_routing", [False, True])
def test_tabucol_rejection_preserves_layout(*, conduct_partial_routing: bool) -> None:
    """Reject an engine requiring an unavailable color bound before routing."""
    layout = cartesian_gate_layout((2, 2), "2DDWave")
    input_node = layout.create_pi("input", (0, 0))
    output = layout.create_buf(input_node, (1, 1))
    with pytest.raises(ValueError, match="TabuCol"):
        color_routing(
            layout,
            [((0, 0), (1, 1), 0)],
            engine=graph_coloring_engine.TABUCOL,
            conduct_partial_routing=conduct_partial_routing,
        )
    assert layout.source(layout_input_port(output, 0)) == input_node
    assert layout.num_wires() == 1
    assert layout.get_tile(output) == coordinate(1, 1)


def test_endpoint_validation_preserves_layout() -> None:
    """Reject invalid endpoints during search and preserve existing connections."""
    layout = cartesian_gate_layout((2, 2), "2DDWave")
    input_node = layout.create_pi("input", (0, 0))
    output = layout.create_buf(input_node, (1, 1))
    objectives = [((2, 0), (1, 1), 0)]
    with pytest.raises(ValueError, match="Routing endpoints"):
        color_routing(layout, objectives)
    with pytest.raises(ValueError, match="Routing endpoints"):
        color_routing(layout, objectives, path_limit=1)
    assert not color_routing(layout, objectives, path_limit=0)
    assert layout.source(layout_input_port(output, 0)) == input_node
    assert layout.num_wires() == 1
    assert layout.get_tile(output) == coordinate(1, 1)


@pytest.mark.parametrize(
    "make_lyt",
    [
        pytest.param(lambda: cartesian_gate_layout((5, 5), "2DDWave", "Layout"), id="cartesian_gate_layout"),
        pytest.param(
            lambda: hexagonal_gate_layout(arrangement.EVEN_ROW, (5, 5), "2DDWave", "Layout"), id="hexagonal_gate_layout"
        ),
    ],
)
def test_routing(make_lyt):
    lyt = make_lyt()
    x1 = lyt.create_pi("x1", coordinate(0, 0))
    x2 = lyt.create_pi("x2", coordinate(0, 1))

    a = lyt.create_and(x1, x2, coordinate(2, 2))

    lyt.create_po(a, "f1", coordinate(4, 4))

    success = color_routing(lyt, [((0, 0), (2, 2), 0), ((0, 1), (2, 2), 1), ((2, 2), (4, 4), 0)])

    assert success


def test_crossings():
    lyt = cartesian_gate_layout((5, 3, 2), "2DDWave", "Layout")

    x1 = lyt.create_pi("x1", (0, 1))
    x2 = lyt.create_pi("x2", (3, 2))
    x3 = lyt.create_pi("x3", (2, 0))

    buf1 = lyt.create_buf(x3, (2, 1))
    lyt.create_buf(buf1, (2, 2))

    gate = lyt.create_and(x1, x2, (4, 2))
    lyt.disconnect(layout_input_port(gate, 0))
    lyt.disconnect(layout_input_port(gate, 1))

    success = color_routing(lyt, [((0, 1), (4, 2), 0), ((3, 2), (4, 2), 1)], crossings=True, path_limit=1)

    assert success
