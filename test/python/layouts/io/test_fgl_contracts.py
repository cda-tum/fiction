# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING
from xml.etree import ElementTree as ET  # ruff: ignore[suspicious-xml-etree-import] -- fixed local test documents

import pytest

from mnt.pyfiction.layouts import (
    cartesian_gate_layout,
    coordinate,
    extent,
    hexagonal_gate_layout,
    layout_input_port,
    shifted_cartesian_gate_layout,
)
from mnt.pyfiction.layouts.io import fgl_parsing_error, read_fgl_layout, write_fgl_layout
from mnt.pyfiction.synthesis import dynamic_truth_table

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path


LEGACY = """<fgl><layout><name>Test</name><topology>cartesian</topology>
<size><x>2</x><y>1</y><z>0</z></size>
<clocking><name>OPEN</name><zones><zone><x>0</x><y>0</y><clock>0</clock></zone></zones></clocking></layout>
<gates>
<gate><id>0</id><type>PI</type><name>pi0</name><loc><x>0</x><y>1</y><z>0</z></loc></gate>
<gate><id>1</id><type>PI</type><name>pi1</name><loc><x>1</x><y>0</y><z>0</z></loc></gate>
<gate><id>2</id><type>AND</type><loc><x>1</x><y>1</y><z>0</z></loc><incoming>
<signal><x>0</x><y>1</y><z>0</z></signal><signal><x>1</x><y>0</y><z>0</z></signal></incoming></gate>
<gate><id>3</id><type>PO</type><name>po0</name><loc><x>2</x><y>1</y><z>0</z></loc><incoming>
<signal><x>1</x><y>1</y><z>0</z></signal></incoming></gate></gates></fgl>"""

VERSION_TWO = """<fgl version="2"><layout><size><x>2</x><y>1</y><z>1</z></size>
<clocking><name>2DDWave</name></clocking><inputs><id>4294967295</id></inputs><outputs><id>2</id></outputs></layout>
<gates><gate><id>2</id><type>PO</type><name/><arity>1</arity>
<loc><x>1</x><y>0</y><z>0</z></loc><incoming><signal><source>4294967295</source><index>0</index>
<input>0</input></signal></incoming></gate><gate><id>4294967295</id><type>PI</type><name/><arity>0</arity>
<loc><x>0</x><y>0</y><z>0</z></loc></gate></gates></fgl>"""


@pytest.fixture
def fgl_path(tmp_path: Path) -> Path:
    """Return the path to an independent, valid legacy FGL file."""
    path = tmp_path / "layout.fgl"
    path.write_text(LEGACY, encoding="utf-8")
    return path


def test_legacy_gate_locations_labels_and_hex_functions(fgl_path: Path) -> None:
    layout = read_fgl_layout(str(fgl_path))
    assert layout.get_extent() == extent(3, 2, 1)
    assert layout.get_layout_name() == "Test"
    assert layout.is_clocking_scheme("OPEN")
    assert [layout.get_name(obj) for obj in layout.pis()] == ["pi0", "pi1"]
    gate = layout.find_object((1, 1))
    assert gate is not None
    assert layout.is_and(gate)
    assert [None if obj is None else layout.get_tile(obj) for obj in layout.inputs(gate)] == [
        coordinate(0, 1),
        coordinate(1, 0),
    ]
    fgl_path.write_text(LEGACY.replace("<type>AND</type>", "<type>B</type>"), encoding="utf-8")
    restored = read_fgl_layout(str(fgl_path))
    gate = restored.find_object((1, 1))
    assert gate is not None
    assert restored.is_le(gate)
    assert restored.object_function(gate).to_hex() == "b"


