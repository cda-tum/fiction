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

from mnt.pyfiction.fcn.io import write_qll_layout
from mnt.pyfiction.inml import inml_layout, inml_magnet_type
from mnt.pyfiction.mol_qca import mol_qca_cell_type, mol_qca_layout
from mnt.pyfiction.qca import qca_cell_mode, qca_cell_type, qca_layout

if TYPE_CHECKING:
    from pathlib import Path


def test_write_mol_qca_phases(tmp_path: Path) -> None:
    """Serialize every phase cell inside a size-based molQCA frame."""
    filename = tmp_path / "mol_qca.qll"

    layout = mol_qca_layout((4, 1), "molQCA")
    for x, cell_type in enumerate([
        mol_qca_cell_type.NORMAL1,
        mol_qca_cell_type.NORMAL2,
        mol_qca_cell_type.NORMAL3,
        mol_qca_cell_type.NORMAL4,
    ]):
        layout.assign_cell_type((x, 0), cell_type)

    write_qll_layout(layout, str(filename))

    document = ET.parse(filename).getroot()  # ruff: ignore[suspicious-xml-element-tree-usage] -- Fixed writer output.
    settings = document.find("technologies/settings")
    assert settings is not None
    assert settings.attrib == {"tech": "MolFCN"}
    assert [item.attrib for item in document.findall("components/item")] == [{"tech": "MolFCN", "name": "Bisferrocene"}]
    assert [
        (item.attrib["x"], item.attrib["y"], item.attrib["layer"], [prop.attrib for prop in item])
        for item in document.findall("layout/item")
    ] == [(str(phase), "0", "0", [{"name": "phase", "value": str(phase)}]) for phase in range(4)]


@pytest.mark.parametrize(
    ("layout", "tech", "layers", "components"),
    [
        (inml_layout((3, 3)), "iNML", "false", ["Magnet", "Coupler", "Cross Wire", "And", "Inverter", "Or"]),
        (qca_layout((3, 3, 2)), "MolFCN", "true", ["Bisferrocene"]),
        (mol_qca_layout((3, 3)), "MolFCN", "false", ["Bisferrocene"]),
    ],
)
def test_empty_layout_settings(
    tmp_path: Path, layout: qca_layout | inml_layout | mol_qca_layout, tech: str, layers: str, components: list[str]
) -> None:
    destination = tmp_path / "layout.qll"
    write_qll_layout(layout, str(destination))
    document = ET.parse(destination).getroot()  # ruff: ignore[suspicious-xml-element-tree-usage] -- Fixed writer output.
    settings = document.find("technologies/settings")
    assert settings is not None
    assert settings.attrib == {"tech": tech}
    properties = {prop.attrib["name"]: prop.attrib["value"] for prop in settings}
    expected_settings = {
        "Layoutwidth": "2",
        "Layoutheight": "2",
        "layersEnabled": layers,
        "PhaseNumber": "4",
        "CZSequence": "4",
    }
    expected_settings.update(
        {
            "Width": "60",
            "Height": "90",
            "Thickness": "10",
            "VDistance": "10",
            "HDistance": "25",
            "DWEnabled": "true",
            "DWMaxSize": "6",
            "DWDefSize": "2",
            "InterlayerSpace": "10",
        }
        if tech == "iNML"
        else {"Intermolecular Distance": "1000"}
    )
    assert properties == expected_settings
    assert [item.attrib for item in document.findall("components/item")] == [
        {"tech": tech, "name": name} for name in components
    ]
    cells = document.find("layout")
    assert cells is not None
    assert list(cells) == []


@pytest.mark.parametrize(
    ("cell_type", "name"), [(mol_qca_cell_type.CONST_0, "const0"), (mol_qca_cell_type.CONST_1, "const1")]
)
def test_mol_qca_constants_are_input_pins(tmp_path: Path, cell_type: mol_qca_cell_type, name: str) -> None:
    layout = mol_qca_layout((1, 1))
    layout.assign_cell_type((0, 0), cell_type)
    destination = tmp_path / "layout.qll"
    write_qll_layout(layout, str(destination))
    document = ET.parse(destination).getroot()  # ruff: ignore[suspicious-xml-element-tree-usage] -- Fixed writer output.
    assert document.findall("layout/item") == []
    assert [pin.attrib for pin in document.findall("layout/pin")] == [
        {"tech": "MolFCN", "name": name, "direction": "0", "id": "1", "x": "0", "y": "0", "layer": "0"}
    ]


