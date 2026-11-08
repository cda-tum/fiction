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
    coordinate,
    hexagonal_gate_layout,
    layout_input_port,
)
from mnt.pyfiction.networks import simulate_outputs
from mnt.pyfiction.physical_design import color_routing, graph_coloring_engine

if TYPE_CHECKING:
    from collections.abc import Callable


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
def test_routing(make_lyt: Callable[[], cartesian_gate_layout | hexagonal_gate_layout]) -> None:
    lyt = make_lyt()
    x1 = lyt.create_pi("x1", coordinate(0, 0))
    x2 = lyt.create_pi("x2", coordinate(0, 1))

    a = lyt.create_and(x1, x2, coordinate(2, 2))

    lyt.create_po(a, "f1", coordinate(4, 4))

    success = color_routing(lyt, [((0, 0), (2, 2), 0), ((0, 1), (2, 2), 1), ((2, 2), (4, 4), 0)])

    assert success
    assert simulate_outputs(lyt) == [("f1", [True, False, False, False])]


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


@pytest.mark.parametrize(
    "engine",
    [graph_coloring_engine.MCS, graph_coloring_engine.DSATUR, graph_coloring_engine.LMXRLF, graph_coloring_engine.SAT],
)
def test_color_routing_preserves_noncommutative_input_indices(engine: graph_coloring_engine) -> None:
    """Each public engine reconnects ports in the supplied logical order."""
    layout = cartesian_gate_layout((2, 2), "2DDWave")
    a = layout.create_pi("a", (0, 1))
    b = layout.create_pi("b", (1, 0))
    gate = layout.create_lt(a, b, (1, 1))
    layout.disconnect(layout_input_port(gate, 0))
    layout.disconnect(layout_input_port(gate, 1))
    success = color_routing(layout, [((1, 0), (1, 1), 1), ((0, 1), (1, 1), 0)], engine=engine)
    assert success
    assert layout.source(layout_input_port(gate, 0)) == a
    assert layout.source(layout_input_port(gate, 1)) == b
    assert layout.is_lt(gate)


@pytest.mark.parametrize("partial", [False, True])
def test_partial_routing_preserves_satisfiable_objectives(*, partial: bool) -> None:
    """Partial mode connects the feasible objective and tolerates reverse clock flow."""
    layout = cartesian_gate_layout((2, 2), "2DDWave")
    source = layout.create_pi("a", (0, 0))
    output = layout.create_po(source, "f", (1, 1))
    layout.disconnect(layout_input_port(output, 0))
    objectives = [((0, 0), (1, 1), 0), ((1, 1), (0, 0), 0)]
    success = color_routing(layout, objectives, conduct_partial_routing=partial)
    assert success is partial
    if partial:
        assert simulate_outputs(layout) == [("f", [True, False])]
    else:
        assert layout.source(layout_input_port(output, 0)) is None


@pytest.mark.parametrize("crossings", [False, True])
def test_color_routing_repeated_sources_keep_distinct_input_paths(*, crossings: bool) -> None:
    """The same source needs a separate physical path for each destination input."""
    layout = cartesian_gate_layout((2, 2, 2), "2DDWave")
    a = layout.create_pi("a", (0, 0))
    gate = layout.create_lt(a, a, (1, 1))
    layout.disconnect(layout_input_port(gate, 0))
    layout.disconnect(layout_input_port(gate, 1))
    success = color_routing(layout, [((0, 0), (1, 1), 0), ((0, 0), (1, 1), 1)], crossings=crossings)
    assert success
    wires = [layout.source(layout_input_port(gate, i)) for i in range(2)]
    assert wires[0] != wires[1]
    for wire in wires:
        assert wire is not None
        assert layout.source(layout_input_port(wire, 0)) == a
