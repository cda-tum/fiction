# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from pathlib import Path
from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction import physical_design
from mnt.pyfiction.layouts import (
    arrangement,
    cartesian_gate_layout,
    hexagonal_gate_layout,
    shifted_cartesian_gate_layout,
)
from mnt.pyfiction.physical_design import (
    apply_bestagon_library,
    apply_qca_one_library,
    apply_sim7_mol_library,
    apply_topolinano_library,
    orthogonal,
)
from mnt.pyfiction.qca import qca_cell_mode, qca_layout
from mnt.pyfiction.sidb import lattice, sidb_layout
from mnt.pyfiction.sidb.io import read_sqd_layout
from mnt.pyfiction.synthesis import create_and_tt

if TYPE_CHECKING:
    from collections.abc import Callable

    from mnt.pyfiction.networks import technology_network


def test_apply_qca_one_library(mux21: technology_network) -> None:
    """Map the library and report every processed source gate."""
    layout = orthogonal(mux21)

    reports: list[tuple[str, int, int]] = []
    cells = apply_qca_one_library(layout, lambda task, done, total: reports.append((task, done, total)))
    assert isinstance(cells, qca_layout)
    assert cells.num_cells() > 0
    assert cells.get_layout_name() == layout.get_layout_name()
    assert reports[-1][1] == reports[-1][2] > 0


def test_apply_sim7_mol_library(mux21: technology_network) -> None:
    """Map the library and report every processed source gate."""
    layout = orthogonal(mux21)

    reports: list[tuple[str, int, int]] = []
    cell_layout = apply_sim7_mol_library(layout, lambda task, done, total: reports.append((task, done, total)))
    assert reports[-1][1] == reports[-1][2] > 0
    assert cell_layout is not None
    assert cell_layout.num_cells() > 0


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_apply_bestagon_library(mux21: technology_network) -> None:
    """Map the library and report every processed source gate."""

    layout = physical_design.exact(
        mux21,
        layout_type=hexagonal_gate_layout,
        scheme="ROW",
        crossings=True,
        border_io=True,
        layout_arrangement=arrangement.EVEN_ROW,
    )
    assert layout is not None

    reports: list[tuple[str, int, int]] = []
    cells = apply_bestagon_library(layout, lambda task, done, total: reports.append((task, done, total)))
    assert isinstance(cells, sidb_layout)
    assert cells.num_dots() > 0
    assert cells.get_lattice() == lattice.si_100_2x1()
    assert reports[-1][1] == reports[-1][2] > 0


@pytest.mark.skipif(not hasattr(physical_design, "exact"), reason="pyfiction was built without Z3")
def test_apply_topolinano_library(mux21: technology_network) -> None:
    """Map the library and report every processed source gate."""

    layout = physical_design.exact(
        mux21,
        layout_type=shifted_cartesian_gate_layout,
        scheme="COLUMNAR3",
        crossings=True,
        border_io=True,
        technology_specifics=physical_design.technology_constraints.TOPOLINANO,
        layout_arrangement=arrangement.ODD_COLUMN,
    )
    assert layout is not None

    reports: list[tuple[str, int, int]] = []
    apply_topolinano_library(layout, lambda task, done, total: reports.append((task, done, total)))
    assert reports[-1][1] == reports[-1][2] > 0


@pytest.mark.parametrize("name", ["INV", "OR", "NAND", "NOR", "XOR", "XNOR"])
def test_bestagon_static_interfaces(name: str) -> None:
    """Static Bestagon gates retain the reference's complete tagged SiDB placement."""
    layout = hexagonal_gate_layout(arrangement.EVEN_ROW, (3, 3, 2), "ROW", name)
    a = layout.create_pi("x1", (0, 0))
    b = layout.create_pi("x2", (1, 0)) if name != "INV" else None
    gate = layout.create_not(a, (1, 1)) if b is None else getattr(layout, f"create_{name.lower()}")(a, b, (1, 1))
    layout.create_po(gate, "f1", (0, 2))
    cells = apply_bestagon_library(layout, on_progress=None)
    reference = read_sqd_layout(
        str(
            Path(__file__).resolve().parents[2] / "resources" / "bestagon_library" / "multi_tile_layout" / f"{name}.sqd"
        )
    )
    assert cells.num_dots() == reference.num_dots()
    assert [(site, cells.get_dot_tag(site)) for site in cells.sidbs()] == [
        (site, reference.get_dot_tag(site)) for site in reference.sidbs()
    ]
    assert cells.get_layout_name() == name
    assert cells.get_lattice() == lattice.si_100_2x1()


