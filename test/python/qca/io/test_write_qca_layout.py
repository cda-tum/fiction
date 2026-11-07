# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import re
from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.qca import qca_cell_mode, qca_cell_type, qca_layout
from mnt.pyfiction.qca.io import write_qca_layout
from mnt.pyfiction.utils import __repo__, __version__

if TYPE_CHECKING:
    from pathlib import Path


@pytest.mark.parametrize("name", ["a\nb", "a\rb", "a\0b"])
@pytest.mark.parametrize("cell_name", [False, True])
def test_reject_invalid_names_without_changing_file(tmp_path: Path, name: str, *, cell_name: bool) -> None:
    """Reject names that cannot fit one QCADesigner text record."""
    layout = qca_layout((1, 1), "OPEN", "valid" if cell_name else name)
    layout.assign_cell_type((0, 0), qca_cell_type.INPUT)
    layout.assign_cell_name((0, 0), name if cell_name else "valid")
    destination = tmp_path / "layout.qca"
    destination.write_text("sentinel", encoding="utf-8")

    with pytest.raises(ValueError, match="line breaks or null bytes"):
        write_qca_layout(layout, str(destination))

    assert destination.read_text(encoding="utf-8") == "sentinel"


@pytest.mark.parametrize("create_vias", [False, True])
def test_empty_layout_headers_and_layers(tmp_path: Path, *, create_vias: bool) -> None:
    layout = qca_layout((3, 3, 2), "OPEN", "empty layout")
    destination = tmp_path / "layout.qca"
    write_qca_layout(layout, str(destination), create_inter_layer_via_cells=create_vias)
    output = destination.read_text(encoding="utf-8")

    assert output.startswith("[VERSION]\nqcadesigner_version=2.000000\n[#VERSION]\n")
    assert f"name=empty layout\ngenerated_by={__version__}\nrepo={__repo__}\n" in output
    assert re.findall(r"^pszDescription=(.*)$", output, re.MULTILINE) == [
        "Drawing Layer",
        "Substrate",
        "Ground Layer",
        "Crossing Layer 1",
    ]
    assert "[TYPE:QCADCell]" not in output
    assert output.endswith("[#TYPE:DESIGN]\n")


def test_representable_names_and_clipping(tmp_path: Path) -> None:
    name = "a=b\t<>&\u00b5"
    layout = qca_layout((2, 1), "OPEN", name)
    layout.assign_cell_type((0, 0), qca_cell_type.INPUT)
    layout.assign_cell_name((0, 0), name)
    layout.assign_cell_type((1, 0), qca_cell_type.CONST_0)
    destination = tmp_path / "layout.qca"
    write_qca_layout(layout, str(destination))
    expected = destination.read_text(encoding="utf-8")

    layout.assign_cell_name((1, 0), "ignored\nconstant name")
    for cell in [(-1, 0), (2, 0)]:
        layout.assign_cell_type(cell, qca_cell_type.INPUT)
        layout.assign_cell_name(cell, "ignored\noutside cell")
    write_qca_layout(layout, str(destination))

    assert destination.read_text(encoding="utf-8") == expected
    assert f"name={name}\n" in expected
    assert f"psz={name}\n" in expected


@pytest.mark.parametrize(
    ("cell_type", "function", "charges", "label"),
    [
        (qca_cell_type.NORMAL, "NORMAL", ["8.010882e-20"] * 4, "a"),
        (qca_cell_type.INPUT, "INPUT", ["8.010882e-20"] * 4, "a"),
        (qca_cell_type.OUTPUT, "OUTPUT", ["8.010882e-20"] * 4, "a"),
        (qca_cell_type.CONST_0, "FIXED", ["0.000000e+00", "1.602176e-19", "0.000000e+00", "1.602176e-19"], "-1.00"),
        (qca_cell_type.CONST_1, "FIXED", ["1.602176e-19", "0.000000e+00", "1.602176e-19", "0.000000e+00"], "1.00"),
    ],
)
def test_cell_records(tmp_path: Path, cell_type: qca_cell_type, function: str, charges: list[str], label: str) -> None:
    """QCAD records retain cell geometry, type, polarization, and labels."""
    layout = qca_layout((3, 4))
    layout.assign_cell_type((2, 3), cell_type)
    layout.assign_cell_name((2, 3), "a")
    layout.assign_clock_number((2, 3), 2)
    destination = tmp_path / "layout.qca"
    write_qca_layout(layout, str(destination))
    output = destination.read_text(encoding="utf-8")
    cells = re.findall(r"\[TYPE:QCADCell\]\n(.*?)\[#TYPE:QCADCell\]\n", output, re.DOTALL)

    assert len(cells) == 1
    cell = cells[0]
    assert "x=140.000000\ny=160.000000\n" in cell
    assert "cell_options.cxCell=18\ncell_options.cyCell=18\ncell_options.dot_diameter=5\n" in cell
    assert "cell_options.clock=2\ncell_options.mode=QCAD_CELL_MODE_NORMAL\n" in cell
    assert f"cell_function=QCAD_CELL_{function}\nnumber_of_dots=4\n" in cell
    expected_color = {
        qca_cell_type.NORMAL: (0, 65535, 65535),
        qca_cell_type.INPUT: (0, 0, 65535),
        qca_cell_type.OUTPUT: (65535, 65535, 0),
        qca_cell_type.CONST_0: (65535, 32768, 0),
        qca_cell_type.CONST_1: (65535, 32768, 0),
    }[cell_type]
    red, green, blue = expected_color
    assert f"clr.red={red}\nclr.green={green}\nclr.blue={blue}\n" in cell
    dots = re.findall(r"\[TYPE:CELL_DOT\]\n(.*?)\[#TYPE:CELL_DOT\]\n", cell, re.DOTALL)
    assert [re.findall(r"^([xy])=(.*)$", dot, re.MULTILINE) for dot in dots] == [
        [("x", "144.500000"), ("y", "155.500000")],
        [("x", "144.500000"), ("y", "164.500000")],
        [("x", "135.500000"), ("y", "164.500000")],
        [("x", "135.500000"), ("y", "155.500000")],
    ]
    assert [re.findall(r"^charge=(.*)$", dot, re.MULTILINE) for dot in dots] == [[charge] for charge in charges]
    expected_spin = (
        "-319472243083548083355648.000000" if cell_type in (qca_cell_type.INPUT, qca_cell_type.OUTPUT) else "0.000000"
    )
    assert [re.findall(r"^spin=(.*)$", dot, re.MULTILINE) for dot in dots] == [[expected_spin]] * 4
    assert all("diameter=5\n" in dot and "potential=0.000000\n" in dot for dot in dots)
    assert re.findall(r"^psz=(.*)$", cell, re.MULTILINE) == ([label] if label else [])


