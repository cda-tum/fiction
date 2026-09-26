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

import mnt.pyfiction.layouts
import mnt.pyfiction.layouts.coords
from mnt.pyfiction.qca import io as io

class qca_cell_type(enum.Enum):
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

class qca_cell_mode(enum.Enum):
    """Modes of QCA cells as known from QCADesigner."""

    NORMAL = 0
    """Regular cell."""

    ROTATED = 1
    """Cell rotated by 45°."""

    VERTICAL = 2
    """Via cell that connects the ground and the crossing layer."""

    CROSSOVER = 3
    """Cell of a multilayer crossing."""

class qca_layout(mnt.pyfiction.layouts.cartesian_layout):
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
        self, dimension: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> None: ...
    @overload
    def __init__(
        self,
        dimension: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
        clocking_scheme: str = "2DDWave",
        layout_name: str = "",
        tile_size_x: int = 1,
        tile_size_y: int = 1,
    ) -> None:
        """
        Creates an empty layout of the given dimension, clocked by the predefined scheme of the given name, with clock zones of the given tile size. Raises ValueError for an unknown scheme or a zero tile size.
        """

    def get_cell_type(
        self, c: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> qca_cell_type:
        """
        The cell type at a position.

        Args:
            c: Cell position.

        Returns:
            Cell type at `c`, `EMPTY` if no cell is there.
        """

    def is_empty_cell(
        self, c: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Whether no cell sits at a position.

        Args:
            c: Cell position.

        Returns:
            `true` iff `c` holds no cell.
        """

    def assign_cell_name(
        self, c: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int], n: str
    ) -> None:
        """
        Assigns a name to a cell. The empty string removes the name.

        Args:
            c: Cell position.
            n: Cell name.
        """

    def get_cell_name(
        self, c: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> str:
        """
        The name of a cell.

        Args:
            c: Cell position.

        Returns:
            Name of the cell at `c`, or the empty string if it has none.
        """

    def set_layout_name(self, name: str) -> None:
        """
        Sets the layout name.

        Args:
            name: Layout name.
        """

    def get_layout_name(self) -> str:
        """
        The layout name.

        Returns:
            Layout name.
        """

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

    def is_pi(self, c: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Whether a cell is a primary input, i.e., of type `INPUT`.

        Args:
            c: Cell position.

        Returns:
            `true` iff `c` holds an input cell.
        """

    def is_po(self, c: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Whether a cell is a primary output, i.e., of type `OUTPUT`.

        Args:
            c: Cell position.

        Returns:
            `true` iff `c` holds an output cell.
        """

    def cells(self) -> list[mnt.pyfiction.layouts.coords.offset_coordinate]:
        """Returns the positions of all cells, in unspecified order."""

    def pis(self) -> list[mnt.pyfiction.layouts.coords.offset_coordinate]:
        """Returns the positions of all input cells, in unspecified order."""

    def pos(self) -> list[mnt.pyfiction.layouts.coords.offset_coordinate]:
        """Returns the positions of all output cells, in unspecified order."""

    def bounding_box_2d(
        self,
    ) -> tuple[mnt.pyfiction.layouts.coords.offset_coordinate, mnt.pyfiction.layouts.coords.offset_coordinate]:
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

    def __copy__(self) -> qca_layout:
        """Returns an independent copy of the layout."""

    def __deepcopy__(self, memo: dict) -> qca_layout:
        """Returns an independent copy of the layout."""

    def __eq__(self, arg: qca_layout, /) -> bool: ...
    def __ne__(self, arg: qca_layout, /) -> bool: ...
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
        self, c: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> mnt.pyfiction.layouts.coords.offset_coordinate:
        """
        The clock zone that contains a cell: its tile on layer 0.

        Args:
            c: Cell position.

        Returns:
            Clock zone of `c`.
        """

    def assign_clock_number(
        self, cz: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int], cn: int
    ) -> None:
        """
        Overrides the clock number of a clock zone.

        Args:
            cz: Clock zone.
            cn: Clock number.
        """

    def get_clock_number(
        self, c: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int]
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
        c: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
        ct: qca_cell_type,
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
        c: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
        m: qca_cell_mode,
    ) -> None:
        """
        Assigns a mode to a cell. `cell_mode::NORMAL` removes a stored mode.

        Args:
            c: Cell position.
            m: Cell mode.
        """

    def get_cell_mode(
        self, c: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> qca_cell_mode:
        """
        The mode of a cell.

        Args:
            c: Cell position.

        Returns:
            Mode of the cell at `c`, `cell_mode::NORMAL` if none is stored.
        """

    def assign_synchronization_element(
        self, cz: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int], se: int
    ) -> None:
        """
        Turns a clock zone into a synchronization element.

        Args:
            cz: Clock zone.
            se: Number of full clock cycles to extend the Hold phase of `cz`
                by; 0 turns `cz` back into a normal clock zone.
        """

    def is_synchronization_element(
        self, c: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int]
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
        self, c: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int]
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
