# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""DOT layout contracts shared with the Catch2 drawer tests."""

from __future__ import annotations

import json
import re
from itertools import combinations
from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.layouts import arrangement, cartesian_gate_layout, hexagonal_gate_layout
from mnt.pyfiction.layouts.io import write_dot_layout
from mnt.pyfiction.synthesis import dynamic_truth_table

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path


def _nodes(text: str) -> list[tuple[str, str, str]]:
    """Read complete tile declarations without collapsing duplicate identifiers.

    Args:
        text: DOT document emitted by the writer.

    Returns:
        Tile identifiers, decoded labels, and fill colors.
    """
    return [
        (identifier, json.loads(label, strict=False), attributes.split(",")[0])
        for identifier, label, attributes in re.findall(
            r'^([A-Za-z_][A-Za-z_0-9]*) \[label=("(?:[^"\\]|\\.)*"), fillcolor=([^\]]+)\];',
            text,
            re.MULTILINE,
        )
    ]


def _edges(text: str) -> list[tuple[str, str]]:
    """Read logical edges independently of invisible topology constraints.

    Args:
        text: DOT document emitted by the writer.

    Returns:
        Every solid source/target pair, retaining multiplicity.
    """
    return re.findall(r"^([A-Za-z_][A-Za-z_0-9]*) -> ([A-Za-z_][A-Za-z_0-9]*) \[style=solid\];$", text, re.MULTILINE)


def test_empty_geometry(gate_factory: Callable, tmp_path: Path) -> None:
    """Empty layouts declare every ground tile with the topology's shape and rank direction."""
    layout = gate_factory((3, 3, 1))
    destination = tmp_path / "empty.dot"
    write_dot_layout(layout, str(destination))
    text = destination.read_text(encoding="utf-8")
    assert text.startswith("digraph layout {")
    assert text.rstrip().endswith("}")
    assert sorted(_nodes(text)) == sorted((f"x{x}y{y}", "", "white") for x in range(3) for y in range(3))
    assert _edges(text) == []
    cartesian = type(layout) is cartesian_gate_layout
    row = cartesian or layout.get_arrangement() in {arrangement.EVEN_ROW, arrangement.ODD_ROW}
    ranks = [
        " -> ".join(f"x{position if row else line}y{line if row else position}" for position in range(3))
        for line in range(3)
    ]
    if cartesian:
        assert "shape=square" in text
        assert "rankdir=TB" in text
        assert re.findall(r"^(x\d+y\d+ -> x\d+y\d+ -> x\d+y\d+);$", text, re.MULTILINE) == [
            " -> ".join(f"x{x}y{y}" for y in range(3)) for x in range(3)
        ]
    else:
        assert f"rankdir={'TB' if row else 'LR'}" in text
        assert ("shape=hexagon" in text) is (type(layout) is hexagonal_gate_layout)
        assert ("orientation=30" in text) is (type(layout) is hexagonal_gate_layout and row)
        odd = layout.get_arrangement() in {arrangement.ODD_ROW, arrangement.ODD_COLUMN}
        shifted = range(int(odd), 3, 2)
        ranks += [f"invis{line} -> x{0 if row else line}y{line if row else 0}" for line in shifted]
        assert sorted(re.findall(r"^(invis\d+) -> (x\d+y\d+);$", text, re.MULTILINE)) == sorted(
            (f"invis{line}", f"x{0 if row else neighbor}y{neighbor if row else 0}")
            for line in shifted
            for neighbor in (line - 1, line + 1)
            if 0 <= neighbor < 3
        )
        points = [(x, y) for y in range(3) for x in range(3)]
        expected_links = []
        for (ax, ay), (bx, by) in combinations(points, 2):
            aline, bline = (ay, by) if row else (ax, bx)
            along_a, along_b = (ax, bx) if row else (ay, by)
            # Neighboring shifted lines meet at centers half a tile apart.
            center_a = along_a + 0.5 * (aline % 2 == int(odd))
            center_b = along_b + 0.5 * (bline % 2 == int(odd))
            if abs(aline - bline) == 1 and abs(center_a - center_b) == 0.5:
                expected_links.append((f"x{ax}y{ay}", f"x{bx}y{by}"))
        assert sorted(re.findall(r"^(x\d+y\d+) -> (x\d+y\d+);$", text, re.MULTILINE)) == sorted(expected_links)
    assert sorted(re.findall(r"^rank = same \{ (.+) \};$", text, re.MULTILINE)) == sorted(ranks)


@pytest.mark.parametrize("indexes", [False, True])
def test_and_or_labels_colors_and_edges(gate_factory: Callable, tmp_path: Path, *, indexes: bool) -> None:
    """Gate blueprints retain complete interfaces, colors, identities, and logical edges."""
    layout = gate_factory((4, 2, 1), "OPEN")
    a = layout.create_pi("x1", (2, 0))
    b = layout.create_pi("x2", (1, 1))
    first = layout.create_and(a, b, (1, 0))
    second = layout.create_or(b, a, (2, 1))
    layout.create_po(first, "f1", (0, 0))
    layout.create_po(second, "f2", (3, 1))
    destination = tmp_path / "gates.dot"
    write_dot_layout(layout, str(destination), indexes=indexes)
    text = destination.read_text(encoding="utf-8")
    first_label = f"{first.index}:{first.generation}: AND" if indexes else "AND"
    second_label = f"{second.index}:{second.generation}: OR" if indexes else "OR"
    assert sorted(_nodes(text)) == sorted([
        ("x0y0", "f1", "snow2"),
        ("x1y0", first_label, "lightcoral"),
        ("x2y0", "x1", "snow2"),
        ("x3y0", "", "white"),
        ("x0y1", "", "white"),
        ("x1y1", "x2", "snow2"),
        ("x2y1", second_label, "palegreen2"),
        ("x3y1", "f2", "snow2"),
    ])
    assert sorted(_edges(text)) == sorted([
        ("x2y0", "x1y0"),
        ("x1y1", "x1y0"),
        ("x1y1", "x2y1"),
        ("x2y0", "x2y1"),
        ("x1y0", "x0y0"),
        ("x2y1", "x3y1"),
    ])


