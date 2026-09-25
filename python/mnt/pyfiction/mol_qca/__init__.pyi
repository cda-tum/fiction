# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Molecular QCA cell-level layouts."""

import enum
from typing import overload

import mnt.pyfiction.layouts
import mnt.pyfiction.layouts.coords
from mnt.pyfiction.mol_qca import io as io

class mol_qca_cell_type(enum.Enum):
    """
    Types of molecular Quantum-dot Cellular Automata (molQCA) cells. The
    enumerators carry the symbols of their ASCII representation. A regular
    cell is clocked individually: `NORMAL1` to `NORMAL4` place it in clock
    phase 0 to 3, which is how the SCERPA simulator addresses clock
    regions and how SIM(7) gates arrange several clock regions in one
    tile.
    """

    EMPTY = 32
    """No cell."""

    NORMAL1 = 97
    """Regular cell in clock phase 0."""

    NORMAL2 = 98
    """Regular cell in clock phase 1."""

    NORMAL3 = 99
    """Regular cell in clock phase 2."""

    NORMAL4 = 100
    """Regular cell in clock phase 3."""

    INPUT = 105
    """Primary input cell."""

    OUTPUT = 111
    """Primary output cell."""

    CONST_0 = 48
    """Cell with a fixed polarization of logic 0."""

    CONST_1 = 49
    """Cell with a fixed polarization of logic 1."""

def mol_qca_clock_number(ct: mol_qca_cell_type) -> int:
    """
    The clock phase of a regular molQCA cell.

    Args:
        ct: Cell type.

    Returns:
        Clock phase 0 to 3 of `NORMAL1` to `NORMAL4`; 0 for every other
        type.
    """

class mol_qca_layout(mnt.pyfiction.layouts.cartesian_layout):
    """
    A molQCA layout: molecular QCA cells on a planar Cartesian grid. Each
    cell carries a type, which includes the clock phase of regular cells,
    and, for inputs and outputs, a name. The layout has no crossing layer,
    no cell modes, and no tile-based clocking, since molQCA crossings are
    coplanar and every cell names its own clock phase. The layout has
    value semantics; copies are independent.
    """

    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(
        self,
        dimension: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
        layout_name: str = "",
    ) -> None:
        """
        Creates an empty layout.

        Args:
            ar: Highest cell position; its z-coordinate is ignored because the
                layout is planar.
            name: Layout name.
        """

    def assign_cell_type(
        self,
        c: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int],
        ct: mol_qca_cell_type,
    ) -> None:
        """
        Assigns a cell type to a position. Assigning `EMPTY` removes the cell
        and its name.

        Args:
            c: Cell position.
            ct: Cell type.
        """

    def get_cell_type(
        self, c: mnt.pyfiction.layouts.coords.offset_coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> mol_qca_cell_type:
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

    def __copy__(self) -> mol_qca_layout:
        """Returns an independent copy of the layout."""

    def __deepcopy__(self, memo: dict) -> mol_qca_layout:
        """Returns an independent copy of the layout."""

    def __eq__(self, arg: mol_qca_layout, /) -> bool: ...
    def __ne__(self, arg: mol_qca_layout, /) -> bool: ...
