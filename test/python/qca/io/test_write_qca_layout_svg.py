# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING
from xml.etree import ElementTree as ET  # ruff: ignore[suspicious-xml-etree-import] -- Fixed writer output.

import pytest

from mnt.pyfiction.qca import qca_cell_mode, qca_cell_type, qca_layout
from mnt.pyfiction.qca.io import write_qca_layout_svg

if TYPE_CHECKING:
    from pathlib import Path


@pytest.mark.parametrize("simple", [False, True])
@pytest.mark.parametrize(
    ("cell_type", "clock", "color", "dots"),
    [
        (qca_cell_type.NORMAL, 0, "86e291", 4),
        (qca_cell_type.NORMAL, 1, "ffa5fa", 4),
        (qca_cell_type.NORMAL, 2, "00c8bc", 4),
        (qca_cell_type.NORMAL, 3, "ffffff", 4),
        (qca_cell_type.INPUT, 0, "008dc8", 4),
        (qca_cell_type.OUTPUT, 0, "e28686", 4),
        (qca_cell_type.CONST_0, 0, "000000", 2),
        (qca_cell_type.CONST_1, 0, "000000", 2),
    ],
)
def test_cell_colors_and_details(
    tmp_path: Path, cell_type: qca_cell_type, clock: int, color: str, dots: int, *, simple: bool
) -> None:
    """SVG colors encode cell type and clock; detail mode adds quantum dots."""
    layout = qca_layout((1, 1))
    layout.assign_cell_type((0, 0), cell_type)
    layout.assign_clock_number((0, 0), clock)
    destination = tmp_path / "layout.svg"

    write_qca_layout_svg(layout, str(destination), simple=simple)

    svg = destination.read_text(encoding="utf-8")
    document = ET.fromstring(svg)  # ruff: ignore[suspicious-xml-element-tree-usage] -- Fixed writer output.
    assert document.tag == "{http://www.w3.org/2000/svg}svg"
    assert f"fill:#{color};" in svg
    assert "fill:##" not in svg
    assert len(document.findall(".//{*}rect")) == 1
    assert len(document.findall(".//{*}circle")) == (0 if simple else dots)


@pytest.mark.parametrize("simple", [False, True])
@pytest.mark.parametrize("clock", range(4))
def test_tile_clock_labels(tmp_path: Path, clock: int, *, simple: bool) -> None:
    layout = qca_layout((5, 5), "OPEN", "tile", 5, 5)
    layout.assign_cell_type((2, 2), qca_cell_type.NORMAL)
    layout.assign_clock_number((0, 0), clock)
    destination = tmp_path / "layout.svg"

    write_qca_layout_svg(layout, str(destination), simple=simple)

    document = ET.parse(destination)  # ruff: ignore[suspicious-xml-element-tree-usage] -- Fixed writer output.
    assert [element.text for element in document.findall(".//{*}tspan") if element.text] == (
        [] if simple else [str(clock + 1)]
    )
    assert len(document.findall(".//{*}circle")) == (0 if simple else 4)
    assert "fill:#;" not in destination.read_text(encoding="utf-8")


@pytest.mark.parametrize("simple", [False, True])
@pytest.mark.parametrize(("clock", "lower_clock"), [(0, 1), (3, 0)])
def test_synchronized_tile(tmp_path: Path, clock: int, lower_clock: int, *, simple: bool) -> None:
    layout = qca_layout((5, 5), "OPEN", "synchronized", 5, 5)
    layout.assign_cell_type((2, 2), qca_cell_type.NORMAL)
    layout.assign_cell_type((1, 2), qca_cell_type.INPUT)
    layout.assign_clock_number((0, 0), clock)
    layout.assign_synchronization_element((0, 0), 1)
    destination = tmp_path / "layout.svg"

    write_qca_layout_svg(layout, str(destination), simple=simple)

    svg = destination.read_text(encoding="utf-8")
    document = ET.fromstring(svg)  # ruff: ignore[suspicious-xml-element-tree-usage] -- Fixed writer output.
    assert "fill:#ffe33a;" in svg
    assert "fill:#008dc8;" in svg
    assert len(document.findall(".//{*}rect")) == 3
    assert len(document.findall(".//{*}circle")) == (0 if simple else 8)
    assert sorted(element.text for element in document.findall(".//{*}tspan") if element.text) == (
        [] if simple else sorted([str(clock + 1), str(lower_clock + 1)])
    )


