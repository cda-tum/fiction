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
    Writes a layout in DOT format into an output stream. Terminal names
    use quoted DOT strings.

    Args:
        lyt: Layout.
        os: Output stream.
        drawer: Formats the layout's tiles and topology.
        on_progress: Receives completed drawing work.

    Template Args:
        Lyt: Gate-level layout type.
        Drawer: DOT drawer type.
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
    Writes a finished layout in FGL version 2 with extent counts and
    declared interface order.

    This overload uses a file name to create and write into.

    Args:
        lyt: The layout to be written.
        on_progress: Receives completed serialization work.
        filename: The file name to create and write into. Should
                  preferably use the .fgl extension.

    Template Args:
        Lyt: Layout.

    Raises:
        std::invalid_argument: If the layout is incomplete, cyclic,
                               physically invalid, uses an unsupported
                               scheme, or contains illegal XML text
                               controls.
    """

class fgl_parsing_error(RuntimeError): ...

def read_cartesian_fgl_layout(filename: str, layout_name: str = "") -> mnt.pyfiction.layouts.cartesian_gate_layout:
    """
    Reads a gate-level layout from an FGL file provided as a file name.

    May throw an `fgl_parsing_error` if the FGL file is malformed.

    Args:
        filename: The file name to open and read from.
        name: The name to give to the generated layout.

    Template Args:
        Lyt: The layout type to be created from an input.
    """

def read_shifted_cartesian_fgl_layout(
    filename: str, layout_name: str = ""
) -> mnt.pyfiction.layouts.shifted_cartesian_gate_layout:
    """
    Reads a gate-level layout from an FGL file provided as a file name.

    May throw an `fgl_parsing_error` if the FGL file is malformed.

    Args:
        filename: The file name to open and read from.
        name: The name to give to the generated layout.

    Template Args:
        Lyt: The layout type to be created from an input.
    """

def read_hexagonal_fgl_layout(filename: str, layout_name: str = "") -> mnt.pyfiction.layouts.hexagonal_gate_layout:
    """
    Reads a gate-level layout from an FGL file provided as a file name.

    May throw an `fgl_parsing_error` if the FGL file is malformed.

    Args:
        filename: The file name to open and read from.
        name: The name to give to the generated layout.

    Template Args:
        Lyt: The layout type to be created from an input.
    """
