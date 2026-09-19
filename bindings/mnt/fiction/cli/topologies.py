# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The CLI topology names and their native layout bindings."""

from __future__ import annotations

from mnt import pyfiction

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

TOPOLOGIES = {getattr(pyfiction, f"{native}_gate_layout"): name for name, native in NATIVE_NAMES.items()}
"""Gate-level layout classes mapped to canonical names for descriptions."""

NAMES = {**NATIVE_NAMES, "shifted_cartesian": "shifted_cartesian", "hexagonal": "hexagonal"}
"""Accepted topology names, including the two established aliases."""

FGL_READERS = {name: getattr(pyfiction, f"read_{native}_fgl_layout") for name, native in NAMES.items()}
"""The reader for each accepted FGL topology."""

GATE_LAYOUTS = {name: getattr(pyfiction, f"{native}_gate_layout") for name, native in NAMES.items()}
"""Gate constructors used to validate a topology's clocking schemes."""
