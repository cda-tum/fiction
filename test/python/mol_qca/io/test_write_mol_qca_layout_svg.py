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

from mnt.pyfiction.mol_qca import mol_qca_cell_type, mol_qca_layout
from mnt.pyfiction.mol_qca.io import write_mol_qca_layout_svg

if TYPE_CHECKING:
    from pathlib import Path


@pytest.mark.parametrize("simple", [False, True])
def test_cell_types_and_molecules(tmp_path: Path, *, simple: bool) -> None:
    """molQCA SVG preserves cell colors, positions, and constant labels."""
    layout = mol_qca_layout((7, 1), "molQCA SVG")
    for x, cell_type in enumerate([
        mol_qca_cell_type.NORMAL1,
        mol_qca_cell_type.INPUT,
        mol_qca_cell_type.OUTPUT,
        mol_qca_cell_type.CONST_0,
        mol_qca_cell_type.CONST_1,
    ]):
        layout.assign_cell_type((x, 0), cell_type)
    destination = tmp_path / "layout.svg"

    write_mol_qca_layout_svg(layout, str(destination), simple=simple)

    document = ET.parse(destination).getroot()  # ruff: ignore[suspicious-xml-element-tree-usage] -- Fixed writer output.
    assert document.tag == "{http://www.w3.org/2000/svg}svg"
    assert [float(value) for value in document.attrib["viewBox"].split()] == [0, 0, 102.5, 33.5]
    cells = document.findall("./{*}g/{*}g")
    assert len(cells) == 5
    assert [rect.attrib["style"].split(";")[0] for rect in document.findall(".//{*}rect")] == [
        "fill:#ffe700",
        "fill:#cccccc",
        "fill:#4a4a4a",
        "fill:#000000",
        "fill:#000000",
    ]
    translations = [cell.attrib["transform"][10:-1].split(",") for cell in cells]
    assert [float(x) for x, _ in translations] == pytest.approx([-78.56596, -67.06596, -55.56596, -44.06596, -32.56596])
    assert [float(y) for _, y in translations] == pytest.approx([-41.78984] * 5)
    assert len(document.findall(".//{*}rect")) == 5
    assert len(document.findall(".//{*}circle")) == (0 if simple else 18)
    assert len(document.findall(".//{*}line")) == (0 if simple else 6)
    assert [
        tuple(rect.attrib[key] for key in ("x", "y", "width", "height")) for rect in document.findall(".//{*}rect")
    ] == [("92.93404", "56.46016", "10", "10")] * 5
    for cell in cells[:3]:
        assert [tuple(circle.attrib[key] for key in ("cx", "cy", "r")) for circle in cell.findall(".//{*}circle")] == (
            []
            if simple
            else [
                ("95.43404", "58.96016", "0.97553"),
                ("95.43404", "61.46016", "0.50000"),
                ("95.43404", "63.96016", "0.97553"),
                ("100.43404", "58.96016", "0.97553"),
                ("100.43404", "61.46016", "0.50000"),
                ("100.43404", "63.96016", "0.97553"),
            ]
        )
        assert [tuple(line.attrib[key] for key in ("x1", "y1", "x2", "y2")) for line in cell.findall(".//{*}line")] == (
            []
            if simple
            else [("95.43404", "58.96016", "95.43404", "63.96016"), ("100.43404", "58.96016", "100.43404", "63.96016")]
        )
    assert [(element.text or "").strip() for element in document.findall(".//{*}text")] == (
        [] if simple else ["C0", "C1"]
    )


def test_default_is_detailed(tmp_path: Path) -> None:
    layout = mol_qca_layout((1, 1))
    layout.assign_cell_type((0, 0), mol_qca_cell_type.CONST_0)
    destination = tmp_path / "layout.svg"
    write_mol_qca_layout_svg(layout, str(destination))
    assert [(text.text or "").strip() for text in ET.parse(destination).findall(".//{*}text")] == ["C0"]  # ruff: ignore[suspicious-xml-element-tree-usage] -- Fixed writer output.


def test_rejects_invalid_output_paths(tmp_path: Path) -> None:
    layout = mol_qca_layout((1, 1))
    with pytest.raises(RuntimeError):
        write_mol_qca_layout_svg(layout, str(tmp_path / "missing" / "layout.svg"))
    assert not (tmp_path / "missing").exists()
    with pytest.raises(RuntimeError):
        write_mol_qca_layout_svg(layout, str(tmp_path))