@pytest.mark.parametrize(
    ("coordinate", "center", "positive_dot", "negative_dot"),
    [
        (1000000, "20000100.000000", "20000104.500000", "20000095.500000"),
        (107374183, "2147483760.000000", "2147483764.500000", "2147483755.500000"),
        (2147483647, "42949673040.000000", "42949673044.500000", "42949673035.500000"),
    ],
)
@pytest.mark.parametrize("along_y", [False, True])
def test_large_coordinate_dot_offsets(
    tmp_path: Path, coordinate: int, center: str, positive_dot: str, negative_dot: str, *, along_y: bool
) -> None:
    layout = qca_layout((1, coordinate + 1) if along_y else (coordinate + 1, 1))
    layout.assign_cell_type((0, coordinate) if along_y else (coordinate, 0), qca_cell_type.NORMAL)
    destination = tmp_path / "layout.qca"
    write_qca_layout(layout, str(destination))
    output = destination.read_text(encoding="utf-8")
    axis = "y" if along_y else "x"
    assert f"{axis}={center}\n" in output
    assert f"{axis}={positive_dot}\n" in output
    assert f"{axis}={negative_dot}\n" in output


def test_long_label_width(tmp_path: Path) -> None:
    layout = qca_layout((1, 1))
    layout.assign_cell_type((0, 0), qca_cell_type.INPUT)
    layout.assign_cell_name((0, 0), "x" * 3355443)
    destination = tmp_path / "layout.qca"
    write_qca_layout(layout, str(destination))
    assert "bounding_box.cxWorld=33554434.000000\n" in destination.read_text(encoding="utf-8")


@pytest.mark.parametrize("create_vias", [False, True])
def test_optional_via_layer_before_empty_crossing_layer(tmp_path: Path, *, create_vias: bool) -> None:
    layout = qca_layout((3, 3, 2))
    for cell in [(0, 2), (2, 0)]:
        layout.assign_cell_type(cell, qca_cell_type.NORMAL)
        layout.assign_cell_mode(cell, qca_cell_mode.VERTICAL)
    destination = tmp_path / "layout.qca"
    write_qca_layout(layout, str(destination), create_inter_layer_via_cells=create_vias)
    output = destination.read_text(encoding="utf-8")

    assert re.findall(r"^pszDescription=(.*)$", output, re.MULTILINE) == [
        "Drawing Layer",
        "Substrate",
        "Ground Layer",
        *(["Via Layer 1"] if create_vias else []),
        "Crossing Layer 1",
    ]
    assert output.count("[TYPE:QCADCell]\n") == (4 if create_vias else 2)
    if create_vias:
        via = output.split("pszDescription=Via Layer 1\n")[1].split("pszDescription=Crossing Layer 1\n")[0]
        assert via.index("x=140.000000\ny=100.000000\n") < via.index("x=100.000000\ny=140.000000\n")


def test_crossing_modes_and_default_vias(tmp_path: Path) -> None:
    layout = qca_layout((3, 1, 2))
    layout.assign_cell_type((0, 0), qca_cell_type.INPUT)
    layout.assign_cell_mode((0, 0), qca_cell_mode.VERTICAL)
    layout.assign_cell_type((2, 0), qca_cell_type.NORMAL)
    layout.assign_cell_mode((2, 0), qca_cell_mode.ROTATED)
    layout.assign_cell_type((1, 0, 1), qca_cell_type.NORMAL)
    layout.assign_cell_mode((1, 0, 1), qca_cell_mode.CROSSOVER)
    layout.assign_cell_type((2, 0, 1), qca_cell_type.NORMAL)
    layout.assign_cell_mode((2, 0, 1), qca_cell_mode.ROTATED)
    destination = tmp_path / "layout.qca"
    write_qca_layout(layout, str(destination))
    output = destination.read_text(encoding="utf-8")
    assert re.findall(r"^cell_options.mode=(.*)$", output, re.MULTILINE) == [
        "QCAD_CELL_MODE_VERTICAL",
        "QCAD_CELL_MODE_ROTATED",
        "QCAD_CELL_MODE_VERTICAL",
        "QCAD_CELL_MODE_CROSSOVER",
        "QCAD_CELL_MODE_CROSSOVER",
    ]