@pytest.mark.parametrize("simple", [False, True])
def test_synchronization_preserves_cell_position(tmp_path: Path, *, simple: bool) -> None:
    layout = qca_layout((1, 1))
    layout.assign_cell_type((0, 0), qca_cell_type.NORMAL)
    destination = tmp_path / "layout.svg"
    write_qca_layout_svg(layout, str(destination), simple=simple)
    original = ET.parse(destination).find("./{*}g/{*}g")  # ruff: ignore[suspicious-xml-element-tree-usage] -- Fixed writer output.

    layout.assign_synchronization_element((0, 0), 1)
    write_qca_layout_svg(layout, str(destination), simple=simple)
    synchronized = ET.parse(destination).find("./{*}g/{*}g")  # ruff: ignore[suspicious-xml-element-tree-usage] -- Fixed writer output.

    assert original is not None
    assert synchronized is not None
    assert synchronized.attrib["transform"] == original.attrib["transform"]


@pytest.mark.parametrize("simple", [False, True])
@pytest.mark.parametrize(("width", "full_width", "rectangles", "dots"), [(1, 5, 2, 4), (6, 10, 4, 8)])
def test_partial_boundary_tiles(
    tmp_path: Path, width: int, full_width: int, rectangles: int, dots: int, *, simple: bool
) -> None:
    layout = qca_layout((width, 1), "OPEN", "partial tile", 5, 5)
    for x in range(0, width, 5):
        layout.assign_cell_type((x, 0), qca_cell_type.NORMAL)
    destination = tmp_path / "layout.svg"
    write_qca_layout_svg(layout, str(destination), simple=simple)
    partial = ET.parse(destination).getroot()  # ruff: ignore[suspicious-xml-element-tree-usage] -- Fixed writer output.

    layout.resize((full_width, 5))
    write_qca_layout_svg(layout, str(destination), simple=simple)
    full = ET.parse(destination).getroot()  # ruff: ignore[suspicious-xml-element-tree-usage] -- Fixed writer output.

    assert partial.attrib["viewBox"] == full.attrib["viewBox"]
    for document in [partial, full]:
        assert len(document.findall(".//{*}rect")) == rectangles
        assert len(document.findall(".//{*}circle")) == (0 if simple else dots)


@pytest.mark.parametrize("simple", [False, True])
@pytest.mark.parametrize("tile_size", [1, 5])
def test_clips_cells_to_frame(tmp_path: Path, tile_size: int, *, simple: bool) -> None:
    layout = qca_layout((3, 3, 2), "OPEN", "crossing", tile_size, tile_size)
    layout.assign_cell_type((1, 1), qca_cell_type.NORMAL)
    layout.assign_cell_type((1, 1, 1), qca_cell_type.NORMAL)
    layout.assign_cell_mode((1, 1, 1), qca_cell_mode.VERTICAL)
    destination = tmp_path / "layout.svg"
    write_qca_layout_svg(layout, str(destination), simple=simple)
    expected = destination.read_text(encoding="utf-8")

    for cell, cell_type in [
        ((-1, 1), qca_cell_type.INPUT),
        ((3, 1), qca_cell_type.OUTPUT),
        ((1, 1, 2), qca_cell_type.NORMAL),
    ]:
        layout.assign_cell_type(cell, cell_type)
    write_qca_layout_svg(layout, str(destination), simple=simple)

    assert destination.read_text(encoding="utf-8") == expected


@pytest.mark.parametrize("simple", [False, True])
@pytest.mark.parametrize("tile_size", [1, 5])
def test_paints_lower_layers_first(tmp_path: Path, tile_size: int, *, simple: bool) -> None:
    layout = qca_layout((10, 5, 2), "OPEN", "layers", tile_size, tile_size)
    layout.assign_cell_type((1, 1, 1), qca_cell_type.OUTPUT)
    layout.assign_cell_type((1, 1), qca_cell_type.INPUT)
    destination = tmp_path / "layout.svg"

    write_qca_layout_svg(layout, str(destination), simple=simple)

    svg = destination.read_text(encoding="utf-8")
    assert svg.index("fill:#008dc8") < svg.index("fill:#e28686")


@pytest.mark.parametrize("simple", [False, True])
def test_rejects_invalid_output_paths(tmp_path: Path, *, simple: bool) -> None:
    layout = qca_layout((1, 1))
    with pytest.raises(RuntimeError):
        write_qca_layout_svg(layout, str(tmp_path), simple=simple)
    with pytest.raises(RuntimeError):
        write_qca_layout_svg(layout, str(tmp_path / "missing" / "layout.svg"), simple=simple)
    assert not (tmp_path / "missing").exists()