def test_bestagon_disconnected_and() -> None:
    """The static AND tile maps without a solver or connected primary inputs."""
    layout = hexagonal_gate_layout(arrangement.EVEN_ROW, (3, 3), "ROW")
    layout.create_gate([], create_and_tt(), (1, 2))
    assert apply_bestagon_library(layout).num_dots() == 18


@pytest.mark.parametrize("a", [arrangement.ODD_COLUMN, arrangement.EVEN_COLUMN])
def test_bestagon_rejects_shifted_columns(a: arrangement) -> None:
    """Bestagon supports row-shifted hexagonal arrangements."""
    layout = hexagonal_gate_layout(a, (3, 3), "ROW")
    layout.create_pi("a", (0, 0))
    with pytest.raises(ValueError, match="pointy"):
        apply_bestagon_library(layout)


@pytest.mark.parametrize("apply", [apply_qca_one_library, apply_sim7_mol_library])
def test_cartesian_libraries_reject_disconnected_and(apply: Callable[..., Any]) -> None:
    """Translate unsupported gate orientations into ValueError with the gate tile."""
    layout = cartesian_gate_layout((3, 3))
    layout.create_gate([], create_and_tt(), (1, 1))
    with pytest.raises(ValueError, match=r"orientation.*1.*1"):
        apply(layout)


@pytest.mark.parametrize("a", [arrangement.ODD_ROW, arrangement.EVEN_ROW])
def test_topolinano_rejects_shifted_rows(a: arrangement) -> None:
    """ToPoliNano requires a column-shifted gate layout."""
    layout = shifted_cartesian_gate_layout(a, (3, 3))
    layout.create_pi("a", (0, 0))
    with pytest.raises(ValueError, match="column"):
        apply_topolinano_library(layout)


@pytest.mark.parametrize("apply", [apply_qca_one_library, apply_sim7_mol_library])
def test_cartesian_library_unsupported_gate(apply: Callable[..., Any]) -> None:
    """Unsupported XOR gates report their source tile without editing the layout."""
    layout = cartesian_gate_layout((3, 3), "2DDWave")
    a = layout.create_pi("a", (0, 1))
    b = layout.create_pi("b", (1, 0))
    gate = layout.create_xor(a, b, (1, 1))
    layout.create_po(gate, "f", (2, 1))
    with pytest.raises(ValueError, match=r"type.*1.*1"):
        apply(layout)
    assert layout.is_xor(gate)
    assert layout.size() == 4


def test_qca_mapping_copies_clock_zones_and_synchronization() -> None:
    """A mapped delay covers the complete clock zone, including future cells."""
    layout = cartesian_gate_layout((3, 1), "2DDWave", "wire")
    source = layout.create_pi("x", (0, 0))
    wire = layout.create_buf(source, (1, 0))
    layout.create_po(wire, "f", (2, 0))
    layout.assign_synchronization_element((1, 0), 2)
    reports = []
    cells = apply_qca_one_library(layout, lambda *report: reports.append(report))
    assert (cells.width(), cells.height(), cells.layers()) == (15, 5, 1)
    assert (cells.get_tile_size_x(), cells.get_tile_size_y()) == (5, 5)
    assert cells.num_se() == 1
    for x in range(15):
        for y in range(5):
            assert cells.get_clock_number((x, y)) == x // 5
            assert cells.get_synchronization_element((x, y)) == (2 if 5 <= x < 10 else 0)
    assert [cells.get_cell_name(site) for site in cells.cells() if cells.is_pi(site)] == ["x"]
    assert [cells.get_cell_name(site) for site in cells.cells() if cells.is_po(site)] == ["f"]
    counts = [(done, total) for task, done, total in reports if task == "mapping gates"]
    assert counts[0] == (0, 3)
    assert counts[-1] == (3, 3)
    assert counts == sorted(counts)
    assert all(0 <= done <= total == 3 for done, total in counts)
    assert cells.num_cells() == apply_qca_one_library(layout, None).num_cells()


