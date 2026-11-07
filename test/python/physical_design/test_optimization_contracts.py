# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction.layouts import cartesian_gate_layout, layout_input_port
from mnt.pyfiction.networks import simulate_outputs
from mnt.pyfiction.physical_design import (
    extract_routing_objectives,
    post_layout_optimization,
    post_layout_optimization_stats,
    wiring_reduction,
    wiring_reduction_stats,
)

if TYPE_CHECKING:
    from collections.abc import Callable


@pytest.fixture(
    params=[(wiring_reduction, wiring_reduction_stats), (post_layout_optimization, post_layout_optimization_stats)],
    ids=["wiring", "post-layout"],
)
def optimizer(request: pytest.FixtureRequest) -> tuple[Callable[..., None], Callable[[], Any]]:
    """Provide an optimizer and its statistics constructor.

    Returns:
        The selected optimizer and its statistics constructor.
    """
    return request.param


def test_optimization_preserves_ordered_ports_and_caller_identity(
    optimizer: tuple[Callable[..., None], Callable[[], Any]],
) -> None:
    """Reduce an asymmetric circuit while retaining gate and terminal handles."""
    optimize, make_stats = optimizer
    layout = cartesian_gate_layout((3, 6, 2), "2DDWave")
    a = layout.create_pi("a", (0, 0))
    b = layout.create_pi("b", (2, 0))
    left, right = a, b
    for y in range(1, 4):
        left = layout.create_buf(left, (0, y))
        right = layout.create_buf(right, (2, y))
    gate = layout.create_lt(left, right, (1, 4))
    output = layout.create_po(gate, "f", (1, 5))
    independent = layout.clone()
    expected = [("f", [False, False, True, False])]
    assert simulate_outputs(layout) == expected
    stats = make_stats()
    reports = []
    result = optimize(layout, statistics=stats, on_progress=lambda *report: reports.append(report))
    assert result is None
    assert all(layout.contains(node) for node in (a, b, gate, output))
    assert layout.is_lt(gate)
    assert simulate_outputs(layout) == expected
    assert simulate_outputs(independent) == expected
    assert independent.height() == 6
    assert independent.num_wires() == 6
    assert layout.num_wires() < independent.num_wires()
    assert sorted(extract_routing_objectives(layout)) == sorted([
        (layout.get_tile(a), layout.get_tile(gate), 0),
        (layout.get_tile(b), layout.get_tile(gate), 1),
        (layout.get_tile(gate), layout.get_tile(output), 0),
    ])
    assert [layout.get_input_name(i) for i in range(2)] == ["a", "b"]
    assert layout.get_output_name(0) == "f"
    assert (stats.x_size_before, stats.y_size_before) == (3, 6)
    assert (stats.x_size_after, stats.y_size_after) == (layout.width(), layout.height())
    assert (stats.num_wires_before, stats.num_wires_after) == (6, layout.num_wires())
    paths = [(done, total) for task, done, total in reports if task == "wire paths"]
    assert paths
    assert paths[-1][1] == 0
    assert all(done >= 0 for done, _ in paths)


def test_optimization_counts_no_terminal_as_a_wire(optimizer: tuple[Callable[..., None], Callable[[], Any]]) -> None:
    """A direct PI-to-PO connection has no physical routing wires."""
    optimize, make_stats = optimizer
    layout = cartesian_gate_layout((2, 1), "2DDWave")
    source = layout.create_pi("a", (0, 0))
    output = layout.create_po(source, "f", (1, 0))
    stats = make_stats()
    optimize(layout, statistics=stats, on_progress=None)
    assert stats.num_wires_before == stats.num_wires_after == 0
    assert layout.contains(source)
    assert layout.contains(output)
    assert simulate_outputs(layout) == [("f", [True, False])]


def test_empty_optimization(optimizer: tuple[Callable[..., None], Callable[[], Any]]) -> None:
    """Empty geometry has zero area and zero improvement."""
    optimize, make_stats = optimizer
    layout = cartesian_gate_layout((0, 0), "2DDWave")
    stats = make_stats()
    optimize(layout, statistics=stats)
    assert layout.is_empty()
    assert layout.area() == 0
    assert stats.area_improvement == 0


