# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The CLI topology names and their native layout bindings."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction import layouts
from mnt.pyfiction.layouts import arrangement

if TYPE_CHECKING:
    from collections.abc import Callable

    from mnt.pyfiction.layouts import cartesian_gate_layout, hexagonal_gate_layout, shifted_cartesian_gate_layout

    GateLayout = cartesian_gate_layout | shifted_cartesian_gate_layout | hexagonal_gate_layout

SPECS = {
    "cartesian": ("cartesian", None),
    "odd_column_cartesian": ("shifted_cartesian", arrangement.ODD_COLUMN),
    "even_column_cartesian": ("shifted_cartesian", arrangement.EVEN_COLUMN),
    "odd_row_cartesian": ("shifted_cartesian", arrangement.ODD_ROW),
    "even_row_cartesian": ("shifted_cartesian", arrangement.EVEN_ROW),
    "odd_row_hex": ("hexagonal", arrangement.ODD_ROW),
    "even_row_hex": ("hexagonal", arrangement.EVEN_ROW),
    "odd_column_hex": ("hexagonal", arrangement.ODD_COLUMN),
    "even_column_hex": ("hexagonal", arrangement.EVEN_COLUMN),
}
"""Canonical topology names mapped to the native layout family and its arrangement (None for Cartesian)."""

ALIASES = {"shifted_cartesian": "odd_column_cartesian", "hexagonal": "even_row_hex"}
"""The two established aliases of canonical topology names."""

NAMES = {**dict.fromkeys(SPECS), **ALIASES}
"""Accepted topology names, including the two established aliases."""

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


def canonical(topology: str) -> str:
    """Return the canonical name of an accepted topology name.

    Args:
        topology: A canonical name or an alias.

    Returns:
        The canonical name.
    """
    return ALIASES.get(topology, topology)


def topology_name(layout: GateLayout) -> str:
    """Return the canonical topology name of a gate-level layout.

    Args:
        layout: The layout.

    Returns:
        The canonical name, e.g., ``even_row_hex``.
    """
    if isinstance(layout, layouts.cartesian_gate_layout):
        return "cartesian"
    family = "cartesian" if isinstance(layout, layouts.shifted_cartesian_gate_layout) else "hex"
    return f"{layout.get_arrangement().name.lower()}_{family}"


def make_gate_layout(topology: str, dimension: tuple[int, int], scheme: str = "2DDWave") -> GateLayout:
    """Create an empty gate-level layout of a topology.

    The native constructor raises ``RuntimeError`` for an unknown clocking scheme.

    Args:
        topology: A canonical name or an alias.
        dimension: The highest tile position.
        scheme: The clocking scheme name.

    Returns:
        The layout.
    """
    family, shift = SPECS[canonical(topology)]
    cls = getattr(layouts, f"{family}_gate_layout")
    return cls(dimension, scheme) if shift is None else cls(shift, dimension, scheme)


def _fgl_reader(topology: str) -> Callable[[str, str], GateLayout]:
    """Return the FGL reader of a topology that rejects files of another arrangement.

    Args:
        topology: A canonical name or an alias.

    Returns:
        A function of file name and layout name.
    """
    name = canonical(topology)
    read = getattr(layouts.io, f"read_{SPECS[name][0]}_fgl_layout")

    def reader(filename: str, layout_name: str) -> GateLayout:
        layout = read(filename, layout_name)
        if topology_name(layout) != name:
            msg = f"Error parsing FGL file: the layout is not an {name} layout"
            raise layouts.io.fgl_parsing_error(msg)
        return layout

    return reader


FGL_READERS = {name: _fgl_reader(name) for name in NAMES}
"""The reader for each accepted FGL topology."""
