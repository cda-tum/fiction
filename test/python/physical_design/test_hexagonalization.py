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

from mnt.pyfiction.layouts import arrangement, cartesian_gate_layout, layout_input_port
from mnt.pyfiction.physical_design import (
    hexagonalization,
    hexagonalization_stats,
    orthogonal,
)
from mnt.pyfiction.physical_design import (
    hexagonalization_io_pin_extension_mode as mode,
)
from mnt.pyfiction.verification import eq_type, equivalence_checking

if TYPE_CHECKING:
    from mnt.pyfiction.networks import technology_network


@pytest.mark.parametrize(
    ("inputs", "outputs"),
    [
        (mode.NONE, mode.NONE),
        (mode.EXTEND, mode.NONE),
        (mode.NONE, mode.EXTEND),
        (mode.EXTEND, mode.EXTEND),
        (mode.EXTEND_PLANAR, mode.NONE),
        (mode.NONE, mode.EXTEND_PLANAR),
        (mode.EXTEND_PLANAR, mode.EXTEND_PLANAR),
    ],
)
def test_hexagonalization_extensions_and_statistics(mux21: technology_network, inputs: mode, outputs: mode) -> None:
    """Retain logic, names, geometry, and statistics for each supported extension mode."""
    layout = orthogonal(mux21)
    stats = hexagonalization_stats()
    reports = []
    result = hexagonalization(
        layout,
        input_pin_extension=inputs,
        output_pin_extension=outputs,
        statistics=stats,
        on_progress=lambda *report: reports.append(report),
    )
    assert equivalence_checking(mux21, result) == eq_type.STRONG
    assert result.get_arrangement() == arrangement.EVEN_ROW
    assert result.get_layout_name() == layout.get_layout_name()
    assert [result.get_input_name(i) for i in range(result.num_pis())] == [
        layout.get_input_name(i) for i in range(layout.num_pis())
    ]
    assert [result.get_output_name(i) for i in range(result.num_pos())] == [
        layout.get_output_name(i) for i in range(layout.num_pos())
    ]
    assert (stats.x_size, stats.y_size) == (result.width(), result.height())
    assert (stats.num_gates, stats.num_wires, stats.num_crossings) == (
        result.num_gates(),
        result.num_wires(),
        result.num_crossings(),
    )
    assert stats.time_total.total_seconds() >= 0
    counts = [(done, total) for task, done, total in reports if task == "objects"]
    assert counts[0] == (0, layout.size())
    assert counts[-1] == (layout.size(), layout.size())
    assert counts == sorted(counts)
    if inputs != mode.NONE:
        assert all(result.get_tile(pi).y == 0 for pi in result.pis())
    if outputs != mode.NONE:
        assert all(result.get_tile(po).y == result.height() - 1 for po in result.pos())
    if inputs == outputs == mode.NONE:
        default = hexagonalization(layout, statistics=None, on_progress=None)
        assert equivalence_checking(mux21, default) == eq_type.STRONG


def test_hexagonalization_preserves_input_holes_and_declared_order() -> None:
    """A missing input stays disconnected while terminal order remains caller-defined."""
    layout = cartesian_gate_layout((4, 3, 2), "2DDWave")
    b = layout.create_pi("b", (0, 1))
    a = layout.create_pi("a", (1, 0))
    gate = layout.create_lt(a, b, (1, 1))
    layout.disconnect(layout_input_port(gate, 0))
    layout.create_po(gate, "f", (3, 1))
    layout.set_input_order([a, b])
    result = hexagonalization(layout)
    assert [result.get_input_name(i) for i in range(result.num_pis())] == ["a", "b"]
    assert result.get_output_name(0) == "f"
    assert result.num_gates() == 1
    placed = result.gates()[0]
    assert result.is_lt(placed)
    assert result.input_count(placed) == 2
    assert result.source(layout_input_port(placed, 0)) is None
    source = result.source(layout_input_port(placed, 1))
    assert source is not None
    assert result.get_name(source) == "b"


