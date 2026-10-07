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

from mnt.pyfiction.layouts import (
    LayoutInputPort,
    arrangement,
    cartesian_gate_layout,
    coordinate,
    shifted_cartesian_gate_layout,
)
from mnt.pyfiction.layouts.io import (
    fgl_parsing_error,
    read_cartesian_fgl_layout,
    read_hexagonal_fgl_layout,
    read_shifted_cartesian_fgl_layout,
    write_fgl_layout,
)
from mnt.pyfiction.physical_design import hexagonalization, orthogonal
from mnt.pyfiction.verification import eq_type, equivalence_checking

if TYPE_CHECKING:
    from pathlib import Path

    from mnt.pyfiction.networks import technology_network


def test_read_write(mux21: technology_network, tmp_path: Path) -> None:
    """Round-trip Cartesian, hexagonal, and shifted Cartesian layouts."""
    cart_layout = orthogonal(mux21)
    cart_file = str(tmp_path / "mux21_cartesian.fgl")
    write_fgl_layout(cart_layout, cart_file)
    assert equivalence_checking(read_cartesian_fgl_layout(cart_file), cart_layout) == eq_type.STRONG

    hex_layout = hexagonalization(cart_layout)
    hex_file = str(tmp_path / "mux21_hexagonal.fgl")
    write_fgl_layout(hex_layout, hex_file)
    assert equivalence_checking(read_hexagonal_fgl_layout(hex_file), hex_layout) == eq_type.STRONG

    shifted_layout = shifted_cartesian_gate_layout(arrangement.ODD_COLUMN, (3, 3, 1), "2DDWave", "Layout")
    shifted_file = str(tmp_path / "empty_shifted_cartesian.fgl")
    write_fgl_layout(shifted_layout, shifted_file)
    assert equivalence_checking(read_shifted_cartesian_fgl_layout(shifted_file), shifted_layout) == eq_type.STRONG


@pytest.mark.parametrize("value", ["2147483649", "-1", "18446744073709551616", "1garbage"])
def test_fgl_rejects_unrepresentable_layers(tmp_path: Path, value: str) -> None:
    """Reject invalid version-2 layer counts."""
    layout = cartesian_gate_layout((2, 1, 1), "2DDWave", "wire")
    source = layout.create_pi("a", (0, 0))
    layout.create_po(source, "f", (1, 0))
    path = tmp_path / "invalid.fgl"
    write_fgl_layout(layout, str(path))
    path.write_text(path.read_text(encoding="utf-8").replace("<z>1</z>", f"<z>{value}</z>", 1), encoding="utf-8")
    with pytest.raises(fgl_parsing_error, match=r"range|integer"):
        read_cartesian_fgl_layout(str(path))


def test_fgl_preserves_labels_and_synchronization(tmp_path: Path) -> None:
    """Preserve XML labels and synchronization delays."""
    layout = cartesian_gate_layout((3, 1, 1), "2DDWave", "A & B < C")
    source = layout.create_pi("a&b", (0, 0))
    wire = layout.create_buf(source, (1, 0))
    layout.create_po(wire, "f<g", (2, 0))
    layout.assign_synchronization_element((1, 0), 2)
    path = tmp_path / "sync.fgl"
    write_fgl_layout(layout, str(path))
    restored = read_cartesian_fgl_layout(str(path))
    assert restored.get_layout_name() == "A & B < C"
    assert restored.num_se() == 1
    assert restored.get_synchronization_element((1, 0)) == 2
    assert equivalence_checking(restored, layout) == eq_type.STRONG


def test_fgl_preserves_interface_order(tmp_path: Path) -> None:
    """Preserve declared interfaces separately from object allocation order."""
    layout = cartesian_gate_layout((3, 2, 1), "2DDWave", "ordered")
    b = layout.create_pi("b", (1, 0, 0))
    a = layout.create_pi("a", (0, 1, 0))
    gate = layout.create_lt(a, b, (1, 1, 0))
    layout.create_po(gate, "f", (2, 1, 0))
    layout.set_input_order([a.object, b.object])
    path = tmp_path / "ordered.fgl"
    write_fgl_layout(layout, str(path))
    assert '<fgl version="2">' in path.read_text(encoding="utf-8")
    restored = read_cartesian_fgl_layout(str(path))
    assert restored.get_input_name(0) == "a"
    assert restored.get_input_name(1) == "b"
    assert equivalence_checking(restored, layout) == eq_type.STRONG


def test_fgl_failure_preserves_file(tmp_path: Path) -> None:
    """Reject incomplete and cyclic objects before replacing an existing file."""
    layout = cartesian_gate_layout((2, 1, 1), "2DDWave", "editing")
    wire = layout.create_buf((0, 0, 0))
    path = tmp_path / "kept.fgl"
    path.write_text("sentinel", encoding="utf-8")
    with pytest.raises(ValueError, match="connected"):
        write_fgl_layout(layout, str(path))
    assert path.read_text(encoding="utf-8") == "sentinel"
    layout.connect(wire, LayoutInputPort(wire.object, 0))
    with pytest.raises(ValueError, match="acyclic"):
        write_fgl_layout(layout, str(path))
    assert path.read_text(encoding="utf-8") == "sentinel"


def test_fgl_preserves_manual_obstructions(tmp_path: Path) -> None:
    """Preserve signed manual constraints independently of occupied objects."""
    layout = cartesian_gate_layout((3, 2, 1), "2DDWave", "manual")
    source = layout.create_pi("a", (0, 0))
    wire = layout.create_buf(source, (1, 0))
    layout.create_po(wire, "f", (2, 0))
    positions = [coordinate(1, 0), coordinate(-1, -2, -3), coordinate(7, 8, 9)]
    for position in positions:
        layout.obstruct_coordinate(position)
    layout.obstruct_connection(positions[1], positions[2])
    path = tmp_path / "manual.fgl"
    write_fgl_layout(layout, str(path))
    restored = read_cartesian_fgl_layout(str(path))
    assert len(restored.obstructed_coordinates()) == len(positions)
    for position in positions:
        assert position in restored.obstructed_coordinates()
    assert restored.obstructed_connections() == [(positions[1], positions[2])]
    wire_id = restored.find_object((1, 0))
    assert wire_id is not None
    restored.remove(wire_id)
    assert restored.is_obstructed_coordinate((1, 0))
    restored.clear_obstructed_coordinates()
    assert not restored.is_obstructed_coordinate((1, 0))
    assert restored.is_obstructed_coordinate((2, 0))
