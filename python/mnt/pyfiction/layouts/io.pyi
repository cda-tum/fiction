# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Readers, writers, and drawers of gate-level layouts."""

from collections.abc import Callable
from typing import overload

import mnt.pyfiction.layouts

@overload
def write_dot_layout(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
    filename: str,
    clock_colors: bool = False,
    indexes: bool = False,
    on_progress: Callable[[str, int, int], None] | None = None,
) -> None: ...
@overload
def write_dot_layout(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    filename: str,
    clock_colors: bool = False,
    indexes: bool = False,
    on_progress: Callable[[str, int, int], None] | None = None,
) -> None: ...
@overload
def write_dot_layout(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout,
    filename: str,
    clock_colors: bool = False,
    indexes: bool = False,
    on_progress: Callable[[str, int, int], None] | None = None,
) -> None:
    """
    Writes a gate-level layout to a DOT file. Terminal names use quoted DOT strings.

    Args:
        layout: Gate-level layout to draw.
        filename: Output file path.
        clock_colors: Color tiles by clock number instead of gate type. Defaults to False.
        indexes: Label tiles with object indices. Defaults to False.
        on_progress: Receives completed drawing work. Defaults to None.

    Raises:
        RuntimeError: If the output file cannot be created or written.
    """

@overload
def write_fgl_layout(
    layout: mnt.pyfiction.layouts.cartesian_gate_layout,
    filename: str,
    on_progress: Callable[[str, int, int], None] | None = None,
) -> None: ...
@overload
def write_fgl_layout(
    layout: mnt.pyfiction.layouts.shifted_cartesian_gate_layout,
    filename: str,
    on_progress: Callable[[str, int, int], None] | None = None,
) -> None: ...
@overload
def write_fgl_layout(
    layout: mnt.pyfiction.layouts.hexagonal_gate_layout,
    filename: str,
    on_progress: Callable[[str, int, int], None] | None = None,
) -> None:
    """
    Writes a finished layout to an FGL version-2 file with extent counts and declared interface order.

    Args:
        layout: Layout to write.
        filename: Output file path, preferably with the .fgl extension.
        on_progress: Receives completed serialization work. Defaults to None.

    Raises:
        ValueError: If the layout is incomplete, cyclic, physically invalid, uses an unsupported scheme,
            or contains illegal XML text controls.
        RuntimeError: If the output file cannot be created, written, closed, or replaced.
    """

class fgl_parsing_error(RuntimeError): ...

@overload
def read_fgl_layout(
    filename: str, layout_name: str = "", *, layout_type: type[mnt.pyfiction.layouts.cartesian_gate_layout] = ...
) -> mnt.pyfiction.layouts.cartesian_gate_layout:
    """
    Reads a gate-level layout from an FGL file.

    Supports legacy maximum-index extents and version-2 extent counts, declared interface order, and manual obstructions.
    Version 2 requires a complete, acyclic, physically valid layout.

    Args:
        filename: Input file path.
        layout_name: Initial layout name, overridden by a name stored in the file. Defaults to an empty string.
        layout_type: Registered gate layout class. Defaults to cartesian_gate_layout.

    Returns:
        A new layout of the selected layout_type.

    Raises:
        fgl_parsing_error: If the FGL file is malformed or incompatible with the selected layout type.
        RuntimeError: If the file cannot be opened.
        TypeError: If layout_type is not a registered gate layout class.
    """

@overload
def read_fgl_layout(
    filename: str, layout_name: str = "", *, layout_type: type[mnt.pyfiction.layouts.shifted_cartesian_gate_layout]
) -> mnt.pyfiction.layouts.shifted_cartesian_gate_layout: ...
@overload
def read_fgl_layout(
    filename: str, layout_name: str = "", *, layout_type: type[mnt.pyfiction.layouts.hexagonal_gate_layout]
) -> mnt.pyfiction.layouts.hexagonal_gate_layout: ...
@overload
def read_fgl_layout(
    filename: str,
    layout_name: str = "",
    *,
    layout_type: type[
        mnt.pyfiction.layouts.cartesian_gate_layout
        | mnt.pyfiction.layouts.shifted_cartesian_gate_layout
        | mnt.pyfiction.layouts.hexagonal_gate_layout
    ],
) -> (
    mnt.pyfiction.layouts.cartesian_gate_layout
    | mnt.pyfiction.layouts.shifted_cartesian_gate_layout
    | mnt.pyfiction.layouts.hexagonal_gate_layout
): ...
