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

from mnt.pyfiction.inml import inml_layout, inml_magnet_type
from mnt.pyfiction.inml.io import write_qcc_layout

if TYPE_CHECKING:
    from pathlib import Path


@pytest.mark.parametrize("use_filename", [False, True])
def test_dimensions_terminals_and_component_name(tmp_path: Path, *, use_filename: bool) -> None:
    """QCC stores maximum indices, boundary pins, and the selected component name."""
    layout = inml_layout((3, 1), "OPEN", "wire")
    for x, cell_type, name in [
        (0, inml_magnet_type.INPUT, "a"),
        (1, inml_magnet_type.NORMAL, ""),
        (2, inml_magnet_type.OUTPUT, "f"),
    ]:
        layout.assign_cell_type((x, 0), cell_type)
        layout.assign_cell_name((x, 0), name)
    destination = tmp_path / "component.qcc"

    write_qcc_layout(layout, str(destination), use_filename_as_component_name=use_filename)

    document = ET.parse(destination).getroot()  # ruff: ignore[suspicious-xml-element-tree-usage] -- Fixed writer output.
    assert document.tag == "qcacomponent"
    assert document.attrib["maxX"] == "2"
    assert document.attrib["maxY"] == "0"
    assert document.attrib["name"] == ("component" if use_filename else "wire")
    assert document.attrib["MagnetsCount"] == "3"
    assert [
        (pin.attrib["name"], pin.attrib["direction"], pin.attrib["x"], pin.attrib["y"])
        for pin in document.findall("entity/pin")
    ] == [("a", "0", "0", "0"), ("f", "1", "2", "0")]
    assert [(item.attrib["x"], item.attrib["y"]) for item in document.findall("layout/item")] == [
        ("0", "0"),
        ("1", "0"),
        ("2", "0"),
    ]


@pytest.mark.parametrize(
    ("cell_type", "x"),
    [
        (inml_magnet_type.INPUT, 1),
        (inml_magnet_type.INPUT, 2),
        (inml_magnet_type.OUTPUT, 0),
        (inml_magnet_type.OUTPUT, 1),
    ],
)
def test_rejects_misplaced_terminals_then_recovers(tmp_path: Path, cell_type: inml_magnet_type, x: int) -> None:
    layout = inml_layout((3, 1))
    for column in range(3):
        layout.assign_cell_type((column, 0), inml_magnet_type.NORMAL)
    layout.assign_cell_type((x, 0), cell_type)
    destination = tmp_path / "layout.qcc"
    destination.write_text("sentinel", encoding="utf-8")

    with pytest.raises(ValueError, match="borders"):
        write_qcc_layout(layout, str(destination))
    assert destination.read_text(encoding="utf-8") == "sentinel"

    layout.assign_cell_type((x, 0), inml_magnet_type.NORMAL)
    write_qcc_layout(layout, str(destination))
    assert ET.parse(destination).getroot().tag == "qcacomponent"  # ruff: ignore[suspicious-xml-element-tree-usage] -- Fixed writer output.
