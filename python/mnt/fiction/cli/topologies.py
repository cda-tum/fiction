# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The CLI topology names and their native layout bindings."""

from __future__ import annotations

from mnt.pyfiction import layouts

NATIVE_NAMES = {
    "cartesian": "cartesian",
    "odd_column_cartesian": "shifted_cartesian",
    "even_row_hex": "hexagonal",
    "odd_row_cartesian": "odd_row_cartesian",
    "even_row_cartesian": "even_row_cartesian",
    "even_column_cartesian": "even_column_cartesian",
    "odd_row_hex": "odd_row_hex",
    "odd_column_hex": "odd_column_hex",
    "even_column_hex": "even_column_hex",
}
"""Canonical topology names mapped to the prefixes of the native bindings."""

DISPLAY_NAMES = {
    "cartesian": "Cartesian",
    "odd_column_cartesian": "Odd-Column Cartesian",
    "even_column_cartesian": "Even-Column Cartesian",
    "odd_row_cartesian": "Odd-Row Cartesian",
    "even_row_cartesian": "Even-Row Cartesian",
    "odd_row_hex": "Odd-Row Hexagonal",
    "even_row_hex": "Even-Row Hexagonal",
    "odd_column_hex": "Odd-Column Hexagonal",
    "even_column_hex": "Even-Column Hexagonal",
}
"""Human-readable topology names; command options and JSON retain canonical names."""

_LAYOUT_TYPES = {
    "cartesian": layouts.CartesianGateLayout,
    "shifted_cartesian": layouts.ShiftedCartesianGateLayout,
    "hexagonal": layouts.HexagonalGateLayout,
    "odd_row_cartesian": layouts.OddRowCartesianGateLayout,
    "even_row_cartesian": layouts.EvenRowCartesianGateLayout,
    "even_column_cartesian": layouts.EvenColumnCartesianGateLayout,
    "odd_row_hex": layouts.OddRowHexGateLayout,
    "odd_column_hex": layouts.OddColumnHexGateLayout,
    "even_column_hex": layouts.EvenColumnHexGateLayout,
}

TOPOLOGIES = {_LAYOUT_TYPES[native]: name for name, native in NATIVE_NAMES.items()}
"""Gate-level layout classes mapped to canonical names for descriptions."""

NAMES = {**NATIVE_NAMES, "shifted_cartesian": "shifted_cartesian", "hexagonal": "hexagonal"}
"""Accepted topology names, including the two established aliases."""

FGL_READERS = {name: getattr(layouts.io, f"read_{native}_fgl_layout") for name, native in NAMES.items()}
"""The reader for each accepted FGL topology."""

GATE_LAYOUTS = {name: _LAYOUT_TYPES[native] for name, native in NAMES.items()}
"""Gate constructors used to validate a topology's clocking schemes."""