@pytest.mark.parametrize("outputs", [mode.EXTEND, mode.EXTEND_PLANAR])
def test_planar_input_extension_with_output_extension(outputs: mode) -> None:
    """Planar inputs add no crossings when the output is already on the final border."""
    layout = cartesian_gate_layout((5, 5, 2), "2DDWave")
    branches = [
        [(0, 2), (1, 2), (2, 2)],
        [(0, 0), (0, 1), (1, 1), (2, 1), (3, 1)],
    ]
    ends = []
    for index, branch in enumerate(branches):
        wire = layout.create_pi(f"x{index}", branch[0])
        for site in branch[1:]:
            wire = layout.create_buf(wire, site)
        ends.append(wire)
    wire = layout.create_and(*ends, (3, 2))
    for site in [(4, 2), (4, 3)]:
        wire = layout.create_buf(wire, site)
    layout.create_po(wire, "f", (4, 4))
    unextended = hexagonalization(layout)
    assert layout.num_crossings() == unextended.num_crossings() == 0
    assert unextended.get_tile(unextended.po_at(0)).y == unextended.height() - 1

    result = hexagonalization(layout, input_pin_extension=mode.EXTEND_PLANAR, output_pin_extension=outputs)
    assert equivalence_checking(layout, result) == eq_type.STRONG
    assert result.num_crossings() == 0
    assert all(result.get_tile(pi).y == 0 for pi in result.pis())
    assert result.get_tile(result.po_at(0)).y == result.height() - 1


@pytest.mark.parametrize("inputs", [mode.EXTEND, mode.EXTEND_PLANAR])
def test_planar_output_extension_with_input_extension(inputs: mode) -> None:
    """Planar outputs add no crossings when the input is already on the final border."""
    layout = cartesian_gate_layout((5, 5, 2), "2DDWave")
    source = layout.create_pi("x", (0, 0))
    branches = [
        [(0, 1), (1, 1), (1, 2), (1, 3), (2, 3), (3, 3), (4, 3), (4, 4)],
        [(1, 0), (2, 0), (2, 1), (2, 2), (3, 2), (4, 2)],
    ]
    for index, branch in enumerate(branches):
        wire = source
        for site in branch[:-1]:
            wire = layout.create_buf(wire, site)
        layout.create_po(wire, f"f{index}", branch[-1])
    unextended = hexagonalization(layout)
    assert layout.num_crossings() == unextended.num_crossings() == 0
    assert unextended.get_tile(unextended.pi_at(0)).y == 0

    result = hexagonalization(layout, input_pin_extension=inputs, output_pin_extension=mode.EXTEND_PLANAR)
    assert equivalence_checking(layout, result) == eq_type.STRONG
    assert result.num_crossings() == 0
    assert result.get_tile(result.pi_at(0)).y == 0
    assert all(result.get_tile(po).y == result.height() - 1 for po in result.pos())


def test_hexagonalization_preserves_crossing_layer() -> None:
    """A primary input on layer one retains its elevation."""
    layout = cartesian_gate_layout((1, 1, 2), "2DDWave")
    layout.create_pi("a", (0, 0, 1))
    result = hexagonalization(layout)
    assert result.layers() == 2
    assert result.num_pis() == 1
    assert result.get_tile(result.pi_at(0)).z == 1


@pytest.mark.parametrize("case", ["clocking", "outside", "overflow"])
def test_hexagonalization_rejects_invalid_geometry(case: str) -> None:
    """Reject unsupported clocking and geometry before transforming coordinates."""
    layout = cartesian_gate_layout(
        (2147483648, 2) if case == "overflow" else (3, 3), "USE" if case == "clocking" else "2DDWave"
    )
    source = layout.create_pi("a", (-1, 0) if case == "outside" else (0, 0))
    with pytest.raises(OverflowError if case == "overflow" else ValueError):
        hexagonalization(layout)
    assert layout.contains(source)