@pytest.mark.parametrize(
    "field",
    [
        "layout",
        "layout/clocking",
        "layout/clocking/name",
        "layout/clocking/zones",
        "layout/clocking/zones/zone/x",
        "layout/clocking/zones/zone/y",
        "layout/clocking/zones/zone/clock",
        "layout/size",
        "layout/size/x",
        "layout/size/y",
        "layout/size/z",
        "gates/gate/id",
        "gates/gate/type",
        "gates/gate/name",
        "gates/gate/loc",
        "gates/gate/loc/x",
        "gates/gate/loc/y",
        "gates/gate/loc/z",
        "gates/gate[3]/incoming/signal/x",
        "gates/gate[3]/incoming/signal/y",
        "gates/gate[3]/incoming/signal/z",
    ],
)
def test_legacy_requires_structural_fields(fgl_path: Path, field: str) -> None:
    document = ET.fromstring(LEGACY)  # ruff: ignore[suspicious-xml-element-tree-usage] -- fixed local fixture
    parent_path, _, tag = field.rpartition("/")
    parent = document.find(parent_path) if parent_path else document
    assert parent is not None
    child = parent.find(tag)
    assert child is not None
    parent.remove(child)
    fgl_path.write_text(ET.tostring(document, encoding="unicode"), encoding="utf-8")
    with pytest.raises(fgl_parsing_error):
        read_fgl_layout(str(fgl_path))


@pytest.mark.parametrize("xml", [LEGACY[:-6], LEGACY.replace("<fgl>", "<other>").replace("</fgl>", "</other>")])
def test_fgl_requires_a_well_formed_root(fgl_path: Path, xml: str) -> None:
    fgl_path.write_text(xml, encoding="utf-8")
    with pytest.raises(fgl_parsing_error):
        read_fgl_layout(str(fgl_path))


@pytest.mark.parametrize("topology", ["", "<topology/>"])
def test_legacy_missing_topology_defaults_only_for_cartesian(fgl_path: Path, topology: str) -> None:
    fgl_path.write_text(LEGACY.replace("<topology>cartesian</topology>", topology), encoding="utf-8")
    assert read_fgl_layout(str(fgl_path)).get_extent() == extent(3, 2, 1)
    for cls in (hexagonal_gate_layout, shifted_cartesian_gate_layout):
        with pytest.raises(fgl_parsing_error):
            read_fgl_layout(str(fgl_path), layout_type=cls)


@pytest.mark.parametrize("topology", ["unknown", "odd_row_cartesian", "odd_row_hex"])
def test_fgl_rejects_unknown_or_incompatible_topology(fgl_path: Path, topology: str) -> None:
    fgl_path.write_text(LEGACY.replace("cartesian", topology), encoding="utf-8")
    with pytest.raises(fgl_parsing_error):
        read_fgl_layout(str(fgl_path))


@pytest.mark.parametrize("layers", [0, 1])
def test_legacy_maximum_layer_index_becomes_count(fgl_path: Path, layers: int) -> None:
    fgl_path.write_text(LEGACY.replace("<z>0</z></size>", f"<z>{layers}</z></size>"), encoding="utf-8")
    assert read_fgl_layout(str(fgl_path)).layers() == layers + 1


@pytest.mark.parametrize("arity", range(5))
def test_legacy_gate_types_validate_every_arity(fgl_path: Path, arity: int) -> None:
    positions = [(0, 1), (1, 0), (2, 1), (1, 2)]
    gates = "".join(
        f"<gate><id>{i}</id><type>PI</type><name>input</name><loc><x>{x}</x><y>{y}</y><z>0</z></loc></gate>"
        for i, (x, y) in enumerate(positions[:arity])
    )
    incoming = "".join(f"<signal><x>{x}</x><y>{y}</y><z>0</z></signal>" for x, y in positions[:arity])
    template = f"""<fgl><layout><size><x>2</x><y>2</y><z>0</z></size>
<clocking><name>OPEN</name><zones/></clocking></layout>
<gates>{gates}<gate><id>{arity}</id><type>{{kind}}</type><name>target</name><loc><x>1</x><y>1</y><z>0</z></loc>
<incoming>{incoming}</incoming></gate></gates></fgl>"""
    fgl_path.write_text(template.format(kind=["PI", "BUF", "AND", "MAJ", "8000"][arity]), encoding="utf-8")
    layout = read_fgl_layout(str(fgl_path))
    gate = layout.find_object((1, 1))
    assert gate is not None
    assert layout.input_count(gate) == arity
    fgl_path.write_text(template.format(kind="unknown"), encoding="utf-8")
    with pytest.raises(fgl_parsing_error):
        read_fgl_layout(str(fgl_path))


