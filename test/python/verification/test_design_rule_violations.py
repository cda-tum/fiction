# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import json
from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction.layouts import cartesian_gate_layout, layout_input_port
from mnt.pyfiction.physical_design import color_routing
from mnt.pyfiction.synthesis import dynamic_truth_table
from mnt.pyfiction.verification import gate_level_drv_stats, gate_level_drvs

if TYPE_CHECKING:
    from collections.abc import Callable

    from .conftest import GateLayout


def test_drvs() -> None:
    """A routed AND/OR network satisfies the physical design rules."""
    layout = cartesian_gate_layout((3, 6, 1), "2DDWave")
    x1 = layout.create_pi("x1", (0, 3))
    x2 = layout.create_pi("x2", (0, 0))
    x3 = layout.create_pi("x3", (2, 0))
    first_wire = layout.create_buf(x2, (0, 1))
    second_wire = layout.create_buf(first_wire, (1, 1))
    inv = layout.create_not(first_wire, (0, 2))
    third_wire = layout.create_buf(inv, (1, 2))
    first_and = layout.create_and(x1, third_wire, (1, 3))
    second_and = layout.create_and(second_wire, x3, (2, 1))
    fourth_wire = layout.create_buf(second_and, (2, 2))
    result = layout.create_or(first_and, fourth_wire, (2, 3))
    output = layout.create_po(result, "f1", (2, 4))
    layout.move_object(output, (2, 5))
    color_routing(layout, [((2, 3), (2, 5), 0)])

    warnings, drvs = gate_level_drvs(layout)

    assert warnings == 0
    assert drvs == 0


def test_drvs_check_placements_outside_extent() -> None:
    """The extent check visits every live object and can be disabled independently."""
    layout = cartesian_gate_layout((1, 1, 1), "2DDWave")
    source = layout.create_pi("a", (-2, 0))
    layout.create_po(source, "result", (-1, 0))
    options: dict[str, Any] = {
        "non_adjacent_connections": False,
        "missing_connections": False,
        "crossing_gates": False,
        "clocked_data_flow": False,
        "has_io": False,
        "border_io": False,
    }
    assert gate_level_drvs(layout, **options) == (0, 2)
    options["outside_extent"] = False
    assert gate_level_drvs(layout, **options) == (0, 0)


def test_empty_layout_has_no_issues(make_layout: Callable[[tuple[int, int, int], str], GateLayout]) -> None:
    """An empty layout is a valid design in every exposed geometry."""
    layout = make_layout((0, 0, 1), "OPEN")
    stats = gate_level_drv_stats()
    assert gate_level_drvs(layout, statistics=stats) == (0, 0)
    assert stats.warnings == stats.drvs == 0
    assert all(not entries for entries in json.loads(stats.report).values())


@pytest.mark.parametrize("print_report", [False, True])
def test_border_warning_and_printed_report(capsys: pytest.CaptureFixture[str], *, print_report: bool) -> None:
    """An interior primary input produces a warning rather than a violation."""
    layout = cartesian_gate_layout((3, 3, 1), "2DDWave")
    source = layout.create_pi("a", (1, 1))
    layout.create_po(source, "f", (1, 2))
    stats = gate_level_drv_stats()
    assert gate_level_drvs(layout, statistics=stats, print_report=print_report) == (1, 0)
    assert (stats.warnings, stats.drvs) == (1, 0)
    assert bool(capsys.readouterr().out) is print_report
    assert gate_level_drvs(layout, border_io=False) == (0, 0)


@pytest.mark.parametrize("terminal", ["input", "output"])
def test_crossings_require_wires_on_both_layers(terminal: str) -> None:
    """A terminal cannot serve as either physical wire of a crossing."""
    layout = cartesian_gate_layout((1, 1, 2), "OPEN")
    if terminal == "input":
        layout.create_pi("a", (0, 0, 0))
        layout.create_buf((0, 0, 1))
    else:
        wire = layout.create_buf((0, 0, 0))
        layout.create_po(wire, "f", (0, 0, 1))
    stats = gate_level_drv_stats()
    gate_level_drvs(layout, statistics=stats)
    assert len(json.loads(stats.report)["Wires crossing gates"]) == 1
    gate_level_drvs(layout, statistics=stats, crossing_gates=False)
    assert "Wires crossing gates" not in json.loads(stats.report)


def test_declared_input_holes_are_reported() -> None:
    """Filling a required input hole clears the missing-connection diagnostic."""
    layout = cartesian_gate_layout((1, 3, 1), "2DDWave")
    source = layout.create_pi("a", (0, 0))
    function = dynamic_truth_table(2)
    function.create_from_hex_string("8")
    gate = layout.create_gate([source], function, (0, 1))
    layout.disconnect(layout_input_port(gate, 0))
    layout.connect(source, layout_input_port(gate, 1))
    layout.create_po(gate, "result", (0, 2))
    stats = gate_level_drv_stats()
    assert gate_level_drvs(layout, statistics=stats) == (0, 1)
    assert len(json.loads(stats.report)["Missing connections"]) == 1
    layout.connect(source, layout_input_port(gate, 0))
    assert gate_level_drvs(layout, statistics=stats) == (0, 0)
    assert not json.loads(stats.report)["Missing connections"]


def test_reference_layout_design_rules(reference_layout: tuple[cartesian_gate_layout, int, int, int]) -> None:
    """The Catch2 physical blueprints retain their expected warning and violation counts."""
    layout, _length, _throughput, warnings = reference_layout
    assert gate_level_drvs(layout) == (warnings, 0)


def test_all_function_blueprint_violations() -> None:
    """The nonstructural Catch2 blueprint reports every violation and its interior-output warning."""
    layout = cartesian_gate_layout((4, 6, 1), "OPEN")
    for y in range(4):
        for x in range(3):
            layout.assign_clock_number((x, y), y)
    layout.assign_clock_number((2, 2), 3)
    a, b, c = [layout.create_pi(name, (index, 0)) for index, name in enumerate(["x1", "x2", "x3"])]
    for x, name in enumerate(["and", "or", "xor"]):
        getattr(layout, f"create_{name}")(a, b, (x, 1))
    majority = layout.create_maj(a, b, c, (0, 2))
    fanout = layout.create_buf(majority, (1, 2))
    wire = layout.create_buf(fanout, (2, 2))
    layout.create_not(wire, (2, 3))
    layout.create_po(fanout, "po", (1, 3))
    for name, point in [
        ("nand", (0, 4)),
        ("nor", (1, 4)),
        ("lt", (0, 5)),
        ("le", (1, 5)),
        ("gt", (2, 5)),
        ("ge", (3, 5)),
    ]:
        getattr(layout, f"create_{name}")(a, b, point)
    assert gate_level_drvs(layout) == (1, 46)


def test_outside_extent_objects_retain_connection_diagnostics() -> None:
    """Out-of-frame objects participate in adjacency checks as well as extent checks."""
    layout = cartesian_gate_layout((1, 1, 1), "OPEN")
    signal = layout.create_pi("a", (-10, -2))
    layout.create_po(signal, "result", (10, -2))
    stats = gate_level_drv_stats()
    gate_level_drvs(layout, statistics=stats)
    report = json.loads(stats.report)
    assert len(report["Non adjacent connections"]) == 2
    assert len(report["Objects outside extent"]) == 2
    assert stats.drvs > 0
