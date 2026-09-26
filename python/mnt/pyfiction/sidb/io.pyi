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

class write_sidb_layout_svg_params:
    """Parameters for writing SiDB layouts to SVG format."""

    def __init__(self) -> None:
        """Default constructor."""

    @property
    def on_progress(self, /) -> Callable[[str, int, int], None] | None:
        """Receives serialization progress."""

    @on_progress.setter
    def on_progress(self, value: Callable[[str, int, int], None] | None) -> None: ...
    @property
    def lattice_point_size(self) -> float:
        """Size of the H-Si lattice points in SVG units."""

    @lattice_point_size.setter
    def lattice_point_size(self, arg: float, /) -> None: ...
    @property
    def sidb_size(self) -> float:
        """Size of the SiDB in SVG units."""

    @sidb_size.setter
    def sidb_size(self, arg: float, /) -> None: ...
    @property
    def sidb_border_width(self) -> float:
        """Border width of the SiDB."""

    @sidb_border_width.setter
    def sidb_border_width(self, arg: float, /) -> None: ...
    @property
    def color_background(self) -> color_mode:
        """The color mode of the background for the SVG output."""

    @color_background.setter
    def color_background(self, arg: color_mode, /) -> None: ...
    @property
    def lattice_mode(self) -> sidb_lattice_mode:
        """The lattice mode of the SiDB layout."""

    @lattice_mode.setter
    def lattice_mode(self, arg: sidb_lattice_mode, /) -> None: ...

@overload
def write_sidb_layout_svg(
    layout: mnt.pyfiction.sidb.sidb_layout, filename: str, ps: write_sidb_layout_svg_params = ...
) -> None:
    """
    Writes an `sidb::layout` as an SVG image. See the stream overload for
    the image's content.

    Args:
        lyt: Layout to draw.
        filename: File to write into.
        ps: Drawing parameters.

    Raises:
        std::ofstream::failure: if the file cannot be opened.
        std::out_of_range: if padding exceeds the lattice-site coordinate
                           range.
    """

@overload
def write_sidb_layout_svg(
    layout: mnt.pyfiction.sidb.sidb_layout,
    charge_distribution: mnt.pyfiction.sidb.charge_distribution,
    filename: str,
    ps: write_sidb_layout_svg_params = ...,
) -> None:
    """
    Writes an `sidb::layout` with a charge distribution as an SVG image.
    See the stream overload for the image's content.

    Args:
        lyt: Layout to draw.
        cd: Charge distribution over the layout's SiDBs.
        filename: File to write into.
        ps: Drawing parameters.

    Raises:
        std::invalid_argument: if the charge distribution sites differ
                               from the layout.
        std::ofstream::failure: if the file cannot be opened.
    """

@overload
def write_sidb_layout_svg_to_string(
    layout: mnt.pyfiction.sidb.sidb_layout, ps: write_sidb_layout_svg_params = ...
) -> str:
    """
    Writes an `sidb::layout` as an SVG image to a stream: the lattice
    points of the layout's bounding box, if the parameters ask for them,
    and every SiDB at its position on the layout's lattice. Surface
    defects are not drawn.

    Args:
        lyt: Layout to draw.
        os: Output stream to write into.
        ps: Drawing parameters.

    Raises:
        std::out_of_range: if padding exceeds the lattice-site coordinate
                           range.
    """

@overload
def write_sidb_layout_svg_to_string(
    layout: mnt.pyfiction.sidb.sidb_layout,
    charge_distribution: mnt.pyfiction.sidb.charge_distribution,
    ps: write_sidb_layout_svg_params = ...,
) -> str:
    """
    Writes an `sidb::layout` with a charge distribution as an SVG image to
    a stream: as the layout overload, with every SiDB colored by its
    charge state.

    Args:
        lyt: Layout to draw.
        cd: Charge distribution over the layout's SiDBs.
        os: Output stream to write into.
        ps: Drawing parameters.

    Raises:
        std::invalid_argument: if the charge distribution sites differ
                               from the layout.
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