@pytest.mark.parametrize(
    ("field", "value"),
    [("layout/size/x", value) for value in ("-1", "1oops", " ", "4294967296", "18446744073709551616")]
    + [("gates/gate/id", value) for value in ("-1", "1oops", "gate", " ", "2147483648", "18446744073709551616")]
    + [("layout/clocking/zones/zone/clock", "4"), ("layout/clocking/name", "CoolClocking")],
)
def test_fgl_rejects_invalid_numeric_metadata(fgl_path: Path, field: str, value: str) -> None:
    document = ET.fromstring(LEGACY)  # ruff: ignore[suspicious-xml-element-tree-usage] -- fixed local fixture
    element = document.find(field)
    assert element is not None
    element.text = value
    fgl_path.write_text(ET.tostring(document, encoding="unicode"), encoding="utf-8")
    with pytest.raises(fgl_parsing_error):
        read_fgl_layout(str(fgl_path))


def test_version_two_resolves_forward_sources_and_full_ids(fgl_path: Path) -> None:
    fgl_path.write_text(VERSION_TWO, encoding="utf-8")
    layout = read_fgl_layout(str(fgl_path), "München")
    assert layout.get_layout_name() == "München"
    assert not layout.get_input_name(0)
    assert not layout.get_output_name(0)
    assert layout.inputs(layout.po_at(0)) == [layout.pi_at(0)]
    assert layout.get_tile(layout.pi_at(0)) == coordinate(0, 0)


@pytest.mark.parametrize(
    ("old", "new"),
    [
        ("<input>0</input>", "<input>1</input>"),
        ("<index>0</index>", "<index>1</index>"),
        ("<source>4294967295</source>", "<source>999</source>"),
        ("<source>4294967295</source>", "<source>2</source>"),
        ("<outputs><id>2</id>", "<outputs><id>4294967295</id>"),
        ('version="2"', 'version="3"'),
        ("<x>2</x>", "<x>1</x>"),
        ("<loc><x>1</x>", "<loc><x>2</x>"),
        ("</clocking>", "<zones><zone><x>1</x><y>0</y><clock>0</clock></zone></zones></clocking>"),
    ],
)
def test_version_two_rejects_invalid_finished_layout(fgl_path: Path, old: str, new: str) -> None:
    assert old in VERSION_TWO
    fgl_path.write_text(VERSION_TWO.replace(old, new, 1), encoding="utf-8")
    with pytest.raises(fgl_parsing_error):
        read_fgl_layout(str(fgl_path))


@pytest.mark.parametrize("duplicate", [False, True])
def test_version_two_requires_exactly_one_source_per_input(fgl_path: Path, *, duplicate: bool) -> None:
    document = ET.fromstring(VERSION_TWO)  # ruff: ignore[suspicious-xml-element-tree-usage] -- fixed local fixture
    incoming = document.find("gates/gate/incoming")
    assert incoming is not None
    signal = incoming.find("signal")
    assert signal is not None
    if duplicate:
        incoming.append(ET.fromstring(ET.tostring(signal)))  # ruff: ignore[suspicious-xml-element-tree-usage] -- fixed fixture
    else:
        incoming.remove(signal)
    fgl_path.write_text(ET.tostring(document, encoding="unicode"), encoding="utf-8")
    with pytest.raises(fgl_parsing_error):
        read_fgl_layout(str(fgl_path))


@pytest.mark.parametrize("name", ["first\x01second", "first\ufffesecond"])
def test_version_two_validates_selected_name(fgl_path: Path, name: str) -> None:
    fgl_path.write_text(VERSION_TWO, encoding="utf-8")
    with pytest.raises(fgl_parsing_error):
        read_fgl_layout(str(fgl_path), name)
    fgl_path.write_text(VERSION_TWO.replace("<layout>", "<layout><name>stored</name>"), encoding="utf-8")
    assert read_fgl_layout(str(fgl_path), name).get_layout_name() == "stored"


