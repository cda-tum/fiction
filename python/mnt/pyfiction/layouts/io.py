# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Read, write, and draw gate layouts using filesystem paths."""

from __future__ import annotations

from os import PathLike, fspath
from typing import TYPE_CHECKING, overload

from mnt.pyfiction._native.layouts import io as _native
from mnt.pyfiction._native.layouts.io import FGLParsingError
from mnt.pyfiction.layouts import (
    CartesianGateLayout,
    EvenColumnCartesianGateLayout,
    EvenColumnHexGateLayout,
    EvenRowCartesianGateLayout,
    HexagonalGateLayout,
    OddColumnHexGateLayout,
    OddRowCartesianGateLayout,
    OddRowHexGateLayout,
    ShiftedCartesianGateLayout,
)

if TYPE_CHECKING:
    from collections.abc import Callable

    from mnt.pyfiction.layouts._types import GateLayout, GateLayoutT

_READERS: dict[type[object], Callable[[str, str], GateLayout]] = {
    CartesianGateLayout: _native.read_cartesian_fgl_layout,
    ShiftedCartesianGateLayout: _native.read_shifted_cartesian_fgl_layout,
    HexagonalGateLayout: _native.read_hexagonal_fgl_layout,
    OddRowCartesianGateLayout: _native.read_odd_row_cartesian_fgl_layout,
    EvenRowCartesianGateLayout: _native.read_even_row_cartesian_fgl_layout,
    EvenColumnCartesianGateLayout: _native.read_even_column_cartesian_fgl_layout,
    OddRowHexGateLayout: _native.read_odd_row_hex_fgl_layout,
    OddColumnHexGateLayout: _native.read_odd_column_hex_fgl_layout,
    EvenColumnHexGateLayout: _native.read_even_column_hex_fgl_layout,
}


@overload
def read_fgl_layout(path: str | PathLike[str], *, name: str = "") -> CartesianGateLayout: ...


@overload
def read_fgl_layout(path: str | PathLike[str], *, layout_type: type[GateLayoutT], name: str = "") -> GateLayoutT: ...


def read_fgl_layout(
    path: str | PathLike[str], *, layout_type: type[object] = CartesianGateLayout, name: str = ""
) -> GateLayout:
    """Read an FGL file into the selected concrete topology.

    Malformed contents or a topology mismatch raise FGLParsingError.

    Args:
        path: FGL input path.
        layout_type: Concrete gate layout matching the file's topology.
        name: Optional layout name override.

    Returns:
        The parsed layout with the requested concrete type.

    Raises:
        ValueError: The selected layout type is unsupported.
    """
    if layout_type not in _READERS:
        msg = f"read_fgl does not support {layout_type.__name__}"
        raise ValueError(msg)
    return _READERS[layout_type](fspath(path), name)


def write_fgl_layout(
    layout: GateLayout, path: str | PathLike[str], *, on_progress: Callable[[str, int, int], None] | None = None
) -> None:
    """Write a gate layout to an FGL file.

    Args:
        layout: Gate layout to serialize.
        path: Output path.
        on_progress: Optional progress callback; exceptions propagate to the caller.
    """
    _native.write_fgl_layout(layout, fspath(path), on_progress)


def write_dot_layout(
    layout: GateLayout,
    path: str | PathLike[str],
    *,
    clock_colors: bool = False,
    indexes: bool = False,
    on_progress: Callable[[str, int, int], None] | None = None,
) -> None:
    """Draw a gate layout as Graphviz DOT.

    Args:
        layout: Gate layout to draw.
        path: Output path.
        clock_colors: Color tiles by clock phase.
        indexes: Label nodes with their identifiers.
        on_progress: Optional progress callback; exceptions propagate to the caller.
    """
    _native.write_dot_layout(layout, fspath(path), clock_colors, indexes, on_progress)


__all__ = ["FGLParsingError", "read_fgl_layout", "write_dot_layout", "write_fgl_layout"]
