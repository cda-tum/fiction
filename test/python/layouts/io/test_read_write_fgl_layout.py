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

from mnt.pyfiction.layouts import cartesian_gate_layout, shifted_cartesian_gate_layout
from mnt.pyfiction.layouts.io import (
    fgl_parsing_error,
    read_cartesian_fgl_layout,
    read_hexagonal_fgl_layout,
    read_shifted_cartesian_fgl_layout,
    write_fgl_layout,
)
from mnt.pyfiction.networks import get_name, simulate_outputs
from mnt.pyfiction.physical_design import hexagonalization, orthogonal
from mnt.pyfiction.verification import eq_type, equivalence_checking

if TYPE_CHECKING:
    from pathlib import Path


def test_read_write(mux21, tmp_path):
    cart_layout = orthogonal(mux21)
    cart_file = str(tmp_path / "mux21_cartesian.fgl")
    write_fgl_layout(cart_layout, cart_file)
    assert equivalence_checking(read_cartesian_fgl_layout(cart_file), cart_layout) == eq_type.STRONG

    hex_layout = hexagonalization(cart_layout)
    hex_file = str(tmp_path / "mux21_hexagonal.fgl")
    write_fgl_layout(hex_layout, hex_file)
    assert equivalence_checking(read_hexagonal_fgl_layout(hex_file), hex_layout) == eq_type.STRONG

    shifted_layout = shifted_cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout")
    shifted_file = str(tmp_path / "empty_shifted_cartesian.fgl")
    write_fgl_layout(shifted_layout, shifted_file)
    assert equivalence_checking(read_shifted_cartesian_fgl_layout(shifted_file), shifted_layout) == eq_type.STRONG


@pytest.mark.parametrize("value", ["2", "-1", "18446744073709551616", "1garbage"])
def test_fgl_rejects_unrepresentable_layers(tmp_path: Path, value: str) -> None:
    layout = cartesian_gate_layout((1, 0), "2DDWave", "wire")
    source = layout.create_pi("a", (0, 0))
    layout.create_po(source, "f", (1, 0))
    path = tmp_path / "invalid.fgl"
    write_fgl_layout(layout, str(path))
    path.write_text(path.read_text(encoding="utf-8").replace("<z>0</z>", f"<z>{value}</z>", 1), encoding="utf-8")
    with pytest.raises(fgl_parsing_error, match=r"range|integer"):
        read_cartesian_fgl_layout(str(path))


def test_fgl_preserves_labels_and_synchronization(tmp_path: Path) -> None:
    layout = cartesian_gate_layout((2, 0), "2DDWave", "A & B < C")
    source = layout.create_pi("a&b", (0, 0))
    wire = layout.create_buf(source, (1, 0))
    layout.create_po(wire, "f<g", (2, 0))
    layout.assign_synchronization_element((1, 0), 2)
    path = tmp_path / "sync.fgl"
    write_fgl_layout(layout, str(path))
    restored = read_cartesian_fgl_layout(str(path))
    assert get_name(restored) == "A & B < C"
    assert restored.num_se() == 1
    assert restored.get_synchronization_element((1, 0)) == 2
    assert simulate_outputs(restored) == simulate_outputs(layout)