@pytest.mark.parametrize(
    "invalid",
    [
        "<coordinates><coordinate><y>0</y><z>0</z></coordinate></coordinates>",
        "<coordinates><coordinate><x>0</x><y>0</y></coordinate></coordinates>",
        *[
            f"<coordinates><coordinate><x>{x}</x><y>0</y><z>0</z></coordinate></coordinates>"
            for x in ("2147483648", "-2147483649", "-9223372036854775809", "1junk")
        ],
        "<connections><connection><source><x>0</x><y>0</y><z>0</z></source></connection></connections>",
    ],
)
def test_version_two_rejects_malformed_obstructions(fgl_path: Path, invalid: str) -> None:
    fgl_path.write_text(
        VERSION_TWO.replace("</layout>", f"<obstructions>{invalid}</obstructions></layout>"), encoding="utf-8"
    )
    with pytest.raises(fgl_parsing_error):
        read_fgl_layout(str(fgl_path))


@pytest.mark.parametrize("label", ["  ", "\t", "first\r\nsecond\t<&>", " " * 4096 + "\r\nsecond\t<&>ä水"])
def test_fgl_preserves_whitespace_names(tmp_path: Path, label: str) -> None:
    layout = cartesian_gate_layout((3, 1), "2DDWave", label)
    source = layout.create_pi(label, (0, 0))
    wire = layout.create_buf(source, (1, 0))
    layout.set_name(wire, label)
    layout.create_po(wire, label, (2, 0))
    path = tmp_path / "whitespace.fgl"
    write_fgl_layout(layout, str(path))
    restored = read_fgl_layout(str(path))
    assert restored.get_layout_name() == label
    for x in range(3):
        obj = restored.find_object((x, 0))
        assert obj is not None
        assert restored.get_name(obj) == label


def test_fgl_sparse_metadata_ignores_frame_area(tmp_path: Path) -> None:
    layout = cartesian_gate_layout((10**9, 10**9, 2), "2DDWave")
    layout.assign_clock_number((10**9 - 1, 10**9 - 1, 1), 2)
    layout.assign_synchronization_element((10**9 - 1, 10**9 - 1, 1), 7)
    path = tmp_path / "sparse.fgl"
    write_fgl_layout(layout, str(path))
    assert path.stat().st_size < 2000
    restored = read_fgl_layout(str(path))
    assert restored.get_extent() == extent(10**9, 10**9, 2)
    assert restored.get_clock_number((10**9 - 1, 10**9 - 1)) == 2
    assert restored.get_synchronization_element((10**9 - 1, 10**9 - 1, 1)) == 7


@pytest.mark.parametrize(
    "scheme",
    [
        "OPEN3",
        "OPEN4",
        "COLUMNAR3",
        "COLUMNAR4",
        "ROW3",
        "ROW4",
        "2DDWAVE3",
        "2DDWAVE4",
        "2DDWAVEHEX3",
        "2DDWAVEHEX4",
        "BANCS",
    ],
)
def test_fgl_preserves_clock_phases_and_zones(tmp_path: Path, gate_factory: Callable, scheme: str) -> None:
    layout = gate_factory((4, 3, 2), scheme)
    if not layout.is_regularly_clocked():
        layout.assign_clock_number((1, 1, 1), 2)
    layout.create_pi("a", (0, 0))
    path = tmp_path / "clock.fgl"
    write_fgl_layout(layout, str(path))
    restored = read_fgl_layout(str(path), layout_type=type(layout))
    assert restored.num_clocks() == layout.num_clocks()
    assert restored.get_clocking_scheme_name() == layout.get_clocking_scheme_name()
    assert restored.is_regularly_clocked() == layout.is_regularly_clocked()
    assert [restored.get_clock_number((x, y, 1)) for y in range(3) for x in range(4)] == [
        layout.get_clock_number((x, y)) for y in range(3) for x in range(4)
    ]