def test_crossing_projection_retains_both_paths(gate_factory: Callable, tmp_path: Path) -> None:
    """A two-layer wire crossing keeps both logical routes in its planar DOT projection."""
    layout = gate_factory((3, 3, 2))
    a = layout.create_pi("a", (0, 1))
    b = layout.create_pi("b", (1, 0))
    bottom = layout.create_buf(a, (1, 1))
    top = layout.create_buf(b, (1, 1, 1))
    layout.create_po(bottom, "f", (2, 1))
    layout.create_po(top, "g", (1, 2))
    destination = tmp_path / "crossing.dot"
    write_dot_layout(layout, str(destination))
    text = destination.read_text(encoding="utf-8")
    nodes = _nodes(text)
    assert len(nodes) == 9
    assert [(identifier, label) for identifier, label, _color in nodes if identifier == "x1y1"] == [("x1y1", "+")]
    assert sorted(_edges(text)) == sorted([
        ("x0y1", "x1y1"),
        ("x1y0", "x1y1"),
        ("x1y1", "x2y1"),
        ("x1y1", "x1y2"),
    ])


@pytest.mark.parametrize("indexes", [False, True])
def test_terminal_escaping_and_constants(tmp_path: Path, *, indexes: bool) -> None:
    """Quoted terminal names round-trip and placed constants retain their function label."""
    layout = cartesian_gate_layout((4, 1, 1))
    name = 'a"\nb\\c'
    layout.create_pi(name, (0, 0))
    constant = layout.create_gate([], dynamic_truth_table(0), (1, 0))
    wire = layout.create_buf(constant, (2, 0))
    layout.create_po(wire, name, (3, 0))
    destination = tmp_path / "quoted.dot"
    write_dot_layout(layout, str(destination), indexes=indexes)
    constant_label = f"{constant.index}:{constant.generation}: 0" if indexes else "0"
    wire_label = f"{wire.index}:{wire.generation}: BUF" if indexes else "BUF"
    assert _nodes(destination.read_text(encoding="utf-8")) == [
        ("x0y0", name, "snow2"),
        ("x1y0", constant_label, "white"),
        ("x2y0", wire_label, "palegoldenrod"),
        ("x3y0", name, "snow2"),
    ]


@pytest.mark.parametrize(
    ("point", "identifier"),
    [
        ((-2147483648, 2147483647), "xn2147483648y2147483647"),
        ((2147483647, -2147483648), "x2147483647yn2147483648"),
        ((-1, 0), "xn1y0"),
        ((0, -1), "x0yn1"),
    ],
)
def test_signed_coordinate_identifiers(tmp_path: Path, point: tuple[int, int], identifier: str) -> None:
    """Signed boundary coordinates produce distinct valid DOT identifiers on logical edges."""
    layout = cartesian_gate_layout((1, 1, 1))
    source = layout.create_pi("a", point)
    layout.create_buf(source, (0, 0))
    destination = tmp_path / "signed.dot"
    write_dot_layout(layout, str(destination))
    assert _edges(destination.read_text(encoding="utf-8")) == [(identifier, "x0y0")]


def test_clock_colors(tmp_path: Path) -> None:
    """Clock coloring uses every phase independently of gate colors."""
    layout = cartesian_gate_layout((4, 1, 1), "OPEN")
    for phase in range(4):
        layout.assign_clock_number((phase, 0), phase)
    destination = tmp_path / "clock.dot"
    write_dot_layout(layout, str(destination), clock_colors=True)
    assert _nodes(destination.read_text(encoding="utf-8")) == [
        (f"x{phase}y0", "", color) for phase, color in enumerate(["gray94", "gray67", "gray44", "gray17"])
    ]


def test_large_progress_total_and_callback_rollback(tmp_path: Path) -> None:
    """Progress preserves a 64-bit frame area and callback errors leave the destination intact."""
    layout = cartesian_gate_layout((65536, 65536, 1))
    destination = tmp_path / "layout.dot"
    destination.write_text("original", encoding="utf-8")
    reports: list[tuple[str, int, int]] = []
    error = RuntimeError("stop drawing")

    def stop(task: str, completed: int, total: int) -> None:
        """Stop before enumerating the large frame.

        Args:
            task: Drawing phase.
            completed: Completed tile count.
            total: Total ground-tile count.

        """
        reports.append((task, completed, total))
        raise error

    with pytest.raises(RuntimeError) as caught:
        write_dot_layout(layout, str(destination), on_progress=stop)
    assert caught.value is error
    assert reports
    assert reports[0][2] == 65536**2
    assert destination.read_text(encoding="utf-8") == "original"
    assert list(tmp_path.iterdir()) == [destination]