def test_qca_and_gate_cells_and_pins(tmp_path: Path) -> None:
    layout = qca_layout((5, 5), "OPEN", "AND")
    for cell in [(2, 1), (1, 2), (2, 2), (3, 2), (2, 3)]:
        layout.assign_cell_type(cell, qca_cell_type.NORMAL)
    layout.assign_cell_type((2, 0), qca_cell_type.CONST_0)
    for cell, cell_type, name in [
        ((0, 2), qca_cell_type.INPUT, "a"),
        ((2, 4), qca_cell_type.INPUT, "b"),
        ((4, 2), qca_cell_type.OUTPUT, "f"),
    ]:
        layout.assign_cell_type(cell, cell_type)
        layout.assign_cell_name(cell, name)
    destination = tmp_path / "layout.qll"
    write_qll_layout(layout, str(destination))
    document = ET.parse(destination).getroot()  # ruff: ignore[suspicious-xml-element-tree-usage] -- Fixed writer output.

    assert [(item.attrib["x"], item.attrib["y"], item.attrib["layer"]) for item in document.findall("layout/item")] == [
        ("2", "1", "0"),
        ("1", "2", "0"),
        ("2", "2", "0"),
        ("3", "2", "0"),
        ("2", "3", "0"),
    ]
    assert [
        (pin.attrib["name"], pin.attrib["direction"], pin.attrib["x"], pin.attrib["y"], pin.attrib["layer"])
        for pin in document.findall("layout/pin")
    ] == [
        ("const0", "0", "2", "0", "0"),
        ("a", "0", "0", "2", "0"),
        ("b", "0", "2", "4", "0"),
        ("f", "1", "4", "2", "0"),
    ]
    assert [prop.attrib for prop in document.findall("layout/item/property")] == [{"name": "phase", "value": "0"}] * 5
    cells = document.find("layout")
    assert cells is not None
    assert [element.attrib["id"] for element in cells] == [str(index) for index in range(1, 10)]


def test_qca_crossing_layers_and_vias(tmp_path: Path) -> None:
    layout = qca_layout((3, 1, 2))
    for cell in [(0, 0), (1, 0), (2, 0), (0, 0, 1), (1, 0, 1), (2, 0, 1)]:
        layout.assign_cell_type(cell, qca_cell_type.NORMAL)
    layout.assign_cell_mode((0, 0), qca_cell_mode.VERTICAL)
    layout.assign_cell_mode((2, 0), qca_cell_mode.VERTICAL)
    destination = tmp_path / "layout.qll"
    write_qll_layout(layout, str(destination))
    document = ET.parse(destination).getroot()  # ruff: ignore[suspicious-xml-element-tree-usage] -- Fixed writer output.
    assert [(item.attrib["x"], item.attrib["y"], item.attrib["layer"]) for item in document.findall("layout/item")] == [
        ("0", "0", "0"),
        ("0", "0", "1"),
        ("1", "0", "0"),
        ("2", "0", "0"),
        ("2", "0", "1"),
        ("0", "0", "2"),
        ("1", "0", "2"),
        ("2", "0", "2"),
    ]