@pytest.mark.parametrize(
    "problem", ["nonadjacent", "clock", "outside", "override", "synchronization", "partial0", "partial1"]
)
def test_fgl_writer_rejection_preserves_destination(tmp_path: Path, problem: str) -> None:
    layout = cartesian_gate_layout((3, 2), "2DDWave")
    source = layout.create_pi("a", (0, 0))
    if problem == "nonadjacent":
        layout.create_po(source, "f", (2, 0))
    elif problem == "clock":
        layout.create_po(source, "f", (1, 0))
        layout.assign_clock_number((1, 0), 0)
    elif problem == "outside":
        layout.move_object(source, (-1, 0))
    elif problem == "override":
        layout.assign_clock_number((3, 0), 0)
    elif problem == "synchronization":
        layout.assign_synchronization_element((3, 0), 1)
    else:
        other = layout.create_pi("b", (1, 0))
        layout.move_object(source, (0, 1))
        gate = layout.create_and(source, other, (1, 1))
        layout.disconnect(layout_input_port(gate, int(problem[-1])))
    path = tmp_path / "kept.fgl"
    path.write_bytes(b"sentinel")
    with pytest.raises(ValueError):  # ruff: ignore[pytest-raises-too-broad] -- The contract promises a type, not wording.
        write_fgl_layout(layout, str(path))
    assert path.read_bytes() == b"sentinel"
    assert list(tmp_path.iterdir()) == [path]


def test_fgl_keeps_dangling_constants_and_declared_output_order(tmp_path: Path) -> None:
    layout = cartesian_gate_layout((3, 3), "2DDWave")
    b = layout.create_pi("b", (1, 0))
    a = layout.create_pi("a", (0, 1))
    gate = layout.create_lt(a, b, (1, 1))
    passthrough = layout.create_po(a, "pass", (0, 2))
    result = layout.create_po(gate, "f", (2, 1))
    layout.set_input_order([a, b])
    layout.set_output_order([result, passthrough])
    constant = dynamic_truth_table(0)
    constant.create_from_hex_string("1")
    layout.create_gate([], constant, (0, 0))
    path = tmp_path / "ordered.fgl"
    write_fgl_layout(layout, str(path))
    restored = read_fgl_layout(str(path))
    assert restored.size() == 6
    assert [restored.get_input_name(i) for i in range(2)] == ["a", "b"]
    assert [restored.get_output_name(i) for i in range(2)] == ["f", "pass"]
    restored_gate = restored.find_object((1, 1))
    assert restored_gate is not None
    assert [None if obj is None else restored.get_tile(obj) for obj in restored.inputs(restored_gate)] == [
        coordinate(0, 1),
        coordinate(1, 0),
    ]
    restored_constant = restored.find_object((0, 0))
    assert restored_constant is not None
    assert restored.object_function(restored_constant).num_vars() == 0
    assert restored.object_function(restored_constant).to_hex() == "1"


@pytest.mark.parametrize("invalid", [b"\x80", b"\xef\xbf\xbe"])
@pytest.mark.parametrize("location", ["layout", "object"])
def test_version_two_rejects_invalid_name_bytes(fgl_path: Path, invalid: bytes, location: str) -> None:
    xml = VERSION_TWO.encode()
    if location == "layout":
        xml = xml.replace(b"<layout>", b"<layout><name>first" + invalid + b"second</name>")
    else:
        xml = xml.replace(b"<name/>", b"<name>first" + invalid + b"second</name>", 1)
    fgl_path.write_bytes(xml)
    with pytest.raises(fgl_parsing_error):
        read_fgl_layout(str(fgl_path))


def test_fgl_rejects_delay_overflow(fgl_path: Path) -> None:
    xml = LEGACY.replace(
        "</clocking>",
        "<synchronization_elements><element><x>0</x><y>0</y><z>0</z><delay>256</delay>"
        "</element></synchronization_elements></clocking>",
    )
    fgl_path.write_text(xml, encoding="utf-8")
    with pytest.raises(fgl_parsing_error):
        read_fgl_layout(str(fgl_path))


