# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction import (
    eq_type,
    equivalence_checking,
    hexagonalization,
    orthogonal,
    read_cartesian_fgl_layout,
    read_hexagonal_fgl_layout,
    read_shifted_cartesian_fgl_layout,
    shifted_cartesian_gate_layout,
    write_fgl_layout,
)


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