def test_qca_crossing_vias_and_delay() -> None:
    """A crossing preserves layers, adds ground vias, and shares the ground clock delay."""
    layout = cartesian_gate_layout((3, 3, 2), "2DDWave")
    a = layout.create_pi("a", (1, 0))
    vertical = layout.create_buf(a, (1, 1))
    layout.create_po(vertical, "f", (1, 2))
    b = layout.create_pi("b", (0, 1))
    horizontal = layout.create_buf(b, (1, 1, 1))
    layout.create_po(horizontal, "g", (2, 1))
    layout.assign_synchronization_element((1, 1), 2)
    cells = apply_qca_one_library(layout)
    assert cells.layers() == 2
    crossing = [site for site in cells.cells() if site.z == 1]
    assert crossing
    assert all(cells.get_synchronization_element(site) == 2 for site in crossing)
    vias = [site for site in crossing if cells.get_cell_mode(site) == qca_cell_mode.VERTICAL]
    assert len(vias) == 2
    for site in vias:
        assert not cells.is_empty_cell((site.x, site.y, 0))
        assert cells.get_cell_mode((site.x, site.y, 0)) == qca_cell_mode.VERTICAL


@pytest.mark.parametrize(
    ("apply", "size", "patterns"),
    [
        (
            apply_qca_one_library,
            5,
            {
                (0, 1): ["     ", "     ", "  ixx", "     ", "     "],
                (1, 0): ["     ", "     ", "  i  ", "  x  ", "  x  "],
                (1, 1): ["  x  ", "  x  ", "xxxx1", "  x  ", "  x  "],
                (1, 2): ["  x  ", "  x  ", "   xx", "     ", "     "],
                (2, 2): ["     ", "     ", "xxo  ", "     ", "     "],
            },
        ),
        (
            apply_sim7_mol_library,
            10,
            {
                (0, 1): [
                    "          ",
                    "          ",
                    "          ",
                    "          ",
                    "         i",
                    "         i",
                    "          ",
                    "          ",
                    "          ",
                    "          ",
                ],
                (1, 0): [
                    "          ",
                    "          ",
                    "          ",
                    "          ",
                    "          ",
                    "          ",
                    "          ",
                    "          ",
                    "          ",
                    "    ii    ",
                ],
                (1, 1): [
                    "    aa    ",
                    "    aa    ",
                    "    aa    ",
                    "   bbbb   ",
                    "aaabbbbaa1",
                    "aaabbbbaa1",
                    "   bbbb   ",
                    "    cc    ",
                    "    dd    ",
                    "    dd    ",
                ],
                (1, 2): [
                    "    aa    ",
                    "    aa    ",
                    "    aab   ",
                    "    aab dd",
                    "    aa ccd",
                    "    bb ccd",
                    "    bbb dd",
                    "    bbb   ",
                    "          ",
                    "          ",
                ],
                (2, 2): [
                    "          ",
                    "          ",
                    "          ",
                    "          ",
                    "o         ",
                    "o         ",
                    "          ",
                    "          ",
                    "          ",
                    "          ",
                ],
            },
        ),
    ],
    ids=["qca-one", "sim7-mol"],
)
def test_library_maps_complete_or_inverter_tiles(
    apply: Callable[..., Any], size: int, patterns: dict[tuple[int, int], list[str]]
) -> None:
    """Compare occupied cells, empty cells, and physical phases against library reference tiles."""
    layout = cartesian_gate_layout((3, 3), "2DDWave", "or-not")
    a = layout.create_pi("x1", (1, 0))
    b = layout.create_pi("x2", (0, 1))
    gate = layout.create_or(a, b, (1, 1))
    inverter = layout.create_not(gate, (1, 2))
    layout.create_po(inverter, "f1", (2, 2))
    cells = apply(layout)
    expected = {
        (tx * size + x, ty * size + y, 0): character
        for (tx, ty), rows in patterns.items()
        for y, row in enumerate(rows)
        for x, character in enumerate(row)
        if character != " "
    }
    assert {(site.x, site.y, site.z): chr(cells.get_cell_type(site).value) for site in cells.cells()} == expected
    assert (cells.width(), cells.height(), cells.layers()) == (3 * size, 3 * size, 1)
    assert cells.get_layout_name() == "or-not"