def test_legacy_placement_can_extend_beyond_frame(fgl_path: Path) -> None:
    xml = LEGACY.replace("<size><x>2</x><y>1</y>", "<size><x>0</x><y>0</y>")
    fgl_path.write_text(xml, encoding="utf-8")
    layout = read_fgl_layout(str(fgl_path))
    assert layout.get_extent() == extent(1, 1, 1)
    assert layout.find_object((2, 1)) is not None


def test_fgl_reports_missing_files_and_invalid_output_directory(tmp_path: Path) -> None:
    with pytest.raises(RuntimeError):
        read_fgl_layout(str(tmp_path / "missing.fgl"))
    with pytest.raises(RuntimeError):
        write_fgl_layout(cartesian_gate_layout((1, 1)), str(tmp_path / "missing" / "output.fgl"))


@pytest.mark.parametrize(
    ("kind", "literal"),
    [
        ("and", "8"),
        ("nand", "7"),
        ("or", "e"),
        ("nor", "1"),
        ("lt", "2"),
        ("ge", "d"),
        ("gt", "4"),
        ("le", "b"),
        ("xor", "6"),
        ("xnor", "9"),
    ],
)
def test_fgl_round_trip_keeps_each_binary_function(
    tmp_path: Path, gate_factory: Callable, kind: str, literal: str
) -> None:
    layout = gate_factory((3, 2), "2DDWave", kind)
    a = layout.create_pi("a", (0, 1))
    b = layout.create_pi("b", (1, 0))
    gate = getattr(layout, f"create_{kind}")(a, b, (1, 1))
    layout.create_po(gate, "f", (2, 1))
    path = tmp_path / "binary.fgl"
    write_fgl_layout(layout, str(path))
    restored = read_fgl_layout(str(path), layout_type=type(layout))
    assert restored.size() == 4
    assert restored.get_layout_name() == kind
    restored_gate = restored.find_object((1, 1))
    assert restored_gate is not None
    assert restored.object_function(restored_gate).to_hex() == literal
    assert [None if obj is None else restored.get_name(obj) for obj in restored.inputs(restored_gate)] == ["a", "b"]


def test_fgl_empty_extent_round_trip(tmp_path: Path, gate_factory: Callable) -> None:
    layout = gate_factory((0, 0, 0), "2DDWave", "empty")
    path = tmp_path / "empty.fgl"
    write_fgl_layout(layout, str(path))
    restored = read_fgl_layout(str(path), layout_type=type(layout))
    assert restored.get_extent() == extent()
    assert restored.is_empty()
    assert restored.get_layout_name() == "empty"


def test_legacy_empty_maximum_indices_define_one_tile(fgl_path: Path) -> None:
    fgl_path.write_text(
        "<fgl><layout><name>empty</name><size><x>0</x><y>0</y><z>0</z></size>"
        "<clocking><name>2DDWave</name></clocking></layout></fgl>",
        encoding="utf-8",
    )
    layout = read_fgl_layout(str(fgl_path))
    assert layout.get_extent() == extent(1, 1, 1)
    assert layout.is_empty()
    assert layout.area() == 1


def test_version_two_rejects_a_cycle_outside_output_cones(tmp_path: Path) -> None:
    layout = cartesian_gate_layout((3, 2), "2DDWave")
    source = layout.create_pi("a", (0, 0))
    layout.create_po(source, "f", (1, 0))
    wire = layout.create_buf(source, (0, 1))
    layout.create_buf(wire, (1, 1))
    path = tmp_path / "dangling.fgl"
    write_fgl_layout(layout, str(path))
    assert read_fgl_layout(str(path)).size() == 4
    document = ET.fromstring(path.read_text(encoding="utf-8"))  # ruff: ignore[suspicious-xml-element-tree-usage] -- writer output
    gates = document.findall("gates/gate")
    target = gates[2].find("incoming/signal/source")
    assert target is not None
    target.text = gates[3].findtext("id")
    path.write_text(ET.tostring(document, encoding="unicode"), encoding="utf-8")
    with pytest.raises(fgl_parsing_error):
        read_fgl_layout(str(path))