@pytest.mark.parametrize(
    ("extent", "magnets", "pins", "items"),
    [
        pytest.param(
            (5, 5),
            [(1, 0), (2, 0), (2, 1), (1, 2), (2, 2), (3, 2), (2, 3), (1, 4), (2, 4)],
            [(0, 0, "a", 0), (0, 2, "b", 0), (0, 4, "c", 0), (4, 2, "f", 1)],
            [
                (0, 0, "0"),
                (1, 0, "0"),
                (2, 0, "0"),
                (2, 1, "0"),
                (0, 2, "0"),
                (1, 2, "0"),
                (2, 2, "0"),
                (3, 2, "0"),
                (4, 2, "0"),
                (2, 3, "0"),
                (0, 4, "0"),
                (1, 4, "0"),
                (2, 4, "0"),
            ],
            id="majority",
        ),
        pytest.param(
            (6, 3),
            [
                (1, 0, "CROSSWIRE_MAGNET"),
                (1, 2, "CROSSWIRE_MAGNET"),
                (2, 1, "CROSSWIRE_MAGNET"),
                (3, 0, "CROSSWIRE_MAGNET"),
                (3, 2, "CROSSWIRE_MAGNET"),
                (4, 0),
                (4, 2),
            ],
            [(0, 0, "a", 0), (0, 2, "b", 0), (5, 0, "b'", 1), (5, 2, "a'", 1)],
            [(0, 0, "0"), (1, 0, "2"), (4, 0, "0"), (5, 0, "0"), (0, 2, "0"), (4, 2, "0"), (5, 2, "0")],
            id="crosswire",
        ),
        pytest.param(
            (12, 5),
            [
                (1, 2),
                (2, 2),
                (3, 1, "FANOUT_COUPLER_MAGNET"),
                (3, 2, "FANOUT_COUPLER_MAGNET"),
                (3, 3, "FANOUT_COUPLER_MAGNET"),
                (4, 1, "FANOUT_COUPLER_MAGNET"),
                (4, 3, "FANOUT_COUPLER_MAGNET"),
                (5, 1),
                (6, 1),
                (5, 3),
                (6, 3),
                (7, 1, "INVERTER_MAGNET"),
                (8, 1, "INVERTER_MAGNET"),
                (9, 1, "INVERTER_MAGNET"),
                (10, 1),
                (7, 3),
                (8, 3),
                (9, 3),
                (10, 3),
            ],
            [(0, 2, "a", 0), (11, 1, "not a", 1), (11, 3, "a", 1)],
            [
                (3, 0, "1"),
                (5, 0, "0"),
                (6, 0, "0"),
                (7, 0, "4"),
                (11, 0, "0"),
                (0, 1, "0"),
                (1, 1, "0"),
                (2, 1, "0"),
                (5, 2, "0"),
                (6, 2, "0"),
                (7, 2, "0"),
                (8, 2, "0"),
                (9, 2, "0"),
                (10, 2, "0"),
                (11, 2, "0"),
            ],
            id="coupler-inverter",
        ),
    ],
)
def test_inml_components(
    tmp_path: Path,
    extent: tuple[int, int],
    magnets: list[tuple[int, int] | tuple[int, int, str]],
    pins: list[tuple[int, int, str, int]],
    items: list[tuple[int, int, str]],
) -> None:
    """Composite magnets serialize once, while terminals retain names and directions."""
    layout = inml_layout(extent)
    for x, y, *kind in magnets:
        layout.assign_cell_type((x, y), getattr(inml_magnet_type, kind[0] if kind else "NORMAL"))
    for x, y, name, direction in pins:
        layout.assign_cell_type((x, y), inml_magnet_type.OUTPUT if direction else inml_magnet_type.INPUT)
        layout.assign_cell_name((x, y), name)
    destination = tmp_path / "layout.qll"
    write_qll_layout(layout, str(destination))
    document = ET.parse(destination).getroot()  # ruff: ignore[suspicious-xml-element-tree-usage] -- Fixed writer output.

    assert [
        (int(item.attrib["x"]), int(item.attrib["y"]), item.attrib["comp"]) for item in document.findall("layout/item")
    ] == items
    assert [
        (pin.attrib["name"], int(pin.attrib["direction"]), int(pin.attrib["x"]), int(pin.attrib["y"]))
        for pin in document.findall("layout/pin")
    ] == [(name, direction, x, y - min(pin[1] for pin in pins)) for x, y, name, direction in pins]
    assert [prop.attrib for prop in document.findall("layout/item/property[@name='phase']")] == [
        {"name": "phase", "value": "0"}
    ] * len(items)
    assert [prop.attrib for prop in document.findall("layout/item/property[@name='length']")] == (
        [{"name": "length", "value": "4"}] if extent[0] == 12 else []
    )


@pytest.mark.parametrize("invalid_input", [False, True])
def test_rejects_non_border_inml_pins(tmp_path: Path, *, invalid_input: bool) -> None:
    layout = inml_layout((5, 5))
    layout.assign_cell_type((0, 0), inml_magnet_type.NORMAL)
    layout.assign_cell_type((4, 4), inml_magnet_type.NORMAL)
    layout.assign_cell_type((1 if invalid_input else 0, 1), inml_magnet_type.INPUT)
    layout.assign_cell_type((4 if invalid_input else 2, 2), inml_magnet_type.OUTPUT)
    destination = tmp_path / "layout.qll"
    destination.write_text("sentinel", encoding="utf-8")
    with pytest.raises(ValueError, match="borders"):
        write_qll_layout(layout, str(destination))
    assert destination.read_text(encoding="utf-8") == "sentinel"


def test_rejects_invalid_output_path(tmp_path: Path) -> None:
    with pytest.raises(RuntimeError):
        write_qll_layout(qca_layout(), str(tmp_path))
