# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Readers and writers of SiDB layouts."""

import enum
from collections.abc import Callable
from typing import overload

import mnt.pyfiction.sidb

def write_sqd_layout(
    layout: mnt.pyfiction.sidb.sidb_layout, filename: str, on_progress: Callable[[str, int, int], None] | None = None
) -> None:
    """
    Writes an `sidb::layout` as an SQD file. See the stream overload for
    the file's content.

    Args:
        lyt: Layout to write.
        filename: File to write into.
        on_progress: Receives completed dot and defect records.

    Raises:
        std::ofstream::failure: if the file cannot be opened.
    """

class sqd_parsing_error(RuntimeError): ...

def read_sqd_layout(filename: str, layout_name: str = "") -> mnt.pyfiction.sidb.sidb_layout:
    """
    Reads an SQD file into an `sidb::layout`. See the stream overload for
    the file's interpretation.

    Args:
        filename: The file to read.
        name: The name to give to the layout.

    Returns:
        The layout read from the file.

    Raises:
        sqd_parsing_error: if the file is malformed or a name-only lattice
                           is unknown.
        std::ifstream::failure: if the file cannot be opened.
    """

class unsupported_defect_index_exception(ValueError): ...
class missing_position_exception(ValueError): ...

def read_surface_defects(filename: str, layout_name: str = "") -> mnt.pyfiction.sidb.sidb_layout:
    """
    Reads a defect matrix from a file into a defects-only `sidb::layout`.
    See the stream overload for the format.

    Args:
        filename: The file to read.
        name: The name to give to the layout.

    Returns:
        The layout holding the defects.

    Raises:
        std::ifstream::failure: if the file cannot be opened.
    """

class color_mode(enum.Enum):
    """Enumeration to specify the color mode for the SVG output."""

    LIGHT = 1
    """Light mode."""

    DARK = 0
    """Dark mode."""

class sidb_lattice_mode(enum.Enum):
    """
    Enumeration to specify if the H-Si lattice is plotted in addition to
    SiDBs.
    """

    SHOW_LATTICE = 1
    """Lattice is shown."""

    HIDE_LATTICE = 0
    """Lattice is hidden. Only SiDBs are shown."""

@overload
def write_sidb_layout_svg(
    layout: mnt.pyfiction.sidb.sidb_layout,
    filename: str,
    *,
    on_progress: Callable[[str, int, int], None] | None = None,
    lattice_point_size: float = 0.3,
    sidb_size: float = 0.9,
    sidb_border_width: float = 0.3,
    color_background: color_mode = ...,
    lattice_mode: sidb_lattice_mode = ...,
) -> None: ...
@overload
def write_sidb_layout_svg(
    layout: mnt.pyfiction.sidb.sidb_layout,
    charge_distribution: mnt.pyfiction.sidb.charge_distribution,
    filename: str,
    *,
    on_progress: Callable[[str, int, int], None] | None = None,
    lattice_point_size: float = 0.3,
    sidb_size: float = 0.9,
    sidb_border_width: float = 0.3,
    color_background: color_mode = ...,
    lattice_mode: sidb_lattice_mode = ...,
) -> None:
    """
    Write an SVG representation of the SiDB layout to the supplied filename.
    The keyword options select dot sizes, background coloring, lattice rendering, and progress reporting.

    Args:
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        lattice_point_size: Lattice-point size in SVG units.
        sidb_size: Size of the SiDB in SVG units.
        sidb_border_width: SiDB border width in SVG units.
        color_background: The color mode of the background for the SVG output.
        lattice_mode: The lattice mode of the SiDB layout.
    """

@overload
def write_sidb_layout_svg_to_string(
    layout: mnt.pyfiction.sidb.sidb_layout,
    *,
    on_progress: Callable[[str, int, int], None] | None = None,
    lattice_point_size: float = 0.3,
    sidb_size: float = 0.9,
    sidb_border_width: float = 0.3,
    color_background: color_mode = ...,
    lattice_mode: sidb_lattice_mode = ...,
) -> str: ...
@overload
def write_sidb_layout_svg_to_string(
    layout: mnt.pyfiction.sidb.sidb_layout,
    charge_distribution: mnt.pyfiction.sidb.charge_distribution,
    *,
    on_progress: Callable[[str, int, int], None] | None = None,
    lattice_point_size: float = 0.3,
    sidb_size: float = 0.9,
    sidb_border_width: float = 0.3,
    color_background: color_mode = ...,
    lattice_mode: sidb_lattice_mode = ...,
) -> str:
    """
    Return an SVG representation of the SiDB layout.
    The keyword options select dot sizes, background coloring, lattice rendering, and progress reporting.

    Args:
        on_progress: Callback receiving (task, completed, total); None disables progress reporting.
        lattice_point_size: Lattice-point size in SVG units.
        sidb_size: Size of the SiDB in SVG units.
        sidb_border_width: SiDB border width in SVG units.
        color_background: The color mode of the background for the SVG output.
        lattice_mode: The lattice mode of the SiDB layout.
    """

def print_sidb_layout(
    layout: mnt.pyfiction.sidb.sidb_layout,
    charge_distribution: mnt.pyfiction.sidb.charge_distribution | None = None,
    lat_color: bool = True,
    crop_layout: bool = False,
    draw_lattice: bool = True,
) -> str:
    """
    Prints an `sidb::layout` as a lattice picture: one symbol per lattice
    site of the bounding box, row by row, with a blank line between the
    dimer rows of an H-Si(100)-2x1 surface and an indented second row per
    unit cell of an H-Si(111)-1x1 surface. Inputs, outputs, normal SiDBs,
    defects, and empty sites differ by color and symbol. With a charge
    distribution, a charged SiDB is a filled circle colored by its sign
    and a neutral one a hollow circle.

    Args:
        os: Output stream to print into.
        lyt: Layout to print.
        cd: Charge distribution over the layout's SiDBs, or `nullptr` to
            print dot tags only.
        lat_color: Whether to color the picture.
        crop_layout: Whether to pad the bounding box by two columns and
                     one unit cell instead of printing it tight.
        draw_lattice: Whether to print empty lattice sites as dots.

    Raises:
        std::invalid_argument: if the charge distribution sites differ
                               from the layout.
        std::out_of_range: if padding exceeds the lattice-site coordinate
                           range.
    """
