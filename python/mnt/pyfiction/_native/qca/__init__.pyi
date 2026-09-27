# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Quantum-dot Cellular Automata (QCA) cell-level layouts."""

import enum
from typing import overload

import mnt.pyfiction._native.layouts.coords
from mnt.pyfiction._native.qca import io as io

class QcaCellType(enum.Enum):
    """
    Types of Quantum-dot Cellular Automata (QCA) cells. The enumerators
    carry the symbols of their ASCII representation.
    """

    EMPTY = 32
    """No cell."""

    NORMAL = 120
    """Regular cell."""

    INPUT = 105
    """Primary input cell."""

    OUTPUT = 111
    """Primary output cell."""

    CONST_0 = 48
    """Cell with a fixed polarization of logic 0."""

    CONST_1 = 49
    """Cell with a fixed polarization of logic 1."""

class QcaCellMode(enum.Enum):
    """Modes of QCA cells as known from QCADesigner."""

    NORMAL = 0
    """Regular cell."""

    ROTATED = 1
    """Cell rotated by 45°."""

    VERTICAL = 2
    """Via cell that connects the ground and the crossing layer."""

    CROSSOVER = 3
    """Cell of a multilayer crossing."""

class QCALayout:
    """
    A QCA layout: QCA cells on a Cartesian grid with a ground layer (`z =
    0`) and a crossing layer (`z = 1`). Cells carry a type, a mode, and,
    for inputs and outputs, a name. Clock zones are tiles of cells, and
    each clock zone can be a synchronization element that extends its Hold
    phase. The layout has value semantics; copies are independent.
    """

    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(
        self, dimension: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> None: ...
    @overload
    def __init__(
        self,
        dimension: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        clocking_scheme: str = "2DDWave",
        layout_name: str = "",
        tile_size_x: int = 1,
        tile_size_y: int = 1,
    ) -> None:
        """
        Creates an empty layout of the given dimension, clocked by the predefined scheme of the given name, with clock zones of the given tile size. Raises ValueError for an unknown scheme or a zero tile size.
        """

    def coord(self, x: int, y: int, z: int = 0) -> mnt.pyfiction._native.layouts.coords.OffsetCoordinate:
        """
        Creates and returns a coordinate in the layout from the given x-, y-,
        and z-values.

        Args:
            x: x-value.
            y: y-value.
            z: z-value.

        Template Args:
            X: x-type.
            Y: y-type.
            Z: z-type.

        Returns:
            A coordinate in the layout of type `OffsetCoordinateType`.

        Note:
            This function is equivalent to calling `OffsetCoordinateType(x, y,
            z)`.
        """

    def x(self) -> int:
        """
        Returns the layout's x-dimension, i.e., returns the biggest x-value
        that still belongs to the layout.

        Returns:
            x-dimension.
        """

    def y(self) -> int:
        """
        Returns the layout's y-dimension, i.e., returns the biggest y-value
        that still belongs to the layout.

        Returns:
            y-dimension.
        """

    def z(self) -> int:
        """
        Returns the layout's z-dimension, i.e., returns the biggest z-value
        that still belongs to the layout.

        Returns:
            z-dimension.
        """

    def area(self) -> int:
        """
        Returns the layout's number of faces depending on the coordinate type.

        Returns:
            Area of layout.
        """

    def resize(
        self, dimension: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> None:
        """
        Updates the layout's dimensions, effectively resizing it.

        Args:
            ar: New aspect ratio.
        """

    def north(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> mnt.pyfiction._native.layouts.coords.OffsetCoordinate:
        """
        Returns the coordinate that is directly adjacent in northern direction
        of a given coordinate `c`, i.e., the face whose y-dimension is lower
        by 1. If `c`'s y-dimension is already at minimum, `c` is returned
        instead.

        Args:
            c: Coordinate whose northern counterpart is desired.

        Returns:
            Coordinate adjacent and north of `c`.
        """

    def north_east(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> mnt.pyfiction._native.layouts.coords.OffsetCoordinate:
        """
        Returns the coordinate that is located in north-eastern direction of a
        given coordinate `c`, i.e., the face whose x-dimension is higher by 1
        and whose y-dimension is lower by 1. If `c`'s x-dimension is already
        at maximum or `c`'s y-dimension is already at minimum, `c` is returned
        instead.

        Args:
            c: Coordinate whose north-eastern counterpart is desired.

        Returns:
            Coordinate directly north-eastern of `c`.
        """

    def east(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> mnt.pyfiction._native.layouts.coords.OffsetCoordinate:
        """
        Returns the coordinate that is directly adjacent in eastern direction
        of a given coordinate `c`, i.e., the face whose x-dimension is higher
        by 1. If `c`'s x-dimension is already at maximum, `c` is returned
        instead.

        Args:
            c: Coordinate whose eastern counterpart is desired.

        Returns:
            Coordinate adjacent and east of `c`.
        """

    def south_east(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> mnt.pyfiction._native.layouts.coords.OffsetCoordinate:
        """
        Returns the coordinate that is located in south-eastern direction of a
        given coordinate `c`, i.e., the face whose x-dimension and y-dimension
        are higher by 1. If `c`'s x-dimension or y-dimension are already at
        maximum, `c` is returned instead.

        Args:
            c: Coordinate whose south-eastern counterpart is desired.

        Returns:
            Coordinate directly south-eastern of `c`.
        """

    def south(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> mnt.pyfiction._native.layouts.coords.OffsetCoordinate:
        """
        Returns the coordinate that is directly adjacent in southern direction
        of a given coordinate `c`, i.e., the face whose y-dimension is higher
        by 1. If `c`'s y-dimension is already at maximum, `c` is returned
        instead.

        Args:
            c: Coordinate whose southern counterpart is desired.

        Returns:
            Coordinate adjacent and south of `c`.
        """

    def south_west(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> mnt.pyfiction._native.layouts.coords.OffsetCoordinate:
        """
        Returns the coordinate that is located in south-western direction of a
        given coordinate `c`, i.e., the face whose x-dimension is lower by 1
        and whose y-dimension is higher by 1. If `c`'s x-dimension is already
        at minimum or `c`'s y-dimension is already at maximum, `c` is returned
        instead.

        Args:
            c: Coordinate whose south-western counterpart is desired.

        Returns:
            Coordinate directly south-western of `c`.
        """

    def west(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> mnt.pyfiction._native.layouts.coords.OffsetCoordinate:
        """
        Returns the coordinate that is directly adjacent in western direction
        of a given coordinate `c`, i.e., the face whose x-dimension is lower
        by 1. If `c`'s x-dimension is already at minimum, `c` is returned
        instead.

        Args:
            c: Coordinate whose western counterpart is desired.

        Returns:
            Coordinate adjacent and west of `c`.
        """

    def north_west(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> mnt.pyfiction._native.layouts.coords.OffsetCoordinate:
        """
        Returns the coordinate that is located in north-western direction of a
        given coordinate `c`, i.e., the face whose x-dimension and y-dimension
        are lower by 1. If `c`'s x-dimension or y-dimension are already at
        minimum, `c` is returned instead.

        Args:
            c: Coordinate whose north-western counterpart is desired.

        Returns:
            Coordinate directly north-western of `c`.
        """

    def above(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> mnt.pyfiction._native.layouts.coords.OffsetCoordinate:
        """
        Returns the coordinate that is directly above a given coordinate `c`,
        i.e., the face whose z-dimension is higher by 1. If `c`'s z-dimension
        is already at maximum, `c` is returned instead.

        Args:
            c: Coordinate whose above counterpart is desired.

        Returns:
            Coordinate directly above `c`.
        """

    def below(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> mnt.pyfiction._native.layouts.coords.OffsetCoordinate:
        """
        Returns the coordinate that is directly below a given coordinate `c`,
        i.e., the face whose z-dimension is lower by 1. If `c`'s z-dimension
        is already at minimum, `c` is returned instead.

        Args:
            c: Coordinate whose below counterpart is desired.

        Returns:
            Coordinate directly below `c`.
        """

    def is_adjacent_of(
        self,
        c1: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is either directly north, east,
        south, or west of coordinate `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is either directly north, east, south, or west of
            `c1`.
        """

    def is_adjacent_elevation_of(
        self,
        c1: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Similar to `is_adjacent_of` but also considers `c1`'s elevation, i.e.,
        if `c2` is adjacent to `above(c1)` or `below(c1)`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is either directly north, east, south, or west of
            `c1` or `c1`'s elevations.
        """

    def is_ground_layer(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Returns whether the given coordinate is located in the ground layer
        where z is minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in ground layer.
        """

    def is_crossing_layer(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Returns whether the given coordinate is located in a crossing layer
        where z is not minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in a crossing layer.
        """

    def is_within_bounds(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Returns whether the given coordinate is located within the layout
        bounds.

        Args:
            c: Coordinate to check for boundary.

        Returns:
            `true` iff `c` is located within the layout bounds.
        """

    def coordinates(self) -> list[mnt.pyfiction._native.layouts.coords.OffsetCoordinate]:
        """
        Returns a range of all coordinates accessible in the layout between
        `start` and `stop`. If no values are provided, all coordinates in the
        layout will be included. The returned iterator range points to the
        first and last coordinate, respectively. The range object can be used
        within a for-each loop. Incrementing the iterator is equivalent to
        nested for loops in the order z, y, x. Consequently, the iteration
        will happen inside out, i.e., x will be iterated first, then y, then
        z.

        Args:
            start: First coordinate to include in the range of all
                   coordinates.
            stop: Last coordinate (exclusive) to include in the range of all
                  coordinates.

        Returns:
            An iterator range from `start` to `stop`. If they are not
            provided, the first/last coordinate is used as a default.
        """

    def ground_coordinates(self) -> list[mnt.pyfiction._native.layouts.coords.OffsetCoordinate]:
        """
        Returns a range of all coordinates accessible in the layout's ground
        layer between `start` and `stop`. The iteration order is the same as
        for the coordinates function but without the z dimension.

        Args:
            start: First coordinate to include in the range of all ground
                   coordinates.
            stop: Last coordinate (exclusive) to include in the range of all
                  ground coordinates.

        Returns:
            An iterator range from `start` to `stop`. If they are not
            provided, the first/last coordinate in the ground layer is used as
            a default.
        """

    def adjacent_coordinates(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[mnt.pyfiction._native.layouts.coords.OffsetCoordinate]:
        """
        Returns a container that contains all coordinates that are adjacent to
        a given one. Thereby, only cardinal directions are being considered,
        i.e., the container contains all coordinates `ac` for which
        `is_adjacent(c, ac)` returns `true`.

        Coordinates that are outside of the layout bounds are not considered.
        Thereby, the size of the returned container is at most 4.

        Args:
            c: Coordinate whose adjacent ones are desired.

        Returns:
            A container that contains all of `c`'s adjacent coordinates.
        """

    def adjacent_opposite_coordinates(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[
        tuple[
            mnt.pyfiction._native.layouts.coords.OffsetCoordinate, mnt.pyfiction._native.layouts.coords.OffsetCoordinate
        ]
    ]:
        """
        Returns a container that contains all coordinates pairs of opposing
        adjacent coordinates with respect to a given one. In this Cartesian
        layout, the container will contain (`north(c)`, `south(c)`) and
        (`east(c)`, `west(c)`).

        This function comes in handy when straight lines on the layout are to
        be examined.

        Coordinates outside of the layout bounds are not being considered.

        Args:
            c: Coordinate whose opposite ones are desired.

        Returns:
            A container that contains pairs of `c`'s opposing coordinates.
        """

    def get_cell_type(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> QcaCellType:
        """
        The cell type at a position.

        Args:
            c: Cell position.

        Returns:
            Cell type at `c`, `EMPTY` if no cell is there.
        """

    def is_empty_cell(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Whether no cell sits at a position.

        Args:
            c: Cell position.

        Returns:
            `true` iff `c` holds no cell.
        """

    def assign_cell_name(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int], n: str
    ) -> None:
        """
        Assigns a name to a cell. The empty string removes the name.

        Args:
            c: Cell position.
            n: Cell name.
        """

    def get_cell_name(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> str:
        """
        The name of a cell.

        Args:
            c: Cell position.

        Returns:
            Name of the cell at `c`, or the empty string if it has none.
        """

    @property
    def name(self) -> str:
        """The layout name."""

    @name.setter
    def name(self, arg: str, /) -> None: ...
    def num_cells(self) -> int:
        """
        Number of cells.

        Returns:
            Number of cells.
        """

    def is_empty(self) -> bool:
        """
        Whether the grid holds no cell.

        Returns:
            `true` iff there is no cell.
        """

    def num_pis(self) -> int:
        """
        Number of primary input cells.

        Returns:
            Number of input cells.
        """

    def num_pos(self) -> int:
        """
        Number of primary output cells.

        Returns:
            Number of output cells.
        """

    def is_pi(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Whether a cell is a primary input, i.e., of type `INPUT`.

        Args:
            c: Cell position.

        Returns:
            `true` iff `c` holds an input cell.
        """

    def is_po(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Whether a cell is a primary output, i.e., of type `OUTPUT`.

        Args:
            c: Cell position.

        Returns:
            `true` iff `c` holds an output cell.
        """

    def cells(self) -> list[mnt.pyfiction._native.layouts.coords.OffsetCoordinate]:
        """Returns the positions of all cells, in unspecified order."""

    def pis(self) -> list[mnt.pyfiction._native.layouts.coords.OffsetCoordinate]:
        """Returns the positions of all input cells, in unspecified order."""

    def pos(self) -> list[mnt.pyfiction._native.layouts.coords.OffsetCoordinate]:
        """Returns the positions of all output cells, in unspecified order."""

    def bounding_box_2d(
        self,
    ) -> tuple[
        mnt.pyfiction._native.layouts.coords.OffsetCoordinate, mnt.pyfiction._native.layouts.coords.OffsetCoordinate
    ]:
        """
        Returns the minimum and maximum corner of the bounding box.
        A 2D bounding box object computes a minimum-sized box around all
        non-empty coordinates in a given layout. Layouts can be of arbitrary
        size and, thus, may be larger than their contained elements.
        Sometimes, it might be necessary to know exactly which space the
        associated layout internals occupy. A bounding box computes
        coordinates that span a minimum-sized rectangle that encloses all non-
        empty layout coordinates.

        Returns:
            The minimum  and maximum enclosing coordinate in the associated layout.
        """

    def __copy__(self) -> QCALayout:
        """Returns an independent copy of the layout."""

    def __deepcopy__(self, memo: dict) -> QCALayout:
        """Returns an independent copy of the layout."""

    def __eq__(self, arg: QCALayout, /) -> bool: ...
    def __ne__(self, arg: QCALayout, /) -> bool: ...
    def get_tile_size_x(self) -> int:
        """
        Tile width in cells.

        Returns:
            Tile width.
        """

    def set_tile_size_x(self, size: int) -> None:
        """
        Sets the tile width.

        Args:
            tile_size_x: Tile width in cells.

        Raises:
            std::invalid_argument: if `tile_size_x` is zero.
        """

    def get_tile_size_y(self) -> int:
        """
        Tile height in cells.

        Returns:
            Tile height.
        """

    def set_tile_size_y(self, size: int) -> None:
        """
        Sets the tile height.

        Args:
            tile_size_y: Tile height in cells.

        Raises:
            std::invalid_argument: if `tile_size_y` is zero.
        """

    def get_clock_zone(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> mnt.pyfiction._native.layouts.coords.OffsetCoordinate:
        """
        The clock zone that contains a cell: its tile on layer 0.

        Args:
            c: Cell position.

        Returns:
            Clock zone of `c`.
        """

    def assign_clock_number(
        self,
        cz: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        cn: int,
    ) -> None:
        """
        Overrides the clock number of a clock zone.

        Args:
            cz: Clock zone.
            cn: Clock number.
        """

    def get_clock_number(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> int:
        """
        The clock number of the clock zone that contains a cell.

        Args:
            c: Cell position.

        Returns:
            Clock number of `get_clock_zone(c)`.
        """

    def num_clocks(self) -> int:
        """
        Number of clock phases.

        Returns:
            Number of clock phases.
        """

    def is_regularly_clocked(self) -> bool:
        """
        Whether a regular scheme clocks the layout with no overridden zones.

        Returns:
            `true` iff the clocking is regular.
        """

    def is_clocking_scheme(self, name: str) -> bool:
        """
        Compares the clocking scheme against a name. Predefined names are
        constants in `fiction::layouts::clocking`.

        Args:
            name: Clocking scheme name.

        Returns:
            `true` iff the scheme is called `name`.
        """

    def get_clocking_scheme_name(self) -> str:
        """
        Returns the name of the layout's clocking scheme, e.g., `2DDWave` or `USE`.
        """

    def replace_clocking_scheme(self, name: str) -> None:
        """
        Replaces the clocking scheme by the predefined scheme of the given name. Clock-number overrides are discarded. Raises ValueError for an unknown name.
        """

    def assign_cell_type(
        self,
        c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        ct: QcaCellType,
    ) -> None:
        """
        Assigns a cell type to a position. Assigning `EMPTY` removes the cell
        with its name and mode.

        Args:
            c: Cell position.
            ct: Cell type.
        """

    def assign_cell_mode(
        self,
        c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        m: QcaCellMode,
    ) -> None:
        """
        Assigns a mode to a cell. `cell_mode::NORMAL` removes a stored mode.

        Args:
            c: Cell position.
            m: Cell mode.
        """

    def get_cell_mode(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> QcaCellMode:
        """
        The mode of a cell.

        Args:
            c: Cell position.

        Returns:
            Mode of the cell at `c`, `cell_mode::NORMAL` if none is stored.
        """

    def assign_synchronization_element(
        self,
        cz: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        se: int,
    ) -> None:
        """
        Turns a clock zone into a synchronization element.

        Args:
            cz: Clock zone.
            se: Number of full clock cycles to extend the Hold phase of `cz`
                by; 0 turns `cz` back into a normal clock zone.
        """

    def is_synchronization_element(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Whether the clock zone that contains a cell is a synchronization
        element.

        Args:
            c: Cell position.

        Returns:
            `true` iff `get_clock_zone(c)` is a synchronization element.
        """

    def get_synchronization_element(
        self, c: mnt.pyfiction._native.layouts.coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> int:
        """
        The Hold-phase extension of the clock zone that contains a cell.

        Args:
            c: Cell position.

        Returns:
            Hold-phase extension of `get_clock_zone(c)` in full clock cycles.
        """

    def num_se(self) -> int:
        """
        Number of synchronization elements.

        Returns:
            Number of clock zones with a nonzero Hold-phase extension.
        """