@pytest.mark.parametrize("case", ["clocking", "outside", "overflow"])
def test_optimizer_validation_preserves_layout(
    optimizer: tuple[Callable[..., None], Callable[[], Any]], case: str
) -> None:
    """Invalid geometry and clocking reject before mutating the caller."""
    optimize, _ = optimizer
    extent = (2147483647, 1) if case == "overflow" else (4, 4)
    layout = cartesian_gate_layout(extent, "USE" if case == "clocking" else "2DDWave")
    site = (-1, 1) if case == "outside" else (0, 0)
    source = layout.create_pi("a", site)
    output = layout.create_po(source, "f", (1, 0))
    error = OverflowError if case == "overflow" else ValueError
    with pytest.raises(error):
        optimize(layout)
    assert layout.get_tile(source) == site
    assert layout.source(layout_input_port(output, 0)) == source
    assert layout.size() == 2
    assert (layout.width(), layout.height()) == extent


def test_timeout_keeps_logic_and_trims_only_empty_frame(
    optimizer: tuple[Callable[..., None], Callable[[], Any]],
) -> None:
    """A zero timeout preserves connections and still reports actual result bounds."""
    optimize, make_stats = optimizer
    layout = cartesian_gate_layout((6, 3), "2DDWave")
    source = layout.create_pi("a", (0, 0))
    wire = layout.create_buf(source, (1, 0))
    output = layout.create_po(wire, "f", (5, 0))
    stats = make_stats()
    optimize(layout, timeout=0, statistics=stats)
    assert layout.source(layout_input_port(output, 0)) == wire
    assert layout.contains(source)
    assert layout.height() == 1
    assert stats.y_size_before == 3
    assert stats.y_size_after == 1
    assert simulate_outputs(layout) == [("f", [True, False])]


@pytest.mark.parametrize(("width", "height"), [(32, 32), (1024, 4), (4, 1024)])
def test_post_layout_optimization_preserves_rectangular_geometry(width: int, height: int) -> None:
    """PO-only optimization handles square, wide, and tall sparse frames."""

    def tile(x: int, y: int) -> tuple[int, int]:
        """Rotate the circuit into the frame's long direction.

        Returns:
            The position along the longer layout axis.
        """
        return (x, y) if width >= height else (y, x)

    edge = max(width, height)
    layout = cartesian_gate_layout((width, height, 2), "2DDWave")
    a = layout.create_pi("a", tile(edge - 3, 0))
    b = layout.create_pi("b", tile(edge - 4, 1))
    left = layout.create_buf(a, tile(edge - 2, 0))
    right = layout.create_buf(b, tile(edge - 3, 1))
    gate = layout.create_lt(left, right, tile(edge - 2, 1))
    wire = layout.create_buf(gate, tile(edge - 1, 1))
    output = layout.create_po(wire, "f", tile(edge - 1, 2))
    stats = post_layout_optimization_stats()
    post_layout_optimization(layout, optimize_pos_only=True, max_gate_relocations=1, statistics=stats)
    assert all(layout.contains(node) for node in (a, b, gate, output))
    assert simulate_outputs(layout) == [("f", [False, False, True, False])]
    assert sorted(extract_routing_objectives(layout)) == sorted([
        (layout.get_tile(a), layout.get_tile(gate), 0),
        (layout.get_tile(b), layout.get_tile(gate), 1),
        (layout.get_tile(gate), layout.get_tile(output), 0),
    ])
    assert (stats.x_size_before, stats.y_size_before) == (width, height)
    assert (stats.x_size_after, stats.y_size_after) == (layout.width(), layout.height())
    assert all(
        layout.is_within_bounds(layout.get_tile(node))
        for node in [*layout.pis(), *layout.pos(), *layout.gates(), *layout.wires()]
    )


@pytest.mark.parametrize(
    ("source_site", "output_site"), [((1, 1), (3, 1)), ((0, 1), (2, 1))], ids=["interior-input", "interior-output"]
)
def test_optimization_accepts_interior_terminals(
    optimizer: tuple[Callable[..., None], Callable[[], Any]], source_site: tuple[int, int], output_site: tuple[int, int]
) -> None:
    """Interior terminals remain connected through the optimizer's border handling."""
    optimize, _ = optimizer
    layout = cartesian_gate_layout((4, 4), "2DDWave")
    source = layout.create_pi("a", source_site)
    output = layout.create_po(source, "f", output_site)
    optimize(layout)
    assert layout.contains(source)
    assert layout.contains(output)
    assert simulate_outputs(layout) == [("f", [True, False])]