@pytest.mark.parametrize("turns", range(4), ids=["north", "east", "south", "west"])
def test_mol_fanout_three_rotations(turns: int) -> None:
    """All physical fanout branches retain the reference's per-cell clock phases."""
    sites = [(1, 0), (2, 1), (1, 2), (0, 1)]
    layout = cartesian_gate_layout((3, 3), "OPEN")
    source = layout.create_pi("x", sites[turns])
    fanout = layout.create_buf(source, (1, 1))
    layout.assign_clock_number(sites[turns], 0)
    layout.assign_clock_number((1, 1), 1)
    for site in sites:
        if site != sites[turns]:
            layout.create_po(fanout, "branch", site)
            layout.assign_clock_number(site, 2)
    cells = apply_sim7_mol_library(layout)
    rows = [
        "    aa    ",
        "    aa    ",
        "    aa    ",
        "   bbbb   ",
        "ddcbbbbcdd",
        "ddcbbbbcdd",
        "   bbbb   ",
        "    cc    ",
        "    dd    ",
        "    dd    ",
    ]
    for _ in range(turns):
        rows = ["".join(row) for row in zip(*rows[::-1], strict=False)]
    expected = {(x, y): char for y, row in enumerate(rows) for x, char in enumerate(row) if char != " "}
    assert {
        (site.x - 10, site.y - 10): chr(cells.get_cell_type(site).value)
        for site in cells.cells()
        if 10 <= site.x < 20 and 10 <= site.y < 20
    } == expected


def test_topolinano_disconnected_wire_tile() -> None:
    """The complete default wire tile maps through the public application API."""
    layout = shifted_cartesian_gate_layout(arrangement.ODD_COLUMN, (4, 4))
    layout.create_buf((2, 2))
    cells = apply_topolinano_library(layout)
    expected = {(8, 8), (8, 9), (9, 9), (10, 9), (10, 10), (11, 10)}
    assert {(site.x, site.y) for site in cells.cells()} == expected
    assert all(chr(cells.get_cell_type(site).value) == "x" for site in cells.cells())
    assert (cells.get_tile_size_x(), cells.get_tile_size_y()) == (4, 4)
    assert (cells.width(), cells.height()) == (16, 18)


@pytest.mark.parametrize(
    ("make_layout", "apply", "outputs"),
    [
        (lambda: cartesian_gate_layout((2, 3)), apply_qca_one_library, [(0, 0), (1, 1), (0, 2)]),
        (
            lambda: shifted_cartesian_gate_layout(arrangement.ODD_COLUMN, (2, 2), "COLUMNAR3"),
            apply_topolinano_library,
            [(1, 0), (1, 1)],
        ),
    ],
    ids=["qca", "topolinano"],
)
def test_library_rejects_primary_input_fanout(
    make_layout: Callable[[], Any], apply: Callable[..., Any], outputs: list[tuple[int, int]]
) -> None:
    """Gate libraries reject unsupported fanout directly on primary-input tiles."""
    layout = make_layout()
    source = layout.create_pi("a", (0, 1))
    for site in outputs:
        layout.create_po(source, "branch", site)
        layout.assign_clock_number(site, 1)
    layout.assign_clock_number((0, 1), 0)
    with pytest.raises(ValueError, match="type"):
        apply(layout)


def test_topolinano_rejects_nand() -> None:
    """Unsupported NAND gates report the tile at the application boundary."""
    layout = shifted_cartesian_gate_layout(arrangement.ODD_COLUMN, (5, 5))
    a = layout.create_pi("a", (1, 1))
    b = layout.create_pi("b", (1, 2))
    gate = layout.create_nand(a, b, (2, 2))
    layout.create_po(gate, "f", (3, 1))
    for site, clock in [((1, 1), 0), ((1, 2), 0), ((2, 2), 1), ((3, 1), 2)]:
        layout.assign_clock_number(site, clock)
    with pytest.raises(ValueError, match=r"type.*2.*2"):
        apply_topolinano_library(layout)
