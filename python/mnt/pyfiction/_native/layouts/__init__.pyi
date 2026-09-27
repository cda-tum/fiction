# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Coordinates, layout topologies, clocking, and gate-level layouts."""

from collections.abc import Sequence
from typing import overload

import mnt.pyfiction._native.inml
import mnt.pyfiction._native.mol_qca
import mnt.pyfiction._native.qca
from mnt.pyfiction._native.layouts import coords as coords
from mnt.pyfiction._native.layouts import io as io

class CartesianGateLayout:
    """
    A gate layout with geometry, clocking, gates, wires, and obstructions. Gate creation takes and returns coordinates. Node inspection uses identifiers from get_node().
    """

    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(self, dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Standard constructor. Creates a named gate-level layout of the given
        aspect ratio. To this end, it calls `CoordinateLayout`'s standard
        constructor.

        Args:
            ar: Highest possible position in the layout.
            name: Layout name.
        """

    @overload
    def __init__(
        self,
        dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        clocking_scheme: str = "2DDWave",
        layout_name: str = "",
    ) -> None:
        """
        Standard constructor. Creates a gate-level layout of the given aspect
        ratio and clocks it via the given clocking scheme. To this end, it
        calls `CoordinateLayout`'s standard constructor.

        Args:
            ar: Highest possible position in the layout.
            scheme: Clocking scheme to apply to this layout.
            name: Layout name.
        """

    def coord(self, x: int, y: int, z: int = 0) -> coords.OffsetCoordinate:
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

    def resize(self, dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Updates the layout's dimensions, effectively resizing it.

        Args:
            ar: New aspect ratio.
        """

    def north(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def east(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def south(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def west(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def above(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
        """
        Returns the coordinate that is directly above a given coordinate `c`,
        i.e., the face whose z-dimension is higher by 1. If `c`'s z-dimension
        is already at maximum, `c` is returned instead.

        Args:
            c: Coordinate whose above counterpart is desired.

        Returns:
            Coordinate directly above `c`.
        """

    def below(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        c1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
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
        c1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
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

    def is_ground_layer(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in the ground layer
        where z is minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in ground layer.
        """

    def is_crossing_layer(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in a crossing layer
        where z is not minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in a crossing layer.
        """

    def is_within_bounds(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located within the layout
        bounds.

        Args:
            c: Coordinate to check for boundary.

        Returns:
            `true` iff `c` is located within the layout bounds.
        """

    def coordinates(self) -> list[coords.OffsetCoordinate]:
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

    def ground_coordinates(self) -> list[coords.OffsetCoordinate]:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[tuple[coords.OffsetCoordinate, coords.OffsetCoordinate]]:
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

    def assign_clock_number(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int], cn: int
    ) -> None:
        """
        Overrides the clock number of a tile in the stored scheme. The clock
        number applies to every layer of the tile, so the z-coordinate of `cz`
        is ignored.

        Args:
            cz: Clock zone to override.
            cn: New clock number for `cz`.
        """

    def get_clock_number(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the clock number of a tile. Every layer of a tile has the same
        clock number, so the z-coordinate of `cz` is ignored.

        Args:
            cz: Clock zone.

        Returns:
            Clock number of `cz`.
        """

    def num_clocks(self) -> int:
        """
        Returns the number of clock phases in the layout. Each clock cycle is
        divided into n phases. In QCA, the number of phases is usually 4. In
        iNML it is 3. Clocking schemes support 3 or 4 phases.

        Returns:
            The number of different clock signals in the layout.
        """

    def is_regularly_clocked(self) -> bool:
        """
        Returns whether the layout is clocked by a regular clocking scheme
        with no overwritten zones.

        Returns:
            `true` iff the layout is clocked by a regular scheme and no zones
            have been overwritten.
        """

    def is_clocking_scheme(self, name: str) -> bool:
        """
        Compares the stored clocking scheme against the provided name.
        Predefined names are constants in `fiction::layouts::clocking`.

        Args:
            name: Clocking scheme name.

        Returns:
            `true` iff the layout is clocked by a clocking scheme of name
            `name`.
        """

    def get_clocking_scheme_name(self) -> str:
        """
        Returns the name of the layout's clocking scheme, e.g., `2DDWave` or `USE`.
        """

    def is_incoming_clocked(
        self,
        cz1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Evaluates whether clock zone `cz2` feeds information to clock zone
        `cz1`, i.e., whether `cz2` is clocked with a clock number that is
        lower by 1 modulo `num_clocks()`, or either zone is a synchronization
        element.

        Args:
            cz1: Base clock zone.
            cz2: Clock zone to check whether its clock number is lower by 1.

        Returns:
            `true` iff `cz2` can feed information to `cz1`.
        """

    def is_outgoing_clocked(
        self,
        cz1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Evaluates whether clock zone `cz2` accepts information from clock zone
        `cz1`, i.e., whether `cz2` is clocked with a clock number that is
        higher by 1 modulo `num_clocks()`, or either zone is a synchronization
        element.

        Args:
            cz1: Base clock zone.
            cz2: Clock zone to check whether its clock number is higher by 1.

        Returns:
            `true` iff `cz2` can accept information from `cz1`.
        """

    def incoming_clocked_zones(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
        """
        Returns a container with all clock zones that are incoming to the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are incoming to `cz`.
        """

    def outgoing_clocked_zones(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
        """
        Returns a container with all clock zones that are outgoing from the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are outgoing from `cz`.
        """

    def in_degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of incoming clock zones to the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s incoming clock zones.
        """

    def out_degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of outgoing clock zones from the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s outgoing clock zones.
        """

    def degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of distinct incoming or outgoing neighboring clock
        zones.

        Args:
            cz: Base clock zone.

        Returns:
            Number of distinct clocked neighbors of `cz`.
        """

    def replace_clocking_scheme(self, name: str) -> None:
        """
        Replaces the clocking scheme by the predefined scheme of the given name. Clock-number overrides are discarded; synchronization elements are kept. Raises ValueError for an unknown name.
        """

    def obstruct_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Marks the given coordinate as obstructed.

        Args:
            c: clock_zone to obstruct.
        """

    def obstruct_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Marks the connection from coordinate `src` to coordinate `tgt` as
        obstructed.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Note:
            clock_zones marked this way will not be crossed with wires by path
            finding algorithms.
        """

    def clear_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Clears the obstruction status of the given coordinate `c` if the
        obstruction was manually marked via `obstruct_coordinate`.

        Args:
            c: clock_zone to clear.
        """

    def clear_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Clears the obstruction status of the connection from coordinate `src`
        to coordinate `tgt` if the obstruction was manually marked via
        `obstruct_connection`.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.
        """

    def clear_obstructed_coordinates(self) -> None:
        """
        Clears all obstructed coordinates that were manually marked via
        `obstruct_coordinate`.
        """

    def clear_obstructed_connections(self) -> None:
        """
        Clears all obstructed connections that were manually marked via
        `obstruct_connection`.
        """

    def is_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks if the given coordinate is obstructed of some sort.

        Args:
            c: Coordinate to check.

        Returns:
            `true` iff `c` is obstructed.
        """

    def is_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks if the given coordinate-coordinate connection is obstructed of
        some sort.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Returns:
            `true` iff the connection from `src` to `tgt` is obstructed.
        """

    def create_pi(
        self, name: str = "", t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...
    ) -> coords.OffsetCoordinate: ...
    def create_po(
        self,
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        name: str = "",
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def is_pi(self, n: int) -> bool:
        """
        Check whether `n` is a primary input.

        Args:
            n: Node to be checked.

        Returns:
            `true` iff `n` is a PI.
        """

    def is_po(self, n: int) -> bool:
        """
        Check whether `n` is a primary output.

        Args:
            n: Node to be checked.

        Returns:
            `true` iff `n` is a PO.
        """

    def is_pi_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary input.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PI.
        """

    def is_po_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary output.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PO.
        """

    def is_inv(self, arg: int, /) -> bool:
        """
        Returns whether `n` computes the binary inversion (NOT gate).

        Args:
            n: Node to check.

        Returns:
            `true` iff `n` is a NOT gate.
        """

    def is_and(self, arg: int, /) -> bool: ...
    def is_nand(self, arg: int, /) -> bool: ...
    def is_or(self, arg: int, /) -> bool: ...
    def is_nor(self, arg: int, /) -> bool: ...
    def is_xor(self, arg: int, /) -> bool: ...
    def is_xnor(self, arg: int, /) -> bool: ...
    def is_lt(self, arg: int, /) -> bool: ...
    def is_le(self, arg: int, /) -> bool: ...
    def is_gt(self, arg: int, /) -> bool: ...
    def is_ge(self, arg: int, /) -> bool: ...
    def is_maj(self, arg: int, /) -> bool: ...
    def is_fanout(self, arg: int, /) -> bool:
        """
        Returns whether `n` is a wire and has multiple outputs, thereby,
        acting as a fanout gate. Note that a fanout will return `true` for
        both `is_wire` and `is_fanout`.

        Args:
            n: Node to check.

        Returns:
            `true` iff `n` is a fanout gate.
        """

    def is_wire(self, arg: int, /) -> bool:
        """Equivalent to `is_buf`."""

    def set_layout_name(self, name: str) -> None: ...
    @property
    def name(self) -> str:
        """The layout name."""

    @name.setter
    def name(self, arg: str, /) -> None: ...
    def get_layout_name(self) -> str: ...
    def clone(self) -> CartesianGateLayout:
        """
        Clones the layout returning a deep copy.

        Returns:
            Deep copy of the layout.
        """

    def set_input_name(self, index: int, name: str) -> None: ...
    def get_input_name(self, index: int) -> str: ...
    def set_output_name(self, index: int, name: str) -> None: ...
    def get_output_name(self, index: int) -> str: ...
    def get_name(self, s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> str: ...
    def create_buf(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_not(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_and(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_nand(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_or(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_nor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_xor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_xnor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_lt(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_le(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_gt(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_ge(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_maj(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def num_pis(self) -> int: ...
    def num_pos(self) -> int: ...
    def num_gates(self) -> int:
        """
        Returns the number of placed nodes in the layout that do not compute
        the identity function.

        Returns:
            Number of gates in the layout.
        """

    def num_wires(self) -> int:
        """
        Returns the number of placed nodes in the layout that compute the
        identity function including PIs and POs.

        Returns:
            Number of wires in the layout.
        """

    def num_crossings(self) -> int:
        """
        Returns the number of placed nodes in the layout that compute the
        identity function and cross other nodes.

        Returns:
            Number of crossings in the layout.
        """

    def is_empty(self) -> bool:
        """
        Checks whether there are no gates or wires assigned to the layout's
        coordinates.

        Returns:
            `true` iff the layout is empty.
        """

    def fanin_size(self, n: int) -> int:
        """
        Returns the number of incoming, adjacently placed, and properly
        clocked signals to the given node.

        Args:
            n: Node to check.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            Number of fanins to `n`.
        """

    def fanout_size(self, n: int) -> int:
        """
        Returns the number of outgoing, adjacently placed, and properly
        clocked signals of the given node.

        Args:
            n: Node to check.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            Number of fanouts to `n`.
        """

    def get_node(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Fetches the node that is placed onto a tile pointed to by a given
        signal. If no node is placed there, the `const0` node is returned.

        Args:
            s: Pointer to a tile.

        Returns:
            Node at position `t` where `s` points at `t`; or 0 if no node is
            placed at `t`.
        """

    def get_tile(self, n: int) -> coords.OffsetCoordinate:
        """
        The inverse function of `get_node`. Fetches the tile that the provided
        node is placed on. Returns a default dead tile if the node is not
        placed.

        Args:
            n: Node whose location is desired.

        Returns:
            Tile at which `n` is placed or a default dead tile if `n` is not
            placed.
        """

    def make_signal(self, n: int) -> coords.OffsetCoordinate:
        """
        Invokes the same behavior as `get_tile(n)` but additionally casts the
        return value to a signal. That is, this function returns the signal
        representation of the tile that the node `n` is assigned to.

        Args:
            n: Node whose signal is desired.

        Returns:
            Signal that points to `n`.
        """

    def move_node(
        self,
        n: int,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        new_children: Sequence[coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]] = [],
    ) -> coords.OffsetCoordinate:
        """
        Moves a given node to a new position and also updates its children,
        i.e., incoming signals.

        Args:
            n: Node to move.
            t: Tile to move `n` to.
            new_children: New incoming signals to `n`.

        Returns:
            Signal pointing to `n`'s new tile.
        """

    def clear_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Removes all assigned nodes from the given tile and marks them as dead.

        Args:
            t: Tile whose nodes are to be removed.

        Note:
            This function does not reduce the number of nodes in the layout
            nor does it reduce the number of PIs that are being returned via
            `num_pis()` even if the tile to clear is an input tile. However,
            the number of POs is reduced if the tile to clear is an output
            tile. While this seems counter-intuitive and inconsistent, it is
            in line with mockturtle's understanding of nodes and primary
            outputs.
        """

    def is_gate_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_gate` (in
        accordance with `mockturtle`'s definition of gates).

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that is a neither a constant nor a PI.
        """

    def is_wire_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_wire`.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that computes the identity.
        """

    def is_empty_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether `t` does not have a node assigned to it.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` is an empty tile.
        """

    def pis(self) -> list[coords.OffsetCoordinate]: ...
    def pos(self) -> list[coords.OffsetCoordinate]: ...
    def gates(self) -> list[coords.OffsetCoordinate]: ...
    def wires(self) -> list[coords.OffsetCoordinate]: ...
    def fanins(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]: ...
    def fanouts(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]: ...
    def is_incoming_signal(
        self,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks whether signal `s` is incoming to tile `t`. That is, whether
        tile `t` hosts a node that has a fanin assigned to the tile that
        signal `s` points to.

        Args:
            t: Base tile.
            s: Signal pointing to a potential incoming tile to `t`.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `s` is incoming to `t`.
        """

    def has_no_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has no incoming tiles.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `t` does not have incoming tiles.
        """

    def has_northern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in northern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north(t)` is incoming to `t`.
        """

    def has_north_eastern_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in north-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north_east(t)` is incoming to `t`.
        """

    def has_eastern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `east(t)` is incoming to `t`.
        """

    def has_south_eastern_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in south-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south_east(t)` is incoming to `t`.
        """

    def has_southern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in southern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south(t)` is incoming to `t`.
        """

    def has_south_western_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in south-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south_west(t)` is incoming to `t`.
        """

    def has_western_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `west(t)` is incoming to `t`.
        """

    def has_north_western_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in north-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north_west(t)` is incoming to `t`.
        """

    def is_outgoing_signal(
        self,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks whether signal `s` is outgoing from tile `t`. That is, whether
        tile `t` hosts a node that has a fanout assigned to the tile that
        signal `s` points to.

        Args:
            t: Base tile.
            s: Signal pointing to a potential outgoing tile of `t`.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `s` is outgoing from `t`.
        """

    def has_no_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has no outgoing tiles.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `t` does not have outgoing tiles.
        """

    def has_northern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in northern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north(t)` is outgoing from `t`.
        """

    def has_north_eastern_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in north-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north_east(t)` is outgoing from `t`.
        """

    def has_eastern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `east(t)` is outgoing from `t`.
        """

    def has_south_eastern_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in south-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south_east(t)` is outgoing from `t`.
        """

    def has_southern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in southern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south(t)` is outgoing from `t`.
        """

    def has_south_western_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in south-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south_west(t)` is outgoing from `t`.
        """

    def has_western_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `west(t)` is outgoing from `t`.
        """

    def has_north_western_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in north-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north_west(t)` is outgoing from `t`.
        """

    def bounding_box_2d(self) -> tuple[coords.OffsetCoordinate, coords.OffsetCoordinate]:
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

    def is_dead(self, n: int) -> bool:
        """
        Checks whether a node (not its assigned tile) is dead. Nodes can be
        dead for a variety of reasons. For instance if they are dangling (see
        the `mockturtle` API). In this layout type, nodes are also marked dead
        when they are not assigned to a tile (which is considered equivalent
        to dangling).

        Args:
            n: Node to check for liveliness.

        Returns:
            `true` iff `n` is dead.
        """

    def assign_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int], delay: int
    ) -> None:
        """
        Assigns a synchronization element to the provided clock zone.

        Args:
            cz: Clock zone to turn into a synchronization element.
            se: Number of full clock cycles to extend `cz`'s Hold phase by. If
                this value is 0, `cz` is turned back into a normal clock zone.
        """

    def is_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Check whether the provided clock zone is a synchronization element.

        Args:
            cz: Clock zone to check.

        Returns:
            `true` iff `cz` is a synchronization element.
        """

    def get_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> int:
        """
        Returns the Hold phase extension in clock cycles of clock zone `cz`.

        Args:
            cz: Clock zone to check.

        Returns:
            Synchronization element value, i.e., Hold phase extension, of
            clock zone `cz`.
        """

    def num_se(self) -> int:
        """
        Counts zones with a nonzero Hold-phase extension. @return
        Synchronization element count.
        """

class ShiftedCartesianGateLayout:
    """
    A gate layout with geometry, clocking, gates, wires, and obstructions. Gate creation takes and returns coordinates. Node inspection uses identifiers from get_node().
    """

    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(self, dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Standard constructor. Creates a named gate-level layout of the given
        aspect ratio. To this end, it calls `CoordinateLayout`'s standard
        constructor.

        Args:
            ar: Highest possible position in the layout.
            name: Layout name.
        """

    @overload
    def __init__(
        self,
        dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        clocking_scheme: str = "2DDWave",
        layout_name: str = "",
    ) -> None:
        """
        Standard constructor. Creates a gate-level layout of the given aspect
        ratio and clocks it via the given clocking scheme. To this end, it
        calls `CoordinateLayout`'s standard constructor.

        Args:
            ar: Highest possible position in the layout.
            scheme: Clocking scheme to apply to this layout.
            name: Layout name.
        """

    def coord(self, x: int, y: int, z: int = 0) -> coords.OffsetCoordinate:
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

    def resize(self, dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Updates the layout's dimensions, effectively resizing it.

        Args:
            ar: New aspect ratio.
        """

    def north(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def east(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def south(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def west(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def above(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
        """
        Returns the coordinate that is directly above a given coordinate `c`,
        i.e., the face whose z-dimension is higher by 1. If `c`'s z-dimension
        is already at maximum, `c` is returned instead.

        Args:
            c: Coordinate whose above counterpart is desired.

        Returns:
            Coordinate directly above `c`.
        """

    def below(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        c1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
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
        c1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
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

    def is_ground_layer(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in the ground layer
        where z is minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in ground layer.
        """

    def is_crossing_layer(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in a crossing layer
        where z is not minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in a crossing layer.
        """

    def is_within_bounds(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located within the layout
        bounds.

        Args:
            c: Coordinate to check for boundary.

        Returns:
            `true` iff `c` is located within the layout bounds.
        """

    def coordinates(self) -> list[coords.OffsetCoordinate]:
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

    def ground_coordinates(self) -> list[coords.OffsetCoordinate]:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[tuple[coords.OffsetCoordinate, coords.OffsetCoordinate]]:
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

    def assign_clock_number(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int], cn: int
    ) -> None:
        """
        Overrides the clock number of a tile in the stored scheme. The clock
        number applies to every layer of the tile, so the z-coordinate of `cz`
        is ignored.

        Args:
            cz: Clock zone to override.
            cn: New clock number for `cz`.
        """

    def get_clock_number(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the clock number of a tile. Every layer of a tile has the same
        clock number, so the z-coordinate of `cz` is ignored.

        Args:
            cz: Clock zone.

        Returns:
            Clock number of `cz`.
        """

    def num_clocks(self) -> int:
        """
        Returns the number of clock phases in the layout. Each clock cycle is
        divided into n phases. In QCA, the number of phases is usually 4. In
        iNML it is 3. Clocking schemes support 3 or 4 phases.

        Returns:
            The number of different clock signals in the layout.
        """

    def is_regularly_clocked(self) -> bool:
        """
        Returns whether the layout is clocked by a regular clocking scheme
        with no overwritten zones.

        Returns:
            `true` iff the layout is clocked by a regular scheme and no zones
            have been overwritten.
        """

    def is_clocking_scheme(self, name: str) -> bool:
        """
        Compares the stored clocking scheme against the provided name.
        Predefined names are constants in `fiction::layouts::clocking`.

        Args:
            name: Clocking scheme name.

        Returns:
            `true` iff the layout is clocked by a clocking scheme of name
            `name`.
        """

    def get_clocking_scheme_name(self) -> str:
        """
        Returns the name of the layout's clocking scheme, e.g., `2DDWave` or `USE`.
        """

    def is_incoming_clocked(
        self,
        cz1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Evaluates whether clock zone `cz2` feeds information to clock zone
        `cz1`, i.e., whether `cz2` is clocked with a clock number that is
        lower by 1 modulo `num_clocks()`, or either zone is a synchronization
        element.

        Args:
            cz1: Base clock zone.
            cz2: Clock zone to check whether its clock number is lower by 1.

        Returns:
            `true` iff `cz2` can feed information to `cz1`.
        """

    def is_outgoing_clocked(
        self,
        cz1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Evaluates whether clock zone `cz2` accepts information from clock zone
        `cz1`, i.e., whether `cz2` is clocked with a clock number that is
        higher by 1 modulo `num_clocks()`, or either zone is a synchronization
        element.

        Args:
            cz1: Base clock zone.
            cz2: Clock zone to check whether its clock number is higher by 1.

        Returns:
            `true` iff `cz2` can accept information from `cz1`.
        """

    def incoming_clocked_zones(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
        """
        Returns a container with all clock zones that are incoming to the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are incoming to `cz`.
        """

    def outgoing_clocked_zones(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
        """
        Returns a container with all clock zones that are outgoing from the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are outgoing from `cz`.
        """

    def in_degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of incoming clock zones to the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s incoming clock zones.
        """

    def out_degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of outgoing clock zones from the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s outgoing clock zones.
        """

    def degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of distinct incoming or outgoing neighboring clock
        zones.

        Args:
            cz: Base clock zone.

        Returns:
            Number of distinct clocked neighbors of `cz`.
        """

    def replace_clocking_scheme(self, name: str) -> None:
        """
        Replaces the clocking scheme by the predefined scheme of the given name. Clock-number overrides are discarded; synchronization elements are kept. Raises ValueError for an unknown name.
        """

    def obstruct_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Marks the given coordinate as obstructed.

        Args:
            c: clock_zone to obstruct.
        """

    def obstruct_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Marks the connection from coordinate `src` to coordinate `tgt` as
        obstructed.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Note:
            clock_zones marked this way will not be crossed with wires by path
            finding algorithms.
        """

    def clear_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Clears the obstruction status of the given coordinate `c` if the
        obstruction was manually marked via `obstruct_coordinate`.

        Args:
            c: clock_zone to clear.
        """

    def clear_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Clears the obstruction status of the connection from coordinate `src`
        to coordinate `tgt` if the obstruction was manually marked via
        `obstruct_connection`.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.
        """

    def clear_obstructed_coordinates(self) -> None:
        """
        Clears all obstructed coordinates that were manually marked via
        `obstruct_coordinate`.
        """

    def clear_obstructed_connections(self) -> None:
        """
        Clears all obstructed connections that were manually marked via
        `obstruct_connection`.
        """

    def is_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks if the given coordinate is obstructed of some sort.

        Args:
            c: Coordinate to check.

        Returns:
            `true` iff `c` is obstructed.
        """

    def is_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks if the given coordinate-coordinate connection is obstructed of
        some sort.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Returns:
            `true` iff the connection from `src` to `tgt` is obstructed.
        """

    def create_pi(
        self, name: str = "", t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...
    ) -> coords.OffsetCoordinate: ...
    def create_po(
        self,
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        name: str = "",
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def is_pi(self, n: int) -> bool:
        """
        Check whether `n` is a primary input.

        Args:
            n: Node to be checked.

        Returns:
            `true` iff `n` is a PI.
        """

    def is_po(self, n: int) -> bool:
        """
        Check whether `n` is a primary output.

        Args:
            n: Node to be checked.

        Returns:
            `true` iff `n` is a PO.
        """

    def is_pi_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary input.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PI.
        """

    def is_po_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary output.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PO.
        """

    def is_inv(self, arg: int, /) -> bool:
        """
        Returns whether `n` computes the binary inversion (NOT gate).

        Args:
            n: Node to check.

        Returns:
            `true` iff `n` is a NOT gate.
        """

    def is_and(self, arg: int, /) -> bool: ...
    def is_nand(self, arg: int, /) -> bool: ...
    def is_or(self, arg: int, /) -> bool: ...
    def is_nor(self, arg: int, /) -> bool: ...
    def is_xor(self, arg: int, /) -> bool: ...
    def is_xnor(self, arg: int, /) -> bool: ...
    def is_lt(self, arg: int, /) -> bool: ...
    def is_le(self, arg: int, /) -> bool: ...
    def is_gt(self, arg: int, /) -> bool: ...
    def is_ge(self, arg: int, /) -> bool: ...
    def is_maj(self, arg: int, /) -> bool: ...
    def is_fanout(self, arg: int, /) -> bool:
        """
        Returns whether `n` is a wire and has multiple outputs, thereby,
        acting as a fanout gate. Note that a fanout will return `true` for
        both `is_wire` and `is_fanout`.

        Args:
            n: Node to check.

        Returns:
            `true` iff `n` is a fanout gate.
        """

    def is_wire(self, arg: int, /) -> bool:
        """Equivalent to `is_buf`."""

    def set_layout_name(self, name: str) -> None: ...
    @property
    def name(self) -> str:
        """The layout name."""

    @name.setter
    def name(self, arg: str, /) -> None: ...
    def get_layout_name(self) -> str: ...
    def clone(self) -> ShiftedCartesianGateLayout:
        """
        Clones the layout returning a deep copy.

        Returns:
            Deep copy of the layout.
        """

    def set_input_name(self, index: int, name: str) -> None: ...
    def get_input_name(self, index: int) -> str: ...
    def set_output_name(self, index: int, name: str) -> None: ...
    def get_output_name(self, index: int) -> str: ...
    def get_name(self, s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> str: ...
    def create_buf(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_not(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_and(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_nand(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_or(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_nor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_xor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_xnor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_lt(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_le(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_gt(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_ge(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_maj(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def num_pis(self) -> int: ...
    def num_pos(self) -> int: ...
    def num_gates(self) -> int:
        """
        Returns the number of placed nodes in the layout that do not compute
        the identity function.

        Returns:
            Number of gates in the layout.
        """

    def num_wires(self) -> int:
        """
        Returns the number of placed nodes in the layout that compute the
        identity function including PIs and POs.

        Returns:
            Number of wires in the layout.
        """

    def num_crossings(self) -> int:
        """
        Returns the number of placed nodes in the layout that compute the
        identity function and cross other nodes.

        Returns:
            Number of crossings in the layout.
        """

    def is_empty(self) -> bool:
        """
        Checks whether there are no gates or wires assigned to the layout's
        coordinates.

        Returns:
            `true` iff the layout is empty.
        """

    def fanin_size(self, n: int) -> int:
        """
        Returns the number of incoming, adjacently placed, and properly
        clocked signals to the given node.

        Args:
            n: Node to check.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            Number of fanins to `n`.
        """

    def fanout_size(self, n: int) -> int:
        """
        Returns the number of outgoing, adjacently placed, and properly
        clocked signals of the given node.

        Args:
            n: Node to check.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            Number of fanouts to `n`.
        """

    def get_node(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Fetches the node that is placed onto a tile pointed to by a given
        signal. If no node is placed there, the `const0` node is returned.

        Args:
            s: Pointer to a tile.

        Returns:
            Node at position `t` where `s` points at `t`; or 0 if no node is
            placed at `t`.
        """

    def get_tile(self, n: int) -> coords.OffsetCoordinate:
        """
        The inverse function of `get_node`. Fetches the tile that the provided
        node is placed on. Returns a default dead tile if the node is not
        placed.

        Args:
            n: Node whose location is desired.

        Returns:
            Tile at which `n` is placed or a default dead tile if `n` is not
            placed.
        """

    def make_signal(self, n: int) -> coords.OffsetCoordinate:
        """
        Invokes the same behavior as `get_tile(n)` but additionally casts the
        return value to a signal. That is, this function returns the signal
        representation of the tile that the node `n` is assigned to.

        Args:
            n: Node whose signal is desired.

        Returns:
            Signal that points to `n`.
        """

    def move_node(
        self,
        n: int,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        new_children: Sequence[coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]] = [],
    ) -> coords.OffsetCoordinate:
        """
        Moves a given node to a new position and also updates its children,
        i.e., incoming signals.

        Args:
            n: Node to move.
            t: Tile to move `n` to.
            new_children: New incoming signals to `n`.

        Returns:
            Signal pointing to `n`'s new tile.
        """

    def clear_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Removes all assigned nodes from the given tile and marks them as dead.

        Args:
            t: Tile whose nodes are to be removed.

        Note:
            This function does not reduce the number of nodes in the layout
            nor does it reduce the number of PIs that are being returned via
            `num_pis()` even if the tile to clear is an input tile. However,
            the number of POs is reduced if the tile to clear is an output
            tile. While this seems counter-intuitive and inconsistent, it is
            in line with mockturtle's understanding of nodes and primary
            outputs.
        """

    def is_gate_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_gate` (in
        accordance with `mockturtle`'s definition of gates).

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that is a neither a constant nor a PI.
        """

    def is_wire_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_wire`.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that computes the identity.
        """

    def is_empty_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether `t` does not have a node assigned to it.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` is an empty tile.
        """

    def pis(self) -> list[coords.OffsetCoordinate]: ...
    def pos(self) -> list[coords.OffsetCoordinate]: ...
    def gates(self) -> list[coords.OffsetCoordinate]: ...
    def wires(self) -> list[coords.OffsetCoordinate]: ...
    def fanins(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]: ...
    def fanouts(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]: ...
    def is_incoming_signal(
        self,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks whether signal `s` is incoming to tile `t`. That is, whether
        tile `t` hosts a node that has a fanin assigned to the tile that
        signal `s` points to.

        Args:
            t: Base tile.
            s: Signal pointing to a potential incoming tile to `t`.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `s` is incoming to `t`.
        """

    def has_no_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has no incoming tiles.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `t` does not have incoming tiles.
        """

    def has_northern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in northern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north(t)` is incoming to `t`.
        """

    def has_north_eastern_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in north-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north_east(t)` is incoming to `t`.
        """

    def has_eastern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `east(t)` is incoming to `t`.
        """

    def has_south_eastern_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in south-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south_east(t)` is incoming to `t`.
        """

    def has_southern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in southern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south(t)` is incoming to `t`.
        """

    def has_south_western_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in south-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south_west(t)` is incoming to `t`.
        """

    def has_western_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `west(t)` is incoming to `t`.
        """

    def has_north_western_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in north-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north_west(t)` is incoming to `t`.
        """

    def is_outgoing_signal(
        self,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks whether signal `s` is outgoing from tile `t`. That is, whether
        tile `t` hosts a node that has a fanout assigned to the tile that
        signal `s` points to.

        Args:
            t: Base tile.
            s: Signal pointing to a potential outgoing tile of `t`.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `s` is outgoing from `t`.
        """

    def has_no_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has no outgoing tiles.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `t` does not have outgoing tiles.
        """

    def has_northern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in northern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north(t)` is outgoing from `t`.
        """

    def has_north_eastern_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in north-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north_east(t)` is outgoing from `t`.
        """

    def has_eastern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `east(t)` is outgoing from `t`.
        """

    def has_south_eastern_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in south-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south_east(t)` is outgoing from `t`.
        """

    def has_southern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in southern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south(t)` is outgoing from `t`.
        """

    def has_south_western_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in south-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south_west(t)` is outgoing from `t`.
        """

    def has_western_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `west(t)` is outgoing from `t`.
        """

    def has_north_western_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in north-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north_west(t)` is outgoing from `t`.
        """

    def bounding_box_2d(self) -> tuple[coords.OffsetCoordinate, coords.OffsetCoordinate]:
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

    def is_dead(self, n: int) -> bool:
        """
        Checks whether a node (not its assigned tile) is dead. Nodes can be
        dead for a variety of reasons. For instance if they are dangling (see
        the `mockturtle` API). In this layout type, nodes are also marked dead
        when they are not assigned to a tile (which is considered equivalent
        to dangling).

        Args:
            n: Node to check for liveliness.

        Returns:
            `true` iff `n` is dead.
        """

    def assign_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int], delay: int
    ) -> None:
        """
        Assigns a synchronization element to the provided clock zone.

        Args:
            cz: Clock zone to turn into a synchronization element.
            se: Number of full clock cycles to extend `cz`'s Hold phase by. If
                this value is 0, `cz` is turned back into a normal clock zone.
        """

    def is_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Check whether the provided clock zone is a synchronization element.

        Args:
            cz: Clock zone to check.

        Returns:
            `true` iff `cz` is a synchronization element.
        """

    def get_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> int:
        """
        Returns the Hold phase extension in clock cycles of clock zone `cz`.

        Args:
            cz: Clock zone to check.

        Returns:
            Synchronization element value, i.e., Hold phase extension, of
            clock zone `cz`.
        """

    def num_se(self) -> int:
        """
        Counts zones with a nonzero Hold-phase extension. @return
        Synchronization element count.
        """

class HexagonalGateLayout:
    """
    A gate layout with geometry, clocking, gates, wires, and obstructions. Gate creation takes and returns coordinates. Node inspection uses identifiers from get_node().
    """

    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(self, dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Standard constructor. Creates a named gate-level layout of the given
        aspect ratio. To this end, it calls `CoordinateLayout`'s standard
        constructor.

        Args:
            ar: Highest possible position in the layout.
            name: Layout name.
        """

    @overload
    def __init__(
        self,
        dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        clocking_scheme: str = "2DDWave",
        layout_name: str = "",
    ) -> None:
        """
        Standard constructor. Creates a gate-level layout of the given aspect
        ratio and clocks it via the given clocking scheme. To this end, it
        calls `CoordinateLayout`'s standard constructor.

        Args:
            ar: Highest possible position in the layout.
            scheme: Clocking scheme to apply to this layout.
            name: Layout name.
        """

    def coord(self, x: int, y: int, z: int = 0) -> coords.OffsetCoordinate:
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

    def resize(self, dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Updates the layout's dimensions, effectively resizing it.

        Args:
            ar: New aspect ratio.
        """

    def north(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def east(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def south(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def west(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def above(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
        """
        Returns the coordinate that is directly above a given coordinate `c`,
        i.e., the face whose z-dimension is higher by 1. If `c`'s z-dimension
        is already at maximum, `c` is returned instead.

        Args:
            c: Coordinate whose above counterpart is desired.

        Returns:
            Coordinate directly above `c`.
        """

    def below(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        c1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
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
        c1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
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

    def is_ground_layer(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in the ground layer
        where z is minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in ground layer.
        """

    def is_crossing_layer(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in a crossing layer
        where z is not minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in a crossing layer.
        """

    def is_within_bounds(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located within the layout
        bounds.

        Args:
            c: Coordinate to check for boundary.

        Returns:
            `true` iff `c` is located within the layout bounds.
        """

    def coordinates(self) -> list[coords.OffsetCoordinate]:
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

    def ground_coordinates(self) -> list[coords.OffsetCoordinate]:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[tuple[coords.OffsetCoordinate, coords.OffsetCoordinate]]:
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

    def assign_clock_number(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int], cn: int
    ) -> None:
        """
        Overrides the clock number of a tile in the stored scheme. The clock
        number applies to every layer of the tile, so the z-coordinate of `cz`
        is ignored.

        Args:
            cz: Clock zone to override.
            cn: New clock number for `cz`.
        """

    def get_clock_number(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the clock number of a tile. Every layer of a tile has the same
        clock number, so the z-coordinate of `cz` is ignored.

        Args:
            cz: Clock zone.

        Returns:
            Clock number of `cz`.
        """

    def num_clocks(self) -> int:
        """
        Returns the number of clock phases in the layout. Each clock cycle is
        divided into n phases. In QCA, the number of phases is usually 4. In
        iNML it is 3. Clocking schemes support 3 or 4 phases.

        Returns:
            The number of different clock signals in the layout.
        """

    def is_regularly_clocked(self) -> bool:
        """
        Returns whether the layout is clocked by a regular clocking scheme
        with no overwritten zones.

        Returns:
            `true` iff the layout is clocked by a regular scheme and no zones
            have been overwritten.
        """

    def is_clocking_scheme(self, name: str) -> bool:
        """
        Compares the stored clocking scheme against the provided name.
        Predefined names are constants in `fiction::layouts::clocking`.

        Args:
            name: Clocking scheme name.

        Returns:
            `true` iff the layout is clocked by a clocking scheme of name
            `name`.
        """

    def get_clocking_scheme_name(self) -> str:
        """
        Returns the name of the layout's clocking scheme, e.g., `2DDWave` or `USE`.
        """

    def is_incoming_clocked(
        self,
        cz1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Evaluates whether clock zone `cz2` feeds information to clock zone
        `cz1`, i.e., whether `cz2` is clocked with a clock number that is
        lower by 1 modulo `num_clocks()`, or either zone is a synchronization
        element.

        Args:
            cz1: Base clock zone.
            cz2: Clock zone to check whether its clock number is lower by 1.

        Returns:
            `true` iff `cz2` can feed information to `cz1`.
        """

    def is_outgoing_clocked(
        self,
        cz1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Evaluates whether clock zone `cz2` accepts information from clock zone
        `cz1`, i.e., whether `cz2` is clocked with a clock number that is
        higher by 1 modulo `num_clocks()`, or either zone is a synchronization
        element.

        Args:
            cz1: Base clock zone.
            cz2: Clock zone to check whether its clock number is higher by 1.

        Returns:
            `true` iff `cz2` can accept information from `cz1`.
        """

    def incoming_clocked_zones(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
        """
        Returns a container with all clock zones that are incoming to the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are incoming to `cz`.
        """

    def outgoing_clocked_zones(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
        """
        Returns a container with all clock zones that are outgoing from the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are outgoing from `cz`.
        """

    def in_degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of incoming clock zones to the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s incoming clock zones.
        """

    def out_degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of outgoing clock zones from the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s outgoing clock zones.
        """

    def degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of distinct incoming or outgoing neighboring clock
        zones.

        Args:
            cz: Base clock zone.

        Returns:
            Number of distinct clocked neighbors of `cz`.
        """

    def replace_clocking_scheme(self, name: str) -> None:
        """
        Replaces the clocking scheme by the predefined scheme of the given name. Clock-number overrides are discarded; synchronization elements are kept. Raises ValueError for an unknown name.
        """

    def obstruct_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Marks the given coordinate as obstructed.

        Args:
            c: clock_zone to obstruct.
        """

    def obstruct_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Marks the connection from coordinate `src` to coordinate `tgt` as
        obstructed.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Note:
            clock_zones marked this way will not be crossed with wires by path
            finding algorithms.
        """

    def clear_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Clears the obstruction status of the given coordinate `c` if the
        obstruction was manually marked via `obstruct_coordinate`.

        Args:
            c: clock_zone to clear.
        """

    def clear_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Clears the obstruction status of the connection from coordinate `src`
        to coordinate `tgt` if the obstruction was manually marked via
        `obstruct_connection`.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.
        """

    def clear_obstructed_coordinates(self) -> None:
        """
        Clears all obstructed coordinates that were manually marked via
        `obstruct_coordinate`.
        """

    def clear_obstructed_connections(self) -> None:
        """
        Clears all obstructed connections that were manually marked via
        `obstruct_connection`.
        """

    def is_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks if the given coordinate is obstructed of some sort.

        Args:
            c: Coordinate to check.

        Returns:
            `true` iff `c` is obstructed.
        """

    def is_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks if the given coordinate-coordinate connection is obstructed of
        some sort.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Returns:
            `true` iff the connection from `src` to `tgt` is obstructed.
        """

    def create_pi(
        self, name: str = "", t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...
    ) -> coords.OffsetCoordinate: ...
    def create_po(
        self,
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        name: str = "",
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def is_pi(self, n: int) -> bool:
        """
        Check whether `n` is a primary input.

        Args:
            n: Node to be checked.

        Returns:
            `true` iff `n` is a PI.
        """

    def is_po(self, n: int) -> bool:
        """
        Check whether `n` is a primary output.

        Args:
            n: Node to be checked.

        Returns:
            `true` iff `n` is a PO.
        """

    def is_pi_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary input.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PI.
        """

    def is_po_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary output.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PO.
        """

    def is_inv(self, arg: int, /) -> bool:
        """
        Returns whether `n` computes the binary inversion (NOT gate).

        Args:
            n: Node to check.

        Returns:
            `true` iff `n` is a NOT gate.
        """

    def is_and(self, arg: int, /) -> bool: ...
    def is_nand(self, arg: int, /) -> bool: ...
    def is_or(self, arg: int, /) -> bool: ...
    def is_nor(self, arg: int, /) -> bool: ...
    def is_xor(self, arg: int, /) -> bool: ...
    def is_xnor(self, arg: int, /) -> bool: ...
    def is_lt(self, arg: int, /) -> bool: ...
    def is_le(self, arg: int, /) -> bool: ...
    def is_gt(self, arg: int, /) -> bool: ...
    def is_ge(self, arg: int, /) -> bool: ...
    def is_maj(self, arg: int, /) -> bool: ...
    def is_fanout(self, arg: int, /) -> bool:
        """
        Returns whether `n` is a wire and has multiple outputs, thereby,
        acting as a fanout gate. Note that a fanout will return `true` for
        both `is_wire` and `is_fanout`.

        Args:
            n: Node to check.

        Returns:
            `true` iff `n` is a fanout gate.
        """

    def is_wire(self, arg: int, /) -> bool:
        """Equivalent to `is_buf`."""

    def set_layout_name(self, name: str) -> None: ...
    @property
    def name(self) -> str:
        """The layout name."""

    @name.setter
    def name(self, arg: str, /) -> None: ...
    def get_layout_name(self) -> str: ...
    def clone(self) -> HexagonalGateLayout:
        """
        Clones the layout returning a deep copy.

        Returns:
            Deep copy of the layout.
        """

    def set_input_name(self, index: int, name: str) -> None: ...
    def get_input_name(self, index: int) -> str: ...
    def set_output_name(self, index: int, name: str) -> None: ...
    def get_output_name(self, index: int) -> str: ...
    def get_name(self, s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> str: ...
    def create_buf(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_not(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_and(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_nand(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_or(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_nor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_xor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_xnor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_lt(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_le(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_gt(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_ge(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_maj(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def num_pis(self) -> int: ...
    def num_pos(self) -> int: ...
    def num_gates(self) -> int:
        """
        Returns the number of placed nodes in the layout that do not compute
        the identity function.

        Returns:
            Number of gates in the layout.
        """

    def num_wires(self) -> int:
        """
        Returns the number of placed nodes in the layout that compute the
        identity function including PIs and POs.

        Returns:
            Number of wires in the layout.
        """

    def num_crossings(self) -> int:
        """
        Returns the number of placed nodes in the layout that compute the
        identity function and cross other nodes.

        Returns:
            Number of crossings in the layout.
        """

    def is_empty(self) -> bool:
        """
        Checks whether there are no gates or wires assigned to the layout's
        coordinates.

        Returns:
            `true` iff the layout is empty.
        """

    def fanin_size(self, n: int) -> int:
        """
        Returns the number of incoming, adjacently placed, and properly
        clocked signals to the given node.

        Args:
            n: Node to check.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            Number of fanins to `n`.
        """

    def fanout_size(self, n: int) -> int:
        """
        Returns the number of outgoing, adjacently placed, and properly
        clocked signals of the given node.

        Args:
            n: Node to check.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            Number of fanouts to `n`.
        """

    def get_node(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Fetches the node that is placed onto a tile pointed to by a given
        signal. If no node is placed there, the `const0` node is returned.

        Args:
            s: Pointer to a tile.

        Returns:
            Node at position `t` where `s` points at `t`; or 0 if no node is
            placed at `t`.
        """

    def get_tile(self, n: int) -> coords.OffsetCoordinate:
        """
        The inverse function of `get_node`. Fetches the tile that the provided
        node is placed on. Returns a default dead tile if the node is not
        placed.

        Args:
            n: Node whose location is desired.

        Returns:
            Tile at which `n` is placed or a default dead tile if `n` is not
            placed.
        """

    def make_signal(self, n: int) -> coords.OffsetCoordinate:
        """
        Invokes the same behavior as `get_tile(n)` but additionally casts the
        return value to a signal. That is, this function returns the signal
        representation of the tile that the node `n` is assigned to.

        Args:
            n: Node whose signal is desired.

        Returns:
            Signal that points to `n`.
        """

    def move_node(
        self,
        n: int,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        new_children: Sequence[coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]] = [],
    ) -> coords.OffsetCoordinate:
        """
        Moves a given node to a new position and also updates its children,
        i.e., incoming signals.

        Args:
            n: Node to move.
            t: Tile to move `n` to.
            new_children: New incoming signals to `n`.

        Returns:
            Signal pointing to `n`'s new tile.
        """

    def clear_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Removes all assigned nodes from the given tile and marks them as dead.

        Args:
            t: Tile whose nodes are to be removed.

        Note:
            This function does not reduce the number of nodes in the layout
            nor does it reduce the number of PIs that are being returned via
            `num_pis()` even if the tile to clear is an input tile. However,
            the number of POs is reduced if the tile to clear is an output
            tile. While this seems counter-intuitive and inconsistent, it is
            in line with mockturtle's understanding of nodes and primary
            outputs.
        """

    def is_gate_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_gate` (in
        accordance with `mockturtle`'s definition of gates).

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that is a neither a constant nor a PI.
        """

    def is_wire_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_wire`.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that computes the identity.
        """

    def is_empty_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether `t` does not have a node assigned to it.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` is an empty tile.
        """

    def pis(self) -> list[coords.OffsetCoordinate]: ...
    def pos(self) -> list[coords.OffsetCoordinate]: ...
    def gates(self) -> list[coords.OffsetCoordinate]: ...
    def wires(self) -> list[coords.OffsetCoordinate]: ...
    def fanins(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]: ...
    def fanouts(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]: ...
    def is_incoming_signal(
        self,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks whether signal `s` is incoming to tile `t`. That is, whether
        tile `t` hosts a node that has a fanin assigned to the tile that
        signal `s` points to.

        Args:
            t: Base tile.
            s: Signal pointing to a potential incoming tile to `t`.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `s` is incoming to `t`.
        """

    def has_no_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has no incoming tiles.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `t` does not have incoming tiles.
        """

    def has_northern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in northern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north(t)` is incoming to `t`.
        """

    def has_north_eastern_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in north-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north_east(t)` is incoming to `t`.
        """

    def has_eastern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `east(t)` is incoming to `t`.
        """

    def has_south_eastern_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in south-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south_east(t)` is incoming to `t`.
        """

    def has_southern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in southern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south(t)` is incoming to `t`.
        """

    def has_south_western_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in south-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south_west(t)` is incoming to `t`.
        """

    def has_western_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `west(t)` is incoming to `t`.
        """

    def has_north_western_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in north-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north_west(t)` is incoming to `t`.
        """

    def is_outgoing_signal(
        self,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks whether signal `s` is outgoing from tile `t`. That is, whether
        tile `t` hosts a node that has a fanout assigned to the tile that
        signal `s` points to.

        Args:
            t: Base tile.
            s: Signal pointing to a potential outgoing tile of `t`.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `s` is outgoing from `t`.
        """

    def has_no_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has no outgoing tiles.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `t` does not have outgoing tiles.
        """

    def has_northern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in northern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north(t)` is outgoing from `t`.
        """

    def has_north_eastern_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in north-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north_east(t)` is outgoing from `t`.
        """

    def has_eastern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `east(t)` is outgoing from `t`.
        """

    def has_south_eastern_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in south-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south_east(t)` is outgoing from `t`.
        """

    def has_southern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in southern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south(t)` is outgoing from `t`.
        """

    def has_south_western_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in south-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south_west(t)` is outgoing from `t`.
        """

    def has_western_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `west(t)` is outgoing from `t`.
        """

    def has_north_western_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in north-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north_west(t)` is outgoing from `t`.
        """

    def bounding_box_2d(self) -> tuple[coords.OffsetCoordinate, coords.OffsetCoordinate]:
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

    def is_dead(self, n: int) -> bool:
        """
        Checks whether a node (not its assigned tile) is dead. Nodes can be
        dead for a variety of reasons. For instance if they are dangling (see
        the `mockturtle` API). In this layout type, nodes are also marked dead
        when they are not assigned to a tile (which is considered equivalent
        to dangling).

        Args:
            n: Node to check for liveliness.

        Returns:
            `true` iff `n` is dead.
        """

    def assign_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int], delay: int
    ) -> None:
        """
        Assigns a synchronization element to the provided clock zone.

        Args:
            cz: Clock zone to turn into a synchronization element.
            se: Number of full clock cycles to extend `cz`'s Hold phase by. If
                this value is 0, `cz` is turned back into a normal clock zone.
        """

    def is_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Check whether the provided clock zone is a synchronization element.

        Args:
            cz: Clock zone to check.

        Returns:
            `true` iff `cz` is a synchronization element.
        """

    def get_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> int:
        """
        Returns the Hold phase extension in clock cycles of clock zone `cz`.

        Args:
            cz: Clock zone to check.

        Returns:
            Synchronization element value, i.e., Hold phase extension, of
            clock zone `cz`.
        """

    def num_se(self) -> int:
        """
        Counts zones with a nonzero Hold-phase extension. @return
        Synchronization element count.
        """

class OddRowCartesianGateLayout:
    """
    A gate layout with geometry, clocking, gates, wires, and obstructions. Gate creation takes and returns coordinates. Node inspection uses identifiers from get_node().
    """

    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(self, dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Standard constructor. Creates a named gate-level layout of the given
        aspect ratio. To this end, it calls `CoordinateLayout`'s standard
        constructor.

        Args:
            ar: Highest possible position in the layout.
            name: Layout name.
        """

    @overload
    def __init__(
        self,
        dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        clocking_scheme: str = "2DDWave",
        layout_name: str = "",
    ) -> None:
        """
        Standard constructor. Creates a gate-level layout of the given aspect
        ratio and clocks it via the given clocking scheme. To this end, it
        calls `CoordinateLayout`'s standard constructor.

        Args:
            ar: Highest possible position in the layout.
            scheme: Clocking scheme to apply to this layout.
            name: Layout name.
        """

    def coord(self, x: int, y: int, z: int = 0) -> coords.OffsetCoordinate:
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

    def resize(self, dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Updates the layout's dimensions, effectively resizing it.

        Args:
            ar: New aspect ratio.
        """

    def north(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def east(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def south(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def west(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def above(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
        """
        Returns the coordinate that is directly above a given coordinate `c`,
        i.e., the face whose z-dimension is higher by 1. If `c`'s z-dimension
        is already at maximum, `c` is returned instead.

        Args:
            c: Coordinate whose above counterpart is desired.

        Returns:
            Coordinate directly above `c`.
        """

    def below(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        c1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
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
        c1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
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

    def is_ground_layer(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in the ground layer
        where z is minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in ground layer.
        """

    def is_crossing_layer(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in a crossing layer
        where z is not minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in a crossing layer.
        """

    def is_within_bounds(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located within the layout
        bounds.

        Args:
            c: Coordinate to check for boundary.

        Returns:
            `true` iff `c` is located within the layout bounds.
        """

    def coordinates(self) -> list[coords.OffsetCoordinate]:
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

    def ground_coordinates(self) -> list[coords.OffsetCoordinate]:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[tuple[coords.OffsetCoordinate, coords.OffsetCoordinate]]:
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

    def assign_clock_number(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int], cn: int
    ) -> None:
        """
        Overrides the clock number of a tile in the stored scheme. The clock
        number applies to every layer of the tile, so the z-coordinate of `cz`
        is ignored.

        Args:
            cz: Clock zone to override.
            cn: New clock number for `cz`.
        """

    def get_clock_number(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the clock number of a tile. Every layer of a tile has the same
        clock number, so the z-coordinate of `cz` is ignored.

        Args:
            cz: Clock zone.

        Returns:
            Clock number of `cz`.
        """

    def num_clocks(self) -> int:
        """
        Returns the number of clock phases in the layout. Each clock cycle is
        divided into n phases. In QCA, the number of phases is usually 4. In
        iNML it is 3. Clocking schemes support 3 or 4 phases.

        Returns:
            The number of different clock signals in the layout.
        """

    def is_regularly_clocked(self) -> bool:
        """
        Returns whether the layout is clocked by a regular clocking scheme
        with no overwritten zones.

        Returns:
            `true` iff the layout is clocked by a regular scheme and no zones
            have been overwritten.
        """

    def is_clocking_scheme(self, name: str) -> bool:
        """
        Compares the stored clocking scheme against the provided name.
        Predefined names are constants in `fiction::layouts::clocking`.

        Args:
            name: Clocking scheme name.

        Returns:
            `true` iff the layout is clocked by a clocking scheme of name
            `name`.
        """

    def get_clocking_scheme_name(self) -> str:
        """
        Returns the name of the layout's clocking scheme, e.g., `2DDWave` or `USE`.
        """

    def is_incoming_clocked(
        self,
        cz1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Evaluates whether clock zone `cz2` feeds information to clock zone
        `cz1`, i.e., whether `cz2` is clocked with a clock number that is
        lower by 1 modulo `num_clocks()`, or either zone is a synchronization
        element.

        Args:
            cz1: Base clock zone.
            cz2: Clock zone to check whether its clock number is lower by 1.

        Returns:
            `true` iff `cz2` can feed information to `cz1`.
        """

    def is_outgoing_clocked(
        self,
        cz1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Evaluates whether clock zone `cz2` accepts information from clock zone
        `cz1`, i.e., whether `cz2` is clocked with a clock number that is
        higher by 1 modulo `num_clocks()`, or either zone is a synchronization
        element.

        Args:
            cz1: Base clock zone.
            cz2: Clock zone to check whether its clock number is higher by 1.

        Returns:
            `true` iff `cz2` can accept information from `cz1`.
        """

    def incoming_clocked_zones(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
        """
        Returns a container with all clock zones that are incoming to the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are incoming to `cz`.
        """

    def outgoing_clocked_zones(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
        """
        Returns a container with all clock zones that are outgoing from the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are outgoing from `cz`.
        """

    def in_degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of incoming clock zones to the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s incoming clock zones.
        """

    def out_degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of outgoing clock zones from the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s outgoing clock zones.
        """

    def degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of distinct incoming or outgoing neighboring clock
        zones.

        Args:
            cz: Base clock zone.

        Returns:
            Number of distinct clocked neighbors of `cz`.
        """

    def replace_clocking_scheme(self, name: str) -> None:
        """
        Replaces the clocking scheme by the predefined scheme of the given name. Clock-number overrides are discarded; synchronization elements are kept. Raises ValueError for an unknown name.
        """

    def obstruct_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Marks the given coordinate as obstructed.

        Args:
            c: clock_zone to obstruct.
        """

    def obstruct_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Marks the connection from coordinate `src` to coordinate `tgt` as
        obstructed.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Note:
            clock_zones marked this way will not be crossed with wires by path
            finding algorithms.
        """

    def clear_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Clears the obstruction status of the given coordinate `c` if the
        obstruction was manually marked via `obstruct_coordinate`.

        Args:
            c: clock_zone to clear.
        """

    def clear_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Clears the obstruction status of the connection from coordinate `src`
        to coordinate `tgt` if the obstruction was manually marked via
        `obstruct_connection`.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.
        """

    def clear_obstructed_coordinates(self) -> None:
        """
        Clears all obstructed coordinates that were manually marked via
        `obstruct_coordinate`.
        """

    def clear_obstructed_connections(self) -> None:
        """
        Clears all obstructed connections that were manually marked via
        `obstruct_connection`.
        """

    def is_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks if the given coordinate is obstructed of some sort.

        Args:
            c: Coordinate to check.

        Returns:
            `true` iff `c` is obstructed.
        """

    def is_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks if the given coordinate-coordinate connection is obstructed of
        some sort.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Returns:
            `true` iff the connection from `src` to `tgt` is obstructed.
        """

    def create_pi(
        self, name: str = "", t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...
    ) -> coords.OffsetCoordinate: ...
    def create_po(
        self,
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        name: str = "",
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def is_pi(self, n: int) -> bool:
        """
        Check whether `n` is a primary input.

        Args:
            n: Node to be checked.

        Returns:
            `true` iff `n` is a PI.
        """

    def is_po(self, n: int) -> bool:
        """
        Check whether `n` is a primary output.

        Args:
            n: Node to be checked.

        Returns:
            `true` iff `n` is a PO.
        """

    def is_pi_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary input.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PI.
        """

    def is_po_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary output.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PO.
        """

    def is_inv(self, arg: int, /) -> bool:
        """
        Returns whether `n` computes the binary inversion (NOT gate).

        Args:
            n: Node to check.

        Returns:
            `true` iff `n` is a NOT gate.
        """

    def is_and(self, arg: int, /) -> bool: ...
    def is_nand(self, arg: int, /) -> bool: ...
    def is_or(self, arg: int, /) -> bool: ...
    def is_nor(self, arg: int, /) -> bool: ...
    def is_xor(self, arg: int, /) -> bool: ...
    def is_xnor(self, arg: int, /) -> bool: ...
    def is_lt(self, arg: int, /) -> bool: ...
    def is_le(self, arg: int, /) -> bool: ...
    def is_gt(self, arg: int, /) -> bool: ...
    def is_ge(self, arg: int, /) -> bool: ...
    def is_maj(self, arg: int, /) -> bool: ...
    def is_fanout(self, arg: int, /) -> bool:
        """
        Returns whether `n` is a wire and has multiple outputs, thereby,
        acting as a fanout gate. Note that a fanout will return `true` for
        both `is_wire` and `is_fanout`.

        Args:
            n: Node to check.

        Returns:
            `true` iff `n` is a fanout gate.
        """

    def is_wire(self, arg: int, /) -> bool:
        """Equivalent to `is_buf`."""

    def set_layout_name(self, name: str) -> None: ...
    @property
    def name(self) -> str:
        """The layout name."""

    @name.setter
    def name(self, arg: str, /) -> None: ...
    def get_layout_name(self) -> str: ...
    def clone(self) -> OddRowCartesianGateLayout:
        """
        Clones the layout returning a deep copy.

        Returns:
            Deep copy of the layout.
        """

    def set_input_name(self, index: int, name: str) -> None: ...
    def get_input_name(self, index: int) -> str: ...
    def set_output_name(self, index: int, name: str) -> None: ...
    def get_output_name(self, index: int) -> str: ...
    def get_name(self, s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> str: ...
    def create_buf(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_not(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_and(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_nand(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_or(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_nor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_xor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_xnor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_lt(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_le(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_gt(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_ge(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_maj(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def num_pis(self) -> int: ...
    def num_pos(self) -> int: ...
    def num_gates(self) -> int:
        """
        Returns the number of placed nodes in the layout that do not compute
        the identity function.

        Returns:
            Number of gates in the layout.
        """

    def num_wires(self) -> int:
        """
        Returns the number of placed nodes in the layout that compute the
        identity function including PIs and POs.

        Returns:
            Number of wires in the layout.
        """

    def num_crossings(self) -> int:
        """
        Returns the number of placed nodes in the layout that compute the
        identity function and cross other nodes.

        Returns:
            Number of crossings in the layout.
        """

    def is_empty(self) -> bool:
        """
        Checks whether there are no gates or wires assigned to the layout's
        coordinates.

        Returns:
            `true` iff the layout is empty.
        """

    def fanin_size(self, n: int) -> int:
        """
        Returns the number of incoming, adjacently placed, and properly
        clocked signals to the given node.

        Args:
            n: Node to check.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            Number of fanins to `n`.
        """

    def fanout_size(self, n: int) -> int:
        """
        Returns the number of outgoing, adjacently placed, and properly
        clocked signals of the given node.

        Args:
            n: Node to check.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            Number of fanouts to `n`.
        """

    def get_node(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Fetches the node that is placed onto a tile pointed to by a given
        signal. If no node is placed there, the `const0` node is returned.

        Args:
            s: Pointer to a tile.

        Returns:
            Node at position `t` where `s` points at `t`; or 0 if no node is
            placed at `t`.
        """

    def get_tile(self, n: int) -> coords.OffsetCoordinate:
        """
        The inverse function of `get_node`. Fetches the tile that the provided
        node is placed on. Returns a default dead tile if the node is not
        placed.

        Args:
            n: Node whose location is desired.

        Returns:
            Tile at which `n` is placed or a default dead tile if `n` is not
            placed.
        """

    def make_signal(self, n: int) -> coords.OffsetCoordinate:
        """
        Invokes the same behavior as `get_tile(n)` but additionally casts the
        return value to a signal. That is, this function returns the signal
        representation of the tile that the node `n` is assigned to.

        Args:
            n: Node whose signal is desired.

        Returns:
            Signal that points to `n`.
        """

    def move_node(
        self,
        n: int,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        new_children: Sequence[coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]] = [],
    ) -> coords.OffsetCoordinate:
        """
        Moves a given node to a new position and also updates its children,
        i.e., incoming signals.

        Args:
            n: Node to move.
            t: Tile to move `n` to.
            new_children: New incoming signals to `n`.

        Returns:
            Signal pointing to `n`'s new tile.
        """

    def clear_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Removes all assigned nodes from the given tile and marks them as dead.

        Args:
            t: Tile whose nodes are to be removed.

        Note:
            This function does not reduce the number of nodes in the layout
            nor does it reduce the number of PIs that are being returned via
            `num_pis()` even if the tile to clear is an input tile. However,
            the number of POs is reduced if the tile to clear is an output
            tile. While this seems counter-intuitive and inconsistent, it is
            in line with mockturtle's understanding of nodes and primary
            outputs.
        """

    def is_gate_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_gate` (in
        accordance with `mockturtle`'s definition of gates).

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that is a neither a constant nor a PI.
        """

    def is_wire_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_wire`.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that computes the identity.
        """

    def is_empty_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether `t` does not have a node assigned to it.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` is an empty tile.
        """

    def pis(self) -> list[coords.OffsetCoordinate]: ...
    def pos(self) -> list[coords.OffsetCoordinate]: ...
    def gates(self) -> list[coords.OffsetCoordinate]: ...
    def wires(self) -> list[coords.OffsetCoordinate]: ...
    def fanins(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]: ...
    def fanouts(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]: ...
    def is_incoming_signal(
        self,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks whether signal `s` is incoming to tile `t`. That is, whether
        tile `t` hosts a node that has a fanin assigned to the tile that
        signal `s` points to.

        Args:
            t: Base tile.
            s: Signal pointing to a potential incoming tile to `t`.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `s` is incoming to `t`.
        """

    def has_no_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has no incoming tiles.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `t` does not have incoming tiles.
        """

    def has_northern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in northern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north(t)` is incoming to `t`.
        """

    def has_north_eastern_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in north-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north_east(t)` is incoming to `t`.
        """

    def has_eastern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `east(t)` is incoming to `t`.
        """

    def has_south_eastern_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in south-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south_east(t)` is incoming to `t`.
        """

    def has_southern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in southern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south(t)` is incoming to `t`.
        """

    def has_south_western_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in south-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south_west(t)` is incoming to `t`.
        """

    def has_western_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `west(t)` is incoming to `t`.
        """

    def has_north_western_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in north-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north_west(t)` is incoming to `t`.
        """

    def is_outgoing_signal(
        self,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks whether signal `s` is outgoing from tile `t`. That is, whether
        tile `t` hosts a node that has a fanout assigned to the tile that
        signal `s` points to.

        Args:
            t: Base tile.
            s: Signal pointing to a potential outgoing tile of `t`.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `s` is outgoing from `t`.
        """

    def has_no_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has no outgoing tiles.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `t` does not have outgoing tiles.
        """

    def has_northern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in northern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north(t)` is outgoing from `t`.
        """

    def has_north_eastern_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in north-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north_east(t)` is outgoing from `t`.
        """

    def has_eastern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `east(t)` is outgoing from `t`.
        """

    def has_south_eastern_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in south-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south_east(t)` is outgoing from `t`.
        """

    def has_southern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in southern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south(t)` is outgoing from `t`.
        """

    def has_south_western_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in south-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south_west(t)` is outgoing from `t`.
        """

    def has_western_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `west(t)` is outgoing from `t`.
        """

    def has_north_western_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in north-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north_west(t)` is outgoing from `t`.
        """

    def bounding_box_2d(self) -> tuple[coords.OffsetCoordinate, coords.OffsetCoordinate]:
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

    def is_dead(self, n: int) -> bool:
        """
        Checks whether a node (not its assigned tile) is dead. Nodes can be
        dead for a variety of reasons. For instance if they are dangling (see
        the `mockturtle` API). In this layout type, nodes are also marked dead
        when they are not assigned to a tile (which is considered equivalent
        to dangling).

        Args:
            n: Node to check for liveliness.

        Returns:
            `true` iff `n` is dead.
        """

    def assign_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int], delay: int
    ) -> None:
        """
        Assigns a synchronization element to the provided clock zone.

        Args:
            cz: Clock zone to turn into a synchronization element.
            se: Number of full clock cycles to extend `cz`'s Hold phase by. If
                this value is 0, `cz` is turned back into a normal clock zone.
        """

    def is_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Check whether the provided clock zone is a synchronization element.

        Args:
            cz: Clock zone to check.

        Returns:
            `true` iff `cz` is a synchronization element.
        """

    def get_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> int:
        """
        Returns the Hold phase extension in clock cycles of clock zone `cz`.

        Args:
            cz: Clock zone to check.

        Returns:
            Synchronization element value, i.e., Hold phase extension, of
            clock zone `cz`.
        """

    def num_se(self) -> int:
        """
        Counts zones with a nonzero Hold-phase extension. @return
        Synchronization element count.
        """

class EvenRowCartesianGateLayout:
    """
    A gate layout with geometry, clocking, gates, wires, and obstructions. Gate creation takes and returns coordinates. Node inspection uses identifiers from get_node().
    """

    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(self, dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Standard constructor. Creates a named gate-level layout of the given
        aspect ratio. To this end, it calls `CoordinateLayout`'s standard
        constructor.

        Args:
            ar: Highest possible position in the layout.
            name: Layout name.
        """

    @overload
    def __init__(
        self,
        dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        clocking_scheme: str = "2DDWave",
        layout_name: str = "",
    ) -> None:
        """
        Standard constructor. Creates a gate-level layout of the given aspect
        ratio and clocks it via the given clocking scheme. To this end, it
        calls `CoordinateLayout`'s standard constructor.

        Args:
            ar: Highest possible position in the layout.
            scheme: Clocking scheme to apply to this layout.
            name: Layout name.
        """

    def coord(self, x: int, y: int, z: int = 0) -> coords.OffsetCoordinate:
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

    def resize(self, dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Updates the layout's dimensions, effectively resizing it.

        Args:
            ar: New aspect ratio.
        """

    def north(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def east(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def south(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def west(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def above(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
        """
        Returns the coordinate that is directly above a given coordinate `c`,
        i.e., the face whose z-dimension is higher by 1. If `c`'s z-dimension
        is already at maximum, `c` is returned instead.

        Args:
            c: Coordinate whose above counterpart is desired.

        Returns:
            Coordinate directly above `c`.
        """

    def below(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        c1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
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
        c1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
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

    def is_ground_layer(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in the ground layer
        where z is minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in ground layer.
        """

    def is_crossing_layer(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in a crossing layer
        where z is not minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in a crossing layer.
        """

    def is_within_bounds(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located within the layout
        bounds.

        Args:
            c: Coordinate to check for boundary.

        Returns:
            `true` iff `c` is located within the layout bounds.
        """

    def coordinates(self) -> list[coords.OffsetCoordinate]:
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

    def ground_coordinates(self) -> list[coords.OffsetCoordinate]:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[tuple[coords.OffsetCoordinate, coords.OffsetCoordinate]]:
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

    def assign_clock_number(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int], cn: int
    ) -> None:
        """
        Overrides the clock number of a tile in the stored scheme. The clock
        number applies to every layer of the tile, so the z-coordinate of `cz`
        is ignored.

        Args:
            cz: Clock zone to override.
            cn: New clock number for `cz`.
        """

    def get_clock_number(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the clock number of a tile. Every layer of a tile has the same
        clock number, so the z-coordinate of `cz` is ignored.

        Args:
            cz: Clock zone.

        Returns:
            Clock number of `cz`.
        """

    def num_clocks(self) -> int:
        """
        Returns the number of clock phases in the layout. Each clock cycle is
        divided into n phases. In QCA, the number of phases is usually 4. In
        iNML it is 3. Clocking schemes support 3 or 4 phases.

        Returns:
            The number of different clock signals in the layout.
        """

    def is_regularly_clocked(self) -> bool:
        """
        Returns whether the layout is clocked by a regular clocking scheme
        with no overwritten zones.

        Returns:
            `true` iff the layout is clocked by a regular scheme and no zones
            have been overwritten.
        """

    def is_clocking_scheme(self, name: str) -> bool:
        """
        Compares the stored clocking scheme against the provided name.
        Predefined names are constants in `fiction::layouts::clocking`.

        Args:
            name: Clocking scheme name.

        Returns:
            `true` iff the layout is clocked by a clocking scheme of name
            `name`.
        """

    def get_clocking_scheme_name(self) -> str:
        """
        Returns the name of the layout's clocking scheme, e.g., `2DDWave` or `USE`.
        """

    def is_incoming_clocked(
        self,
        cz1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Evaluates whether clock zone `cz2` feeds information to clock zone
        `cz1`, i.e., whether `cz2` is clocked with a clock number that is
        lower by 1 modulo `num_clocks()`, or either zone is a synchronization
        element.

        Args:
            cz1: Base clock zone.
            cz2: Clock zone to check whether its clock number is lower by 1.

        Returns:
            `true` iff `cz2` can feed information to `cz1`.
        """

    def is_outgoing_clocked(
        self,
        cz1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Evaluates whether clock zone `cz2` accepts information from clock zone
        `cz1`, i.e., whether `cz2` is clocked with a clock number that is
        higher by 1 modulo `num_clocks()`, or either zone is a synchronization
        element.

        Args:
            cz1: Base clock zone.
            cz2: Clock zone to check whether its clock number is higher by 1.

        Returns:
            `true` iff `cz2` can accept information from `cz1`.
        """

    def incoming_clocked_zones(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
        """
        Returns a container with all clock zones that are incoming to the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are incoming to `cz`.
        """

    def outgoing_clocked_zones(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
        """
        Returns a container with all clock zones that are outgoing from the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are outgoing from `cz`.
        """

    def in_degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of incoming clock zones to the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s incoming clock zones.
        """

    def out_degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of outgoing clock zones from the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s outgoing clock zones.
        """

    def degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of distinct incoming or outgoing neighboring clock
        zones.

        Args:
            cz: Base clock zone.

        Returns:
            Number of distinct clocked neighbors of `cz`.
        """

    def replace_clocking_scheme(self, name: str) -> None:
        """
        Replaces the clocking scheme by the predefined scheme of the given name. Clock-number overrides are discarded; synchronization elements are kept. Raises ValueError for an unknown name.
        """

    def obstruct_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Marks the given coordinate as obstructed.

        Args:
            c: clock_zone to obstruct.
        """

    def obstruct_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Marks the connection from coordinate `src` to coordinate `tgt` as
        obstructed.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Note:
            clock_zones marked this way will not be crossed with wires by path
            finding algorithms.
        """

    def clear_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Clears the obstruction status of the given coordinate `c` if the
        obstruction was manually marked via `obstruct_coordinate`.

        Args:
            c: clock_zone to clear.
        """

    def clear_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Clears the obstruction status of the connection from coordinate `src`
        to coordinate `tgt` if the obstruction was manually marked via
        `obstruct_connection`.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.
        """

    def clear_obstructed_coordinates(self) -> None:
        """
        Clears all obstructed coordinates that were manually marked via
        `obstruct_coordinate`.
        """

    def clear_obstructed_connections(self) -> None:
        """
        Clears all obstructed connections that were manually marked via
        `obstruct_connection`.
        """

    def is_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks if the given coordinate is obstructed of some sort.

        Args:
            c: Coordinate to check.

        Returns:
            `true` iff `c` is obstructed.
        """

    def is_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks if the given coordinate-coordinate connection is obstructed of
        some sort.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Returns:
            `true` iff the connection from `src` to `tgt` is obstructed.
        """

    def create_pi(
        self, name: str = "", t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...
    ) -> coords.OffsetCoordinate: ...
    def create_po(
        self,
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        name: str = "",
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def is_pi(self, n: int) -> bool:
        """
        Check whether `n` is a primary input.

        Args:
            n: Node to be checked.

        Returns:
            `true` iff `n` is a PI.
        """

    def is_po(self, n: int) -> bool:
        """
        Check whether `n` is a primary output.

        Args:
            n: Node to be checked.

        Returns:
            `true` iff `n` is a PO.
        """

    def is_pi_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary input.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PI.
        """

    def is_po_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary output.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PO.
        """

    def is_inv(self, arg: int, /) -> bool:
        """
        Returns whether `n` computes the binary inversion (NOT gate).

        Args:
            n: Node to check.

        Returns:
            `true` iff `n` is a NOT gate.
        """

    def is_and(self, arg: int, /) -> bool: ...
    def is_nand(self, arg: int, /) -> bool: ...
    def is_or(self, arg: int, /) -> bool: ...
    def is_nor(self, arg: int, /) -> bool: ...
    def is_xor(self, arg: int, /) -> bool: ...
    def is_xnor(self, arg: int, /) -> bool: ...
    def is_lt(self, arg: int, /) -> bool: ...
    def is_le(self, arg: int, /) -> bool: ...
    def is_gt(self, arg: int, /) -> bool: ...
    def is_ge(self, arg: int, /) -> bool: ...
    def is_maj(self, arg: int, /) -> bool: ...
    def is_fanout(self, arg: int, /) -> bool:
        """
        Returns whether `n` is a wire and has multiple outputs, thereby,
        acting as a fanout gate. Note that a fanout will return `true` for
        both `is_wire` and `is_fanout`.

        Args:
            n: Node to check.

        Returns:
            `true` iff `n` is a fanout gate.
        """

    def is_wire(self, arg: int, /) -> bool:
        """Equivalent to `is_buf`."""

    def set_layout_name(self, name: str) -> None: ...
    @property
    def name(self) -> str:
        """The layout name."""

    @name.setter
    def name(self, arg: str, /) -> None: ...
    def get_layout_name(self) -> str: ...
    def clone(self) -> EvenRowCartesianGateLayout:
        """
        Clones the layout returning a deep copy.

        Returns:
            Deep copy of the layout.
        """

    def set_input_name(self, index: int, name: str) -> None: ...
    def get_input_name(self, index: int) -> str: ...
    def set_output_name(self, index: int, name: str) -> None: ...
    def get_output_name(self, index: int) -> str: ...
    def get_name(self, s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> str: ...
    def create_buf(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_not(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_and(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_nand(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_or(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_nor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_xor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_xnor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_lt(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_le(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_gt(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_ge(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_maj(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def num_pis(self) -> int: ...
    def num_pos(self) -> int: ...
    def num_gates(self) -> int:
        """
        Returns the number of placed nodes in the layout that do not compute
        the identity function.

        Returns:
            Number of gates in the layout.
        """

    def num_wires(self) -> int:
        """
        Returns the number of placed nodes in the layout that compute the
        identity function including PIs and POs.

        Returns:
            Number of wires in the layout.
        """

    def num_crossings(self) -> int:
        """
        Returns the number of placed nodes in the layout that compute the
        identity function and cross other nodes.

        Returns:
            Number of crossings in the layout.
        """

    def is_empty(self) -> bool:
        """
        Checks whether there are no gates or wires assigned to the layout's
        coordinates.

        Returns:
            `true` iff the layout is empty.
        """

    def fanin_size(self, n: int) -> int:
        """
        Returns the number of incoming, adjacently placed, and properly
        clocked signals to the given node.

        Args:
            n: Node to check.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            Number of fanins to `n`.
        """

    def fanout_size(self, n: int) -> int:
        """
        Returns the number of outgoing, adjacently placed, and properly
        clocked signals of the given node.

        Args:
            n: Node to check.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            Number of fanouts to `n`.
        """

    def get_node(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Fetches the node that is placed onto a tile pointed to by a given
        signal. If no node is placed there, the `const0` node is returned.

        Args:
            s: Pointer to a tile.

        Returns:
            Node at position `t` where `s` points at `t`; or 0 if no node is
            placed at `t`.
        """

    def get_tile(self, n: int) -> coords.OffsetCoordinate:
        """
        The inverse function of `get_node`. Fetches the tile that the provided
        node is placed on. Returns a default dead tile if the node is not
        placed.

        Args:
            n: Node whose location is desired.

        Returns:
            Tile at which `n` is placed or a default dead tile if `n` is not
            placed.
        """

    def make_signal(self, n: int) -> coords.OffsetCoordinate:
        """
        Invokes the same behavior as `get_tile(n)` but additionally casts the
        return value to a signal. That is, this function returns the signal
        representation of the tile that the node `n` is assigned to.

        Args:
            n: Node whose signal is desired.

        Returns:
            Signal that points to `n`.
        """

    def move_node(
        self,
        n: int,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        new_children: Sequence[coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]] = [],
    ) -> coords.OffsetCoordinate:
        """
        Moves a given node to a new position and also updates its children,
        i.e., incoming signals.

        Args:
            n: Node to move.
            t: Tile to move `n` to.
            new_children: New incoming signals to `n`.

        Returns:
            Signal pointing to `n`'s new tile.
        """

    def clear_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Removes all assigned nodes from the given tile and marks them as dead.

        Args:
            t: Tile whose nodes are to be removed.

        Note:
            This function does not reduce the number of nodes in the layout
            nor does it reduce the number of PIs that are being returned via
            `num_pis()` even if the tile to clear is an input tile. However,
            the number of POs is reduced if the tile to clear is an output
            tile. While this seems counter-intuitive and inconsistent, it is
            in line with mockturtle's understanding of nodes and primary
            outputs.
        """

    def is_gate_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_gate` (in
        accordance with `mockturtle`'s definition of gates).

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that is a neither a constant nor a PI.
        """

    def is_wire_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_wire`.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that computes the identity.
        """

    def is_empty_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether `t` does not have a node assigned to it.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` is an empty tile.
        """

    def pis(self) -> list[coords.OffsetCoordinate]: ...
    def pos(self) -> list[coords.OffsetCoordinate]: ...
    def gates(self) -> list[coords.OffsetCoordinate]: ...
    def wires(self) -> list[coords.OffsetCoordinate]: ...
    def fanins(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]: ...
    def fanouts(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]: ...
    def is_incoming_signal(
        self,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks whether signal `s` is incoming to tile `t`. That is, whether
        tile `t` hosts a node that has a fanin assigned to the tile that
        signal `s` points to.

        Args:
            t: Base tile.
            s: Signal pointing to a potential incoming tile to `t`.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `s` is incoming to `t`.
        """

    def has_no_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has no incoming tiles.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `t` does not have incoming tiles.
        """

    def has_northern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in northern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north(t)` is incoming to `t`.
        """

    def has_north_eastern_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in north-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north_east(t)` is incoming to `t`.
        """

    def has_eastern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `east(t)` is incoming to `t`.
        """

    def has_south_eastern_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in south-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south_east(t)` is incoming to `t`.
        """

    def has_southern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in southern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south(t)` is incoming to `t`.
        """

    def has_south_western_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in south-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south_west(t)` is incoming to `t`.
        """

    def has_western_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `west(t)` is incoming to `t`.
        """

    def has_north_western_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in north-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north_west(t)` is incoming to `t`.
        """

    def is_outgoing_signal(
        self,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks whether signal `s` is outgoing from tile `t`. That is, whether
        tile `t` hosts a node that has a fanout assigned to the tile that
        signal `s` points to.

        Args:
            t: Base tile.
            s: Signal pointing to a potential outgoing tile of `t`.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `s` is outgoing from `t`.
        """

    def has_no_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has no outgoing tiles.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `t` does not have outgoing tiles.
        """

    def has_northern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in northern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north(t)` is outgoing from `t`.
        """

    def has_north_eastern_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in north-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north_east(t)` is outgoing from `t`.
        """

    def has_eastern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `east(t)` is outgoing from `t`.
        """

    def has_south_eastern_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in south-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south_east(t)` is outgoing from `t`.
        """

    def has_southern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in southern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south(t)` is outgoing from `t`.
        """

    def has_south_western_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in south-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south_west(t)` is outgoing from `t`.
        """

    def has_western_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `west(t)` is outgoing from `t`.
        """

    def has_north_western_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in north-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north_west(t)` is outgoing from `t`.
        """

    def bounding_box_2d(self) -> tuple[coords.OffsetCoordinate, coords.OffsetCoordinate]:
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

    def is_dead(self, n: int) -> bool:
        """
        Checks whether a node (not its assigned tile) is dead. Nodes can be
        dead for a variety of reasons. For instance if they are dangling (see
        the `mockturtle` API). In this layout type, nodes are also marked dead
        when they are not assigned to a tile (which is considered equivalent
        to dangling).

        Args:
            n: Node to check for liveliness.

        Returns:
            `true` iff `n` is dead.
        """

    def assign_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int], delay: int
    ) -> None:
        """
        Assigns a synchronization element to the provided clock zone.

        Args:
            cz: Clock zone to turn into a synchronization element.
            se: Number of full clock cycles to extend `cz`'s Hold phase by. If
                this value is 0, `cz` is turned back into a normal clock zone.
        """

    def is_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Check whether the provided clock zone is a synchronization element.

        Args:
            cz: Clock zone to check.

        Returns:
            `true` iff `cz` is a synchronization element.
        """

    def get_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> int:
        """
        Returns the Hold phase extension in clock cycles of clock zone `cz`.

        Args:
            cz: Clock zone to check.

        Returns:
            Synchronization element value, i.e., Hold phase extension, of
            clock zone `cz`.
        """

    def num_se(self) -> int:
        """
        Counts zones with a nonzero Hold-phase extension. @return
        Synchronization element count.
        """

class EvenColumnCartesianGateLayout:
    """
    A gate layout with geometry, clocking, gates, wires, and obstructions. Gate creation takes and returns coordinates. Node inspection uses identifiers from get_node().
    """

    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(self, dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Standard constructor. Creates a named gate-level layout of the given
        aspect ratio. To this end, it calls `CoordinateLayout`'s standard
        constructor.

        Args:
            ar: Highest possible position in the layout.
            name: Layout name.
        """

    @overload
    def __init__(
        self,
        dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        clocking_scheme: str = "2DDWave",
        layout_name: str = "",
    ) -> None:
        """
        Standard constructor. Creates a gate-level layout of the given aspect
        ratio and clocks it via the given clocking scheme. To this end, it
        calls `CoordinateLayout`'s standard constructor.

        Args:
            ar: Highest possible position in the layout.
            scheme: Clocking scheme to apply to this layout.
            name: Layout name.
        """

    def coord(self, x: int, y: int, z: int = 0) -> coords.OffsetCoordinate:
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

    def resize(self, dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Updates the layout's dimensions, effectively resizing it.

        Args:
            ar: New aspect ratio.
        """

    def north(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def east(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def south(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def west(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def above(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
        """
        Returns the coordinate that is directly above a given coordinate `c`,
        i.e., the face whose z-dimension is higher by 1. If `c`'s z-dimension
        is already at maximum, `c` is returned instead.

        Args:
            c: Coordinate whose above counterpart is desired.

        Returns:
            Coordinate directly above `c`.
        """

    def below(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        c1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
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
        c1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
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

    def is_ground_layer(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in the ground layer
        where z is minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in ground layer.
        """

    def is_crossing_layer(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in a crossing layer
        where z is not minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in a crossing layer.
        """

    def is_within_bounds(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located within the layout
        bounds.

        Args:
            c: Coordinate to check for boundary.

        Returns:
            `true` iff `c` is located within the layout bounds.
        """

    def coordinates(self) -> list[coords.OffsetCoordinate]:
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

    def ground_coordinates(self) -> list[coords.OffsetCoordinate]:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[tuple[coords.OffsetCoordinate, coords.OffsetCoordinate]]:
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

    def assign_clock_number(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int], cn: int
    ) -> None:
        """
        Overrides the clock number of a tile in the stored scheme. The clock
        number applies to every layer of the tile, so the z-coordinate of `cz`
        is ignored.

        Args:
            cz: Clock zone to override.
            cn: New clock number for `cz`.
        """

    def get_clock_number(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the clock number of a tile. Every layer of a tile has the same
        clock number, so the z-coordinate of `cz` is ignored.

        Args:
            cz: Clock zone.

        Returns:
            Clock number of `cz`.
        """

    def num_clocks(self) -> int:
        """
        Returns the number of clock phases in the layout. Each clock cycle is
        divided into n phases. In QCA, the number of phases is usually 4. In
        iNML it is 3. Clocking schemes support 3 or 4 phases.

        Returns:
            The number of different clock signals in the layout.
        """

    def is_regularly_clocked(self) -> bool:
        """
        Returns whether the layout is clocked by a regular clocking scheme
        with no overwritten zones.

        Returns:
            `true` iff the layout is clocked by a regular scheme and no zones
            have been overwritten.
        """

    def is_clocking_scheme(self, name: str) -> bool:
        """
        Compares the stored clocking scheme against the provided name.
        Predefined names are constants in `fiction::layouts::clocking`.

        Args:
            name: Clocking scheme name.

        Returns:
            `true` iff the layout is clocked by a clocking scheme of name
            `name`.
        """

    def get_clocking_scheme_name(self) -> str:
        """
        Returns the name of the layout's clocking scheme, e.g., `2DDWave` or `USE`.
        """

    def is_incoming_clocked(
        self,
        cz1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Evaluates whether clock zone `cz2` feeds information to clock zone
        `cz1`, i.e., whether `cz2` is clocked with a clock number that is
        lower by 1 modulo `num_clocks()`, or either zone is a synchronization
        element.

        Args:
            cz1: Base clock zone.
            cz2: Clock zone to check whether its clock number is lower by 1.

        Returns:
            `true` iff `cz2` can feed information to `cz1`.
        """

    def is_outgoing_clocked(
        self,
        cz1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Evaluates whether clock zone `cz2` accepts information from clock zone
        `cz1`, i.e., whether `cz2` is clocked with a clock number that is
        higher by 1 modulo `num_clocks()`, or either zone is a synchronization
        element.

        Args:
            cz1: Base clock zone.
            cz2: Clock zone to check whether its clock number is higher by 1.

        Returns:
            `true` iff `cz2` can accept information from `cz1`.
        """

    def incoming_clocked_zones(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
        """
        Returns a container with all clock zones that are incoming to the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are incoming to `cz`.
        """

    def outgoing_clocked_zones(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
        """
        Returns a container with all clock zones that are outgoing from the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are outgoing from `cz`.
        """

    def in_degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of incoming clock zones to the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s incoming clock zones.
        """

    def out_degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of outgoing clock zones from the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s outgoing clock zones.
        """

    def degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of distinct incoming or outgoing neighboring clock
        zones.

        Args:
            cz: Base clock zone.

        Returns:
            Number of distinct clocked neighbors of `cz`.
        """

    def replace_clocking_scheme(self, name: str) -> None:
        """
        Replaces the clocking scheme by the predefined scheme of the given name. Clock-number overrides are discarded; synchronization elements are kept. Raises ValueError for an unknown name.
        """

    def obstruct_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Marks the given coordinate as obstructed.

        Args:
            c: clock_zone to obstruct.
        """

    def obstruct_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Marks the connection from coordinate `src` to coordinate `tgt` as
        obstructed.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Note:
            clock_zones marked this way will not be crossed with wires by path
            finding algorithms.
        """

    def clear_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Clears the obstruction status of the given coordinate `c` if the
        obstruction was manually marked via `obstruct_coordinate`.

        Args:
            c: clock_zone to clear.
        """

    def clear_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Clears the obstruction status of the connection from coordinate `src`
        to coordinate `tgt` if the obstruction was manually marked via
        `obstruct_connection`.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.
        """

    def clear_obstructed_coordinates(self) -> None:
        """
        Clears all obstructed coordinates that were manually marked via
        `obstruct_coordinate`.
        """

    def clear_obstructed_connections(self) -> None:
        """
        Clears all obstructed connections that were manually marked via
        `obstruct_connection`.
        """

    def is_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks if the given coordinate is obstructed of some sort.

        Args:
            c: Coordinate to check.

        Returns:
            `true` iff `c` is obstructed.
        """

    def is_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks if the given coordinate-coordinate connection is obstructed of
        some sort.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Returns:
            `true` iff the connection from `src` to `tgt` is obstructed.
        """

    def create_pi(
        self, name: str = "", t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...
    ) -> coords.OffsetCoordinate: ...
    def create_po(
        self,
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        name: str = "",
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def is_pi(self, n: int) -> bool:
        """
        Check whether `n` is a primary input.

        Args:
            n: Node to be checked.

        Returns:
            `true` iff `n` is a PI.
        """

    def is_po(self, n: int) -> bool:
        """
        Check whether `n` is a primary output.

        Args:
            n: Node to be checked.

        Returns:
            `true` iff `n` is a PO.
        """

    def is_pi_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary input.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PI.
        """

    def is_po_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary output.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PO.
        """

    def is_inv(self, arg: int, /) -> bool:
        """
        Returns whether `n` computes the binary inversion (NOT gate).

        Args:
            n: Node to check.

        Returns:
            `true` iff `n` is a NOT gate.
        """

    def is_and(self, arg: int, /) -> bool: ...
    def is_nand(self, arg: int, /) -> bool: ...
    def is_or(self, arg: int, /) -> bool: ...
    def is_nor(self, arg: int, /) -> bool: ...
    def is_xor(self, arg: int, /) -> bool: ...
    def is_xnor(self, arg: int, /) -> bool: ...
    def is_lt(self, arg: int, /) -> bool: ...
    def is_le(self, arg: int, /) -> bool: ...
    def is_gt(self, arg: int, /) -> bool: ...
    def is_ge(self, arg: int, /) -> bool: ...
    def is_maj(self, arg: int, /) -> bool: ...
    def is_fanout(self, arg: int, /) -> bool:
        """
        Returns whether `n` is a wire and has multiple outputs, thereby,
        acting as a fanout gate. Note that a fanout will return `true` for
        both `is_wire` and `is_fanout`.

        Args:
            n: Node to check.

        Returns:
            `true` iff `n` is a fanout gate.
        """

    def is_wire(self, arg: int, /) -> bool:
        """Equivalent to `is_buf`."""

    def set_layout_name(self, name: str) -> None: ...
    @property
    def name(self) -> str:
        """The layout name."""

    @name.setter
    def name(self, arg: str, /) -> None: ...
    def get_layout_name(self) -> str: ...
    def clone(self) -> EvenColumnCartesianGateLayout:
        """
        Clones the layout returning a deep copy.

        Returns:
            Deep copy of the layout.
        """

    def set_input_name(self, index: int, name: str) -> None: ...
    def get_input_name(self, index: int) -> str: ...
    def set_output_name(self, index: int, name: str) -> None: ...
    def get_output_name(self, index: int) -> str: ...
    def get_name(self, s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> str: ...
    def create_buf(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_not(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_and(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_nand(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_or(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_nor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_xor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_xnor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_lt(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_le(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_gt(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_ge(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_maj(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def num_pis(self) -> int: ...
    def num_pos(self) -> int: ...
    def num_gates(self) -> int:
        """
        Returns the number of placed nodes in the layout that do not compute
        the identity function.

        Returns:
            Number of gates in the layout.
        """

    def num_wires(self) -> int:
        """
        Returns the number of placed nodes in the layout that compute the
        identity function including PIs and POs.

        Returns:
            Number of wires in the layout.
        """

    def num_crossings(self) -> int:
        """
        Returns the number of placed nodes in the layout that compute the
        identity function and cross other nodes.

        Returns:
            Number of crossings in the layout.
        """

    def is_empty(self) -> bool:
        """
        Checks whether there are no gates or wires assigned to the layout's
        coordinates.

        Returns:
            `true` iff the layout is empty.
        """

    def fanin_size(self, n: int) -> int:
        """
        Returns the number of incoming, adjacently placed, and properly
        clocked signals to the given node.

        Args:
            n: Node to check.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            Number of fanins to `n`.
        """

    def fanout_size(self, n: int) -> int:
        """
        Returns the number of outgoing, adjacently placed, and properly
        clocked signals of the given node.

        Args:
            n: Node to check.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            Number of fanouts to `n`.
        """

    def get_node(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Fetches the node that is placed onto a tile pointed to by a given
        signal. If no node is placed there, the `const0` node is returned.

        Args:
            s: Pointer to a tile.

        Returns:
            Node at position `t` where `s` points at `t`; or 0 if no node is
            placed at `t`.
        """

    def get_tile(self, n: int) -> coords.OffsetCoordinate:
        """
        The inverse function of `get_node`. Fetches the tile that the provided
        node is placed on. Returns a default dead tile if the node is not
        placed.

        Args:
            n: Node whose location is desired.

        Returns:
            Tile at which `n` is placed or a default dead tile if `n` is not
            placed.
        """

    def make_signal(self, n: int) -> coords.OffsetCoordinate:
        """
        Invokes the same behavior as `get_tile(n)` but additionally casts the
        return value to a signal. That is, this function returns the signal
        representation of the tile that the node `n` is assigned to.

        Args:
            n: Node whose signal is desired.

        Returns:
            Signal that points to `n`.
        """

    def move_node(
        self,
        n: int,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        new_children: Sequence[coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]] = [],
    ) -> coords.OffsetCoordinate:
        """
        Moves a given node to a new position and also updates its children,
        i.e., incoming signals.

        Args:
            n: Node to move.
            t: Tile to move `n` to.
            new_children: New incoming signals to `n`.

        Returns:
            Signal pointing to `n`'s new tile.
        """

    def clear_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Removes all assigned nodes from the given tile and marks them as dead.

        Args:
            t: Tile whose nodes are to be removed.

        Note:
            This function does not reduce the number of nodes in the layout
            nor does it reduce the number of PIs that are being returned via
            `num_pis()` even if the tile to clear is an input tile. However,
            the number of POs is reduced if the tile to clear is an output
            tile. While this seems counter-intuitive and inconsistent, it is
            in line with mockturtle's understanding of nodes and primary
            outputs.
        """

    def is_gate_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_gate` (in
        accordance with `mockturtle`'s definition of gates).

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that is a neither a constant nor a PI.
        """

    def is_wire_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_wire`.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that computes the identity.
        """

    def is_empty_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether `t` does not have a node assigned to it.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` is an empty tile.
        """

    def pis(self) -> list[coords.OffsetCoordinate]: ...
    def pos(self) -> list[coords.OffsetCoordinate]: ...
    def gates(self) -> list[coords.OffsetCoordinate]: ...
    def wires(self) -> list[coords.OffsetCoordinate]: ...
    def fanins(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]: ...
    def fanouts(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]: ...
    def is_incoming_signal(
        self,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks whether signal `s` is incoming to tile `t`. That is, whether
        tile `t` hosts a node that has a fanin assigned to the tile that
        signal `s` points to.

        Args:
            t: Base tile.
            s: Signal pointing to a potential incoming tile to `t`.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `s` is incoming to `t`.
        """

    def has_no_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has no incoming tiles.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `t` does not have incoming tiles.
        """

    def has_northern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in northern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north(t)` is incoming to `t`.
        """

    def has_north_eastern_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in north-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north_east(t)` is incoming to `t`.
        """

    def has_eastern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `east(t)` is incoming to `t`.
        """

    def has_south_eastern_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in south-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south_east(t)` is incoming to `t`.
        """

    def has_southern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in southern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south(t)` is incoming to `t`.
        """

    def has_south_western_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in south-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south_west(t)` is incoming to `t`.
        """

    def has_western_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `west(t)` is incoming to `t`.
        """

    def has_north_western_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in north-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north_west(t)` is incoming to `t`.
        """

    def is_outgoing_signal(
        self,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks whether signal `s` is outgoing from tile `t`. That is, whether
        tile `t` hosts a node that has a fanout assigned to the tile that
        signal `s` points to.

        Args:
            t: Base tile.
            s: Signal pointing to a potential outgoing tile of `t`.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `s` is outgoing from `t`.
        """

    def has_no_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has no outgoing tiles.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `t` does not have outgoing tiles.
        """

    def has_northern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in northern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north(t)` is outgoing from `t`.
        """

    def has_north_eastern_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in north-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north_east(t)` is outgoing from `t`.
        """

    def has_eastern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `east(t)` is outgoing from `t`.
        """

    def has_south_eastern_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in south-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south_east(t)` is outgoing from `t`.
        """

    def has_southern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in southern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south(t)` is outgoing from `t`.
        """

    def has_south_western_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in south-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south_west(t)` is outgoing from `t`.
        """

    def has_western_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `west(t)` is outgoing from `t`.
        """

    def has_north_western_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in north-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north_west(t)` is outgoing from `t`.
        """

    def bounding_box_2d(self) -> tuple[coords.OffsetCoordinate, coords.OffsetCoordinate]:
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

    def is_dead(self, n: int) -> bool:
        """
        Checks whether a node (not its assigned tile) is dead. Nodes can be
        dead for a variety of reasons. For instance if they are dangling (see
        the `mockturtle` API). In this layout type, nodes are also marked dead
        when they are not assigned to a tile (which is considered equivalent
        to dangling).

        Args:
            n: Node to check for liveliness.

        Returns:
            `true` iff `n` is dead.
        """

    def assign_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int], delay: int
    ) -> None:
        """
        Assigns a synchronization element to the provided clock zone.

        Args:
            cz: Clock zone to turn into a synchronization element.
            se: Number of full clock cycles to extend `cz`'s Hold phase by. If
                this value is 0, `cz` is turned back into a normal clock zone.
        """

    def is_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Check whether the provided clock zone is a synchronization element.

        Args:
            cz: Clock zone to check.

        Returns:
            `true` iff `cz` is a synchronization element.
        """

    def get_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> int:
        """
        Returns the Hold phase extension in clock cycles of clock zone `cz`.

        Args:
            cz: Clock zone to check.

        Returns:
            Synchronization element value, i.e., Hold phase extension, of
            clock zone `cz`.
        """

    def num_se(self) -> int:
        """
        Counts zones with a nonzero Hold-phase extension. @return
        Synchronization element count.
        """

class OddRowHexGateLayout:
    """
    A gate layout with geometry, clocking, gates, wires, and obstructions. Gate creation takes and returns coordinates. Node inspection uses identifiers from get_node().
    """

    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(self, dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Standard constructor. Creates a named gate-level layout of the given
        aspect ratio. To this end, it calls `CoordinateLayout`'s standard
        constructor.

        Args:
            ar: Highest possible position in the layout.
            name: Layout name.
        """

    @overload
    def __init__(
        self,
        dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        clocking_scheme: str = "2DDWave",
        layout_name: str = "",
    ) -> None:
        """
        Standard constructor. Creates a gate-level layout of the given aspect
        ratio and clocks it via the given clocking scheme. To this end, it
        calls `CoordinateLayout`'s standard constructor.

        Args:
            ar: Highest possible position in the layout.
            scheme: Clocking scheme to apply to this layout.
            name: Layout name.
        """

    def coord(self, x: int, y: int, z: int = 0) -> coords.OffsetCoordinate:
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

    def resize(self, dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Updates the layout's dimensions, effectively resizing it.

        Args:
            ar: New aspect ratio.
        """

    def north(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def east(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def south(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def west(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def above(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
        """
        Returns the coordinate that is directly above a given coordinate `c`,
        i.e., the face whose z-dimension is higher by 1. If `c`'s z-dimension
        is already at maximum, `c` is returned instead.

        Args:
            c: Coordinate whose above counterpart is desired.

        Returns:
            Coordinate directly above `c`.
        """

    def below(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        c1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
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
        c1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
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

    def is_ground_layer(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in the ground layer
        where z is minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in ground layer.
        """

    def is_crossing_layer(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in a crossing layer
        where z is not minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in a crossing layer.
        """

    def is_within_bounds(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located within the layout
        bounds.

        Args:
            c: Coordinate to check for boundary.

        Returns:
            `true` iff `c` is located within the layout bounds.
        """

    def coordinates(self) -> list[coords.OffsetCoordinate]:
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

    def ground_coordinates(self) -> list[coords.OffsetCoordinate]:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[tuple[coords.OffsetCoordinate, coords.OffsetCoordinate]]:
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

    def assign_clock_number(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int], cn: int
    ) -> None:
        """
        Overrides the clock number of a tile in the stored scheme. The clock
        number applies to every layer of the tile, so the z-coordinate of `cz`
        is ignored.

        Args:
            cz: Clock zone to override.
            cn: New clock number for `cz`.
        """

    def get_clock_number(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the clock number of a tile. Every layer of a tile has the same
        clock number, so the z-coordinate of `cz` is ignored.

        Args:
            cz: Clock zone.

        Returns:
            Clock number of `cz`.
        """

    def num_clocks(self) -> int:
        """
        Returns the number of clock phases in the layout. Each clock cycle is
        divided into n phases. In QCA, the number of phases is usually 4. In
        iNML it is 3. Clocking schemes support 3 or 4 phases.

        Returns:
            The number of different clock signals in the layout.
        """

    def is_regularly_clocked(self) -> bool:
        """
        Returns whether the layout is clocked by a regular clocking scheme
        with no overwritten zones.

        Returns:
            `true` iff the layout is clocked by a regular scheme and no zones
            have been overwritten.
        """

    def is_clocking_scheme(self, name: str) -> bool:
        """
        Compares the stored clocking scheme against the provided name.
        Predefined names are constants in `fiction::layouts::clocking`.

        Args:
            name: Clocking scheme name.

        Returns:
            `true` iff the layout is clocked by a clocking scheme of name
            `name`.
        """

    def get_clocking_scheme_name(self) -> str:
        """
        Returns the name of the layout's clocking scheme, e.g., `2DDWave` or `USE`.
        """

    def is_incoming_clocked(
        self,
        cz1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Evaluates whether clock zone `cz2` feeds information to clock zone
        `cz1`, i.e., whether `cz2` is clocked with a clock number that is
        lower by 1 modulo `num_clocks()`, or either zone is a synchronization
        element.

        Args:
            cz1: Base clock zone.
            cz2: Clock zone to check whether its clock number is lower by 1.

        Returns:
            `true` iff `cz2` can feed information to `cz1`.
        """

    def is_outgoing_clocked(
        self,
        cz1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Evaluates whether clock zone `cz2` accepts information from clock zone
        `cz1`, i.e., whether `cz2` is clocked with a clock number that is
        higher by 1 modulo `num_clocks()`, or either zone is a synchronization
        element.

        Args:
            cz1: Base clock zone.
            cz2: Clock zone to check whether its clock number is higher by 1.

        Returns:
            `true` iff `cz2` can accept information from `cz1`.
        """

    def incoming_clocked_zones(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
        """
        Returns a container with all clock zones that are incoming to the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are incoming to `cz`.
        """

    def outgoing_clocked_zones(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
        """
        Returns a container with all clock zones that are outgoing from the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are outgoing from `cz`.
        """

    def in_degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of incoming clock zones to the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s incoming clock zones.
        """

    def out_degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of outgoing clock zones from the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s outgoing clock zones.
        """

    def degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of distinct incoming or outgoing neighboring clock
        zones.

        Args:
            cz: Base clock zone.

        Returns:
            Number of distinct clocked neighbors of `cz`.
        """

    def replace_clocking_scheme(self, name: str) -> None:
        """
        Replaces the clocking scheme by the predefined scheme of the given name. Clock-number overrides are discarded; synchronization elements are kept. Raises ValueError for an unknown name.
        """

    def obstruct_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Marks the given coordinate as obstructed.

        Args:
            c: clock_zone to obstruct.
        """

    def obstruct_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Marks the connection from coordinate `src` to coordinate `tgt` as
        obstructed.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Note:
            clock_zones marked this way will not be crossed with wires by path
            finding algorithms.
        """

    def clear_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Clears the obstruction status of the given coordinate `c` if the
        obstruction was manually marked via `obstruct_coordinate`.

        Args:
            c: clock_zone to clear.
        """

    def clear_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Clears the obstruction status of the connection from coordinate `src`
        to coordinate `tgt` if the obstruction was manually marked via
        `obstruct_connection`.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.
        """

    def clear_obstructed_coordinates(self) -> None:
        """
        Clears all obstructed coordinates that were manually marked via
        `obstruct_coordinate`.
        """

    def clear_obstructed_connections(self) -> None:
        """
        Clears all obstructed connections that were manually marked via
        `obstruct_connection`.
        """

    def is_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks if the given coordinate is obstructed of some sort.

        Args:
            c: Coordinate to check.

        Returns:
            `true` iff `c` is obstructed.
        """

    def is_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks if the given coordinate-coordinate connection is obstructed of
        some sort.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Returns:
            `true` iff the connection from `src` to `tgt` is obstructed.
        """

    def create_pi(
        self, name: str = "", t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...
    ) -> coords.OffsetCoordinate: ...
    def create_po(
        self,
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        name: str = "",
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def is_pi(self, n: int) -> bool:
        """
        Check whether `n` is a primary input.

        Args:
            n: Node to be checked.

        Returns:
            `true` iff `n` is a PI.
        """

    def is_po(self, n: int) -> bool:
        """
        Check whether `n` is a primary output.

        Args:
            n: Node to be checked.

        Returns:
            `true` iff `n` is a PO.
        """

    def is_pi_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary input.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PI.
        """

    def is_po_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary output.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PO.
        """

    def is_inv(self, arg: int, /) -> bool:
        """
        Returns whether `n` computes the binary inversion (NOT gate).

        Args:
            n: Node to check.

        Returns:
            `true` iff `n` is a NOT gate.
        """

    def is_and(self, arg: int, /) -> bool: ...
    def is_nand(self, arg: int, /) -> bool: ...
    def is_or(self, arg: int, /) -> bool: ...
    def is_nor(self, arg: int, /) -> bool: ...
    def is_xor(self, arg: int, /) -> bool: ...
    def is_xnor(self, arg: int, /) -> bool: ...
    def is_lt(self, arg: int, /) -> bool: ...
    def is_le(self, arg: int, /) -> bool: ...
    def is_gt(self, arg: int, /) -> bool: ...
    def is_ge(self, arg: int, /) -> bool: ...
    def is_maj(self, arg: int, /) -> bool: ...
    def is_fanout(self, arg: int, /) -> bool:
        """
        Returns whether `n` is a wire and has multiple outputs, thereby,
        acting as a fanout gate. Note that a fanout will return `true` for
        both `is_wire` and `is_fanout`.

        Args:
            n: Node to check.

        Returns:
            `true` iff `n` is a fanout gate.
        """

    def is_wire(self, arg: int, /) -> bool:
        """Equivalent to `is_buf`."""

    def set_layout_name(self, name: str) -> None: ...
    @property
    def name(self) -> str:
        """The layout name."""

    @name.setter
    def name(self, arg: str, /) -> None: ...
    def get_layout_name(self) -> str: ...
    def clone(self) -> OddRowHexGateLayout:
        """
        Clones the layout returning a deep copy.

        Returns:
            Deep copy of the layout.
        """

    def set_input_name(self, index: int, name: str) -> None: ...
    def get_input_name(self, index: int) -> str: ...
    def set_output_name(self, index: int, name: str) -> None: ...
    def get_output_name(self, index: int) -> str: ...
    def get_name(self, s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> str: ...
    def create_buf(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_not(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_and(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_nand(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_or(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_nor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_xor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_xnor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_lt(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_le(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_gt(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_ge(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_maj(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def num_pis(self) -> int: ...
    def num_pos(self) -> int: ...
    def num_gates(self) -> int:
        """
        Returns the number of placed nodes in the layout that do not compute
        the identity function.

        Returns:
            Number of gates in the layout.
        """

    def num_wires(self) -> int:
        """
        Returns the number of placed nodes in the layout that compute the
        identity function including PIs and POs.

        Returns:
            Number of wires in the layout.
        """

    def num_crossings(self) -> int:
        """
        Returns the number of placed nodes in the layout that compute the
        identity function and cross other nodes.

        Returns:
            Number of crossings in the layout.
        """

    def is_empty(self) -> bool:
        """
        Checks whether there are no gates or wires assigned to the layout's
        coordinates.

        Returns:
            `true` iff the layout is empty.
        """

    def fanin_size(self, n: int) -> int:
        """
        Returns the number of incoming, adjacently placed, and properly
        clocked signals to the given node.

        Args:
            n: Node to check.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            Number of fanins to `n`.
        """

    def fanout_size(self, n: int) -> int:
        """
        Returns the number of outgoing, adjacently placed, and properly
        clocked signals of the given node.

        Args:
            n: Node to check.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            Number of fanouts to `n`.
        """

    def get_node(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Fetches the node that is placed onto a tile pointed to by a given
        signal. If no node is placed there, the `const0` node is returned.

        Args:
            s: Pointer to a tile.

        Returns:
            Node at position `t` where `s` points at `t`; or 0 if no node is
            placed at `t`.
        """

    def get_tile(self, n: int) -> coords.OffsetCoordinate:
        """
        The inverse function of `get_node`. Fetches the tile that the provided
        node is placed on. Returns a default dead tile if the node is not
        placed.

        Args:
            n: Node whose location is desired.

        Returns:
            Tile at which `n` is placed or a default dead tile if `n` is not
            placed.
        """

    def make_signal(self, n: int) -> coords.OffsetCoordinate:
        """
        Invokes the same behavior as `get_tile(n)` but additionally casts the
        return value to a signal. That is, this function returns the signal
        representation of the tile that the node `n` is assigned to.

        Args:
            n: Node whose signal is desired.

        Returns:
            Signal that points to `n`.
        """

    def move_node(
        self,
        n: int,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        new_children: Sequence[coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]] = [],
    ) -> coords.OffsetCoordinate:
        """
        Moves a given node to a new position and also updates its children,
        i.e., incoming signals.

        Args:
            n: Node to move.
            t: Tile to move `n` to.
            new_children: New incoming signals to `n`.

        Returns:
            Signal pointing to `n`'s new tile.
        """

    def clear_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Removes all assigned nodes from the given tile and marks them as dead.

        Args:
            t: Tile whose nodes are to be removed.

        Note:
            This function does not reduce the number of nodes in the layout
            nor does it reduce the number of PIs that are being returned via
            `num_pis()` even if the tile to clear is an input tile. However,
            the number of POs is reduced if the tile to clear is an output
            tile. While this seems counter-intuitive and inconsistent, it is
            in line with mockturtle's understanding of nodes and primary
            outputs.
        """

    def is_gate_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_gate` (in
        accordance with `mockturtle`'s definition of gates).

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that is a neither a constant nor a PI.
        """

    def is_wire_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_wire`.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that computes the identity.
        """

    def is_empty_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether `t` does not have a node assigned to it.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` is an empty tile.
        """

    def pis(self) -> list[coords.OffsetCoordinate]: ...
    def pos(self) -> list[coords.OffsetCoordinate]: ...
    def gates(self) -> list[coords.OffsetCoordinate]: ...
    def wires(self) -> list[coords.OffsetCoordinate]: ...
    def fanins(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]: ...
    def fanouts(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]: ...
    def is_incoming_signal(
        self,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks whether signal `s` is incoming to tile `t`. That is, whether
        tile `t` hosts a node that has a fanin assigned to the tile that
        signal `s` points to.

        Args:
            t: Base tile.
            s: Signal pointing to a potential incoming tile to `t`.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `s` is incoming to `t`.
        """

    def has_no_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has no incoming tiles.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `t` does not have incoming tiles.
        """

    def has_northern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in northern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north(t)` is incoming to `t`.
        """

    def has_north_eastern_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in north-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north_east(t)` is incoming to `t`.
        """

    def has_eastern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `east(t)` is incoming to `t`.
        """

    def has_south_eastern_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in south-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south_east(t)` is incoming to `t`.
        """

    def has_southern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in southern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south(t)` is incoming to `t`.
        """

    def has_south_western_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in south-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south_west(t)` is incoming to `t`.
        """

    def has_western_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `west(t)` is incoming to `t`.
        """

    def has_north_western_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in north-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north_west(t)` is incoming to `t`.
        """

    def is_outgoing_signal(
        self,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks whether signal `s` is outgoing from tile `t`. That is, whether
        tile `t` hosts a node that has a fanout assigned to the tile that
        signal `s` points to.

        Args:
            t: Base tile.
            s: Signal pointing to a potential outgoing tile of `t`.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `s` is outgoing from `t`.
        """

    def has_no_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has no outgoing tiles.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `t` does not have outgoing tiles.
        """

    def has_northern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in northern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north(t)` is outgoing from `t`.
        """

    def has_north_eastern_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in north-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north_east(t)` is outgoing from `t`.
        """

    def has_eastern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `east(t)` is outgoing from `t`.
        """

    def has_south_eastern_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in south-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south_east(t)` is outgoing from `t`.
        """

    def has_southern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in southern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south(t)` is outgoing from `t`.
        """

    def has_south_western_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in south-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south_west(t)` is outgoing from `t`.
        """

    def has_western_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `west(t)` is outgoing from `t`.
        """

    def has_north_western_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in north-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north_west(t)` is outgoing from `t`.
        """

    def bounding_box_2d(self) -> tuple[coords.OffsetCoordinate, coords.OffsetCoordinate]:
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

    def is_dead(self, n: int) -> bool:
        """
        Checks whether a node (not its assigned tile) is dead. Nodes can be
        dead for a variety of reasons. For instance if they are dangling (see
        the `mockturtle` API). In this layout type, nodes are also marked dead
        when they are not assigned to a tile (which is considered equivalent
        to dangling).

        Args:
            n: Node to check for liveliness.

        Returns:
            `true` iff `n` is dead.
        """

    def assign_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int], delay: int
    ) -> None:
        """
        Assigns a synchronization element to the provided clock zone.

        Args:
            cz: Clock zone to turn into a synchronization element.
            se: Number of full clock cycles to extend `cz`'s Hold phase by. If
                this value is 0, `cz` is turned back into a normal clock zone.
        """

    def is_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Check whether the provided clock zone is a synchronization element.

        Args:
            cz: Clock zone to check.

        Returns:
            `true` iff `cz` is a synchronization element.
        """

    def get_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> int:
        """
        Returns the Hold phase extension in clock cycles of clock zone `cz`.

        Args:
            cz: Clock zone to check.

        Returns:
            Synchronization element value, i.e., Hold phase extension, of
            clock zone `cz`.
        """

    def num_se(self) -> int:
        """
        Counts zones with a nonzero Hold-phase extension. @return
        Synchronization element count.
        """

class OddColumnHexGateLayout:
    """
    A gate layout with geometry, clocking, gates, wires, and obstructions. Gate creation takes and returns coordinates. Node inspection uses identifiers from get_node().
    """

    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(self, dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Standard constructor. Creates a named gate-level layout of the given
        aspect ratio. To this end, it calls `CoordinateLayout`'s standard
        constructor.

        Args:
            ar: Highest possible position in the layout.
            name: Layout name.
        """

    @overload
    def __init__(
        self,
        dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        clocking_scheme: str = "2DDWave",
        layout_name: str = "",
    ) -> None:
        """
        Standard constructor. Creates a gate-level layout of the given aspect
        ratio and clocks it via the given clocking scheme. To this end, it
        calls `CoordinateLayout`'s standard constructor.

        Args:
            ar: Highest possible position in the layout.
            scheme: Clocking scheme to apply to this layout.
            name: Layout name.
        """

    def coord(self, x: int, y: int, z: int = 0) -> coords.OffsetCoordinate:
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

    def resize(self, dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Updates the layout's dimensions, effectively resizing it.

        Args:
            ar: New aspect ratio.
        """

    def north(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def east(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def south(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def west(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def above(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
        """
        Returns the coordinate that is directly above a given coordinate `c`,
        i.e., the face whose z-dimension is higher by 1. If `c`'s z-dimension
        is already at maximum, `c` is returned instead.

        Args:
            c: Coordinate whose above counterpart is desired.

        Returns:
            Coordinate directly above `c`.
        """

    def below(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        c1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
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
        c1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
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

    def is_ground_layer(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in the ground layer
        where z is minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in ground layer.
        """

    def is_crossing_layer(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in a crossing layer
        where z is not minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in a crossing layer.
        """

    def is_within_bounds(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located within the layout
        bounds.

        Args:
            c: Coordinate to check for boundary.

        Returns:
            `true` iff `c` is located within the layout bounds.
        """

    def coordinates(self) -> list[coords.OffsetCoordinate]:
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

    def ground_coordinates(self) -> list[coords.OffsetCoordinate]:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[tuple[coords.OffsetCoordinate, coords.OffsetCoordinate]]:
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

    def assign_clock_number(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int], cn: int
    ) -> None:
        """
        Overrides the clock number of a tile in the stored scheme. The clock
        number applies to every layer of the tile, so the z-coordinate of `cz`
        is ignored.

        Args:
            cz: Clock zone to override.
            cn: New clock number for `cz`.
        """

    def get_clock_number(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the clock number of a tile. Every layer of a tile has the same
        clock number, so the z-coordinate of `cz` is ignored.

        Args:
            cz: Clock zone.

        Returns:
            Clock number of `cz`.
        """

    def num_clocks(self) -> int:
        """
        Returns the number of clock phases in the layout. Each clock cycle is
        divided into n phases. In QCA, the number of phases is usually 4. In
        iNML it is 3. Clocking schemes support 3 or 4 phases.

        Returns:
            The number of different clock signals in the layout.
        """

    def is_regularly_clocked(self) -> bool:
        """
        Returns whether the layout is clocked by a regular clocking scheme
        with no overwritten zones.

        Returns:
            `true` iff the layout is clocked by a regular scheme and no zones
            have been overwritten.
        """

    def is_clocking_scheme(self, name: str) -> bool:
        """
        Compares the stored clocking scheme against the provided name.
        Predefined names are constants in `fiction::layouts::clocking`.

        Args:
            name: Clocking scheme name.

        Returns:
            `true` iff the layout is clocked by a clocking scheme of name
            `name`.
        """

    def get_clocking_scheme_name(self) -> str:
        """
        Returns the name of the layout's clocking scheme, e.g., `2DDWave` or `USE`.
        """

    def is_incoming_clocked(
        self,
        cz1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Evaluates whether clock zone `cz2` feeds information to clock zone
        `cz1`, i.e., whether `cz2` is clocked with a clock number that is
        lower by 1 modulo `num_clocks()`, or either zone is a synchronization
        element.

        Args:
            cz1: Base clock zone.
            cz2: Clock zone to check whether its clock number is lower by 1.

        Returns:
            `true` iff `cz2` can feed information to `cz1`.
        """

    def is_outgoing_clocked(
        self,
        cz1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Evaluates whether clock zone `cz2` accepts information from clock zone
        `cz1`, i.e., whether `cz2` is clocked with a clock number that is
        higher by 1 modulo `num_clocks()`, or either zone is a synchronization
        element.

        Args:
            cz1: Base clock zone.
            cz2: Clock zone to check whether its clock number is higher by 1.

        Returns:
            `true` iff `cz2` can accept information from `cz1`.
        """

    def incoming_clocked_zones(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
        """
        Returns a container with all clock zones that are incoming to the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are incoming to `cz`.
        """

    def outgoing_clocked_zones(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
        """
        Returns a container with all clock zones that are outgoing from the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are outgoing from `cz`.
        """

    def in_degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of incoming clock zones to the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s incoming clock zones.
        """

    def out_degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of outgoing clock zones from the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s outgoing clock zones.
        """

    def degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of distinct incoming or outgoing neighboring clock
        zones.

        Args:
            cz: Base clock zone.

        Returns:
            Number of distinct clocked neighbors of `cz`.
        """

    def replace_clocking_scheme(self, name: str) -> None:
        """
        Replaces the clocking scheme by the predefined scheme of the given name. Clock-number overrides are discarded; synchronization elements are kept. Raises ValueError for an unknown name.
        """

    def obstruct_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Marks the given coordinate as obstructed.

        Args:
            c: clock_zone to obstruct.
        """

    def obstruct_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Marks the connection from coordinate `src` to coordinate `tgt` as
        obstructed.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Note:
            clock_zones marked this way will not be crossed with wires by path
            finding algorithms.
        """

    def clear_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Clears the obstruction status of the given coordinate `c` if the
        obstruction was manually marked via `obstruct_coordinate`.

        Args:
            c: clock_zone to clear.
        """

    def clear_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Clears the obstruction status of the connection from coordinate `src`
        to coordinate `tgt` if the obstruction was manually marked via
        `obstruct_connection`.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.
        """

    def clear_obstructed_coordinates(self) -> None:
        """
        Clears all obstructed coordinates that were manually marked via
        `obstruct_coordinate`.
        """

    def clear_obstructed_connections(self) -> None:
        """
        Clears all obstructed connections that were manually marked via
        `obstruct_connection`.
        """

    def is_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks if the given coordinate is obstructed of some sort.

        Args:
            c: Coordinate to check.

        Returns:
            `true` iff `c` is obstructed.
        """

    def is_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks if the given coordinate-coordinate connection is obstructed of
        some sort.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Returns:
            `true` iff the connection from `src` to `tgt` is obstructed.
        """

    def create_pi(
        self, name: str = "", t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...
    ) -> coords.OffsetCoordinate: ...
    def create_po(
        self,
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        name: str = "",
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def is_pi(self, n: int) -> bool:
        """
        Check whether `n` is a primary input.

        Args:
            n: Node to be checked.

        Returns:
            `true` iff `n` is a PI.
        """

    def is_po(self, n: int) -> bool:
        """
        Check whether `n` is a primary output.

        Args:
            n: Node to be checked.

        Returns:
            `true` iff `n` is a PO.
        """

    def is_pi_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary input.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PI.
        """

    def is_po_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary output.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PO.
        """

    def is_inv(self, arg: int, /) -> bool:
        """
        Returns whether `n` computes the binary inversion (NOT gate).

        Args:
            n: Node to check.

        Returns:
            `true` iff `n` is a NOT gate.
        """

    def is_and(self, arg: int, /) -> bool: ...
    def is_nand(self, arg: int, /) -> bool: ...
    def is_or(self, arg: int, /) -> bool: ...
    def is_nor(self, arg: int, /) -> bool: ...
    def is_xor(self, arg: int, /) -> bool: ...
    def is_xnor(self, arg: int, /) -> bool: ...
    def is_lt(self, arg: int, /) -> bool: ...
    def is_le(self, arg: int, /) -> bool: ...
    def is_gt(self, arg: int, /) -> bool: ...
    def is_ge(self, arg: int, /) -> bool: ...
    def is_maj(self, arg: int, /) -> bool: ...
    def is_fanout(self, arg: int, /) -> bool:
        """
        Returns whether `n` is a wire and has multiple outputs, thereby,
        acting as a fanout gate. Note that a fanout will return `true` for
        both `is_wire` and `is_fanout`.

        Args:
            n: Node to check.

        Returns:
            `true` iff `n` is a fanout gate.
        """

    def is_wire(self, arg: int, /) -> bool:
        """Equivalent to `is_buf`."""

    def set_layout_name(self, name: str) -> None: ...
    @property
    def name(self) -> str:
        """The layout name."""

    @name.setter
    def name(self, arg: str, /) -> None: ...
    def get_layout_name(self) -> str: ...
    def clone(self) -> OddColumnHexGateLayout:
        """
        Clones the layout returning a deep copy.

        Returns:
            Deep copy of the layout.
        """

    def set_input_name(self, index: int, name: str) -> None: ...
    def get_input_name(self, index: int) -> str: ...
    def set_output_name(self, index: int, name: str) -> None: ...
    def get_output_name(self, index: int) -> str: ...
    def get_name(self, s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> str: ...
    def create_buf(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_not(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_and(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_nand(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_or(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_nor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_xor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_xnor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_lt(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_le(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_gt(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_ge(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_maj(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def num_pis(self) -> int: ...
    def num_pos(self) -> int: ...
    def num_gates(self) -> int:
        """
        Returns the number of placed nodes in the layout that do not compute
        the identity function.

        Returns:
            Number of gates in the layout.
        """

    def num_wires(self) -> int:
        """
        Returns the number of placed nodes in the layout that compute the
        identity function including PIs and POs.

        Returns:
            Number of wires in the layout.
        """

    def num_crossings(self) -> int:
        """
        Returns the number of placed nodes in the layout that compute the
        identity function and cross other nodes.

        Returns:
            Number of crossings in the layout.
        """

    def is_empty(self) -> bool:
        """
        Checks whether there are no gates or wires assigned to the layout's
        coordinates.

        Returns:
            `true` iff the layout is empty.
        """

    def fanin_size(self, n: int) -> int:
        """
        Returns the number of incoming, adjacently placed, and properly
        clocked signals to the given node.

        Args:
            n: Node to check.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            Number of fanins to `n`.
        """

    def fanout_size(self, n: int) -> int:
        """
        Returns the number of outgoing, adjacently placed, and properly
        clocked signals of the given node.

        Args:
            n: Node to check.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            Number of fanouts to `n`.
        """

    def get_node(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Fetches the node that is placed onto a tile pointed to by a given
        signal. If no node is placed there, the `const0` node is returned.

        Args:
            s: Pointer to a tile.

        Returns:
            Node at position `t` where `s` points at `t`; or 0 if no node is
            placed at `t`.
        """

    def get_tile(self, n: int) -> coords.OffsetCoordinate:
        """
        The inverse function of `get_node`. Fetches the tile that the provided
        node is placed on. Returns a default dead tile if the node is not
        placed.

        Args:
            n: Node whose location is desired.

        Returns:
            Tile at which `n` is placed or a default dead tile if `n` is not
            placed.
        """

    def make_signal(self, n: int) -> coords.OffsetCoordinate:
        """
        Invokes the same behavior as `get_tile(n)` but additionally casts the
        return value to a signal. That is, this function returns the signal
        representation of the tile that the node `n` is assigned to.

        Args:
            n: Node whose signal is desired.

        Returns:
            Signal that points to `n`.
        """

    def move_node(
        self,
        n: int,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        new_children: Sequence[coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]] = [],
    ) -> coords.OffsetCoordinate:
        """
        Moves a given node to a new position and also updates its children,
        i.e., incoming signals.

        Args:
            n: Node to move.
            t: Tile to move `n` to.
            new_children: New incoming signals to `n`.

        Returns:
            Signal pointing to `n`'s new tile.
        """

    def clear_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Removes all assigned nodes from the given tile and marks them as dead.

        Args:
            t: Tile whose nodes are to be removed.

        Note:
            This function does not reduce the number of nodes in the layout
            nor does it reduce the number of PIs that are being returned via
            `num_pis()` even if the tile to clear is an input tile. However,
            the number of POs is reduced if the tile to clear is an output
            tile. While this seems counter-intuitive and inconsistent, it is
            in line with mockturtle's understanding of nodes and primary
            outputs.
        """

    def is_gate_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_gate` (in
        accordance with `mockturtle`'s definition of gates).

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that is a neither a constant nor a PI.
        """

    def is_wire_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_wire`.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that computes the identity.
        """

    def is_empty_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether `t` does not have a node assigned to it.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` is an empty tile.
        """

    def pis(self) -> list[coords.OffsetCoordinate]: ...
    def pos(self) -> list[coords.OffsetCoordinate]: ...
    def gates(self) -> list[coords.OffsetCoordinate]: ...
    def wires(self) -> list[coords.OffsetCoordinate]: ...
    def fanins(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]: ...
    def fanouts(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]: ...
    def is_incoming_signal(
        self,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks whether signal `s` is incoming to tile `t`. That is, whether
        tile `t` hosts a node that has a fanin assigned to the tile that
        signal `s` points to.

        Args:
            t: Base tile.
            s: Signal pointing to a potential incoming tile to `t`.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `s` is incoming to `t`.
        """

    def has_no_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has no incoming tiles.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `t` does not have incoming tiles.
        """

    def has_northern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in northern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north(t)` is incoming to `t`.
        """

    def has_north_eastern_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in north-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north_east(t)` is incoming to `t`.
        """

    def has_eastern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `east(t)` is incoming to `t`.
        """

    def has_south_eastern_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in south-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south_east(t)` is incoming to `t`.
        """

    def has_southern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in southern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south(t)` is incoming to `t`.
        """

    def has_south_western_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in south-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south_west(t)` is incoming to `t`.
        """

    def has_western_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `west(t)` is incoming to `t`.
        """

    def has_north_western_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in north-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north_west(t)` is incoming to `t`.
        """

    def is_outgoing_signal(
        self,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks whether signal `s` is outgoing from tile `t`. That is, whether
        tile `t` hosts a node that has a fanout assigned to the tile that
        signal `s` points to.

        Args:
            t: Base tile.
            s: Signal pointing to a potential outgoing tile of `t`.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `s` is outgoing from `t`.
        """

    def has_no_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has no outgoing tiles.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `t` does not have outgoing tiles.
        """

    def has_northern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in northern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north(t)` is outgoing from `t`.
        """

    def has_north_eastern_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in north-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north_east(t)` is outgoing from `t`.
        """

    def has_eastern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `east(t)` is outgoing from `t`.
        """

    def has_south_eastern_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in south-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south_east(t)` is outgoing from `t`.
        """

    def has_southern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in southern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south(t)` is outgoing from `t`.
        """

    def has_south_western_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in south-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south_west(t)` is outgoing from `t`.
        """

    def has_western_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `west(t)` is outgoing from `t`.
        """

    def has_north_western_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in north-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north_west(t)` is outgoing from `t`.
        """

    def bounding_box_2d(self) -> tuple[coords.OffsetCoordinate, coords.OffsetCoordinate]:
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

    def is_dead(self, n: int) -> bool:
        """
        Checks whether a node (not its assigned tile) is dead. Nodes can be
        dead for a variety of reasons. For instance if they are dangling (see
        the `mockturtle` API). In this layout type, nodes are also marked dead
        when they are not assigned to a tile (which is considered equivalent
        to dangling).

        Args:
            n: Node to check for liveliness.

        Returns:
            `true` iff `n` is dead.
        """

    def assign_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int], delay: int
    ) -> None:
        """
        Assigns a synchronization element to the provided clock zone.

        Args:
            cz: Clock zone to turn into a synchronization element.
            se: Number of full clock cycles to extend `cz`'s Hold phase by. If
                this value is 0, `cz` is turned back into a normal clock zone.
        """

    def is_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Check whether the provided clock zone is a synchronization element.

        Args:
            cz: Clock zone to check.

        Returns:
            `true` iff `cz` is a synchronization element.
        """

    def get_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> int:
        """
        Returns the Hold phase extension in clock cycles of clock zone `cz`.

        Args:
            cz: Clock zone to check.

        Returns:
            Synchronization element value, i.e., Hold phase extension, of
            clock zone `cz`.
        """

    def num_se(self) -> int:
        """
        Counts zones with a nonzero Hold-phase extension. @return
        Synchronization element count.
        """

class EvenColumnHexGateLayout:
    """
    A gate layout with geometry, clocking, gates, wires, and obstructions. Gate creation takes and returns coordinates. Node inspection uses identifiers from get_node().
    """

    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(self, dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Standard constructor. Creates a named gate-level layout of the given
        aspect ratio. To this end, it calls `CoordinateLayout`'s standard
        constructor.

        Args:
            ar: Highest possible position in the layout.
            name: Layout name.
        """

    @overload
    def __init__(
        self,
        dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        clocking_scheme: str = "2DDWave",
        layout_name: str = "",
    ) -> None:
        """
        Standard constructor. Creates a gate-level layout of the given aspect
        ratio and clocks it via the given clocking scheme. To this end, it
        calls `CoordinateLayout`'s standard constructor.

        Args:
            ar: Highest possible position in the layout.
            scheme: Clocking scheme to apply to this layout.
            name: Layout name.
        """

    def coord(self, x: int, y: int, z: int = 0) -> coords.OffsetCoordinate:
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

    def resize(self, dimension: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Updates the layout's dimensions, effectively resizing it.

        Args:
            ar: New aspect ratio.
        """

    def north(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def east(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def south(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def west(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> coords.OffsetCoordinate:
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

    def above(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
        """
        Returns the coordinate that is directly above a given coordinate `c`,
        i.e., the face whose z-dimension is higher by 1. If `c`'s z-dimension
        is already at maximum, `c` is returned instead.

        Args:
            c: Coordinate whose above counterpart is desired.

        Returns:
            Coordinate directly above `c`.
        """

    def below(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> coords.OffsetCoordinate:
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
        c1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
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
        c1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
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

    def is_ground_layer(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in the ground layer
        where z is minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in ground layer.
        """

    def is_crossing_layer(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in a crossing layer
        where z is not minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in a crossing layer.
        """

    def is_within_bounds(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located within the layout
        bounds.

        Args:
            c: Coordinate to check for boundary.

        Returns:
            `true` iff `c` is located within the layout bounds.
        """

    def coordinates(self) -> list[coords.OffsetCoordinate]:
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

    def ground_coordinates(self) -> list[coords.OffsetCoordinate]:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
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
        self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[tuple[coords.OffsetCoordinate, coords.OffsetCoordinate]]:
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

    def assign_clock_number(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int], cn: int
    ) -> None:
        """
        Overrides the clock number of a tile in the stored scheme. The clock
        number applies to every layer of the tile, so the z-coordinate of `cz`
        is ignored.

        Args:
            cz: Clock zone to override.
            cn: New clock number for `cz`.
        """

    def get_clock_number(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the clock number of a tile. Every layer of a tile has the same
        clock number, so the z-coordinate of `cz` is ignored.

        Args:
            cz: Clock zone.

        Returns:
            Clock number of `cz`.
        """

    def num_clocks(self) -> int:
        """
        Returns the number of clock phases in the layout. Each clock cycle is
        divided into n phases. In QCA, the number of phases is usually 4. In
        iNML it is 3. Clocking schemes support 3 or 4 phases.

        Returns:
            The number of different clock signals in the layout.
        """

    def is_regularly_clocked(self) -> bool:
        """
        Returns whether the layout is clocked by a regular clocking scheme
        with no overwritten zones.

        Returns:
            `true` iff the layout is clocked by a regular scheme and no zones
            have been overwritten.
        """

    def is_clocking_scheme(self, name: str) -> bool:
        """
        Compares the stored clocking scheme against the provided name.
        Predefined names are constants in `fiction::layouts::clocking`.

        Args:
            name: Clocking scheme name.

        Returns:
            `true` iff the layout is clocked by a clocking scheme of name
            `name`.
        """

    def get_clocking_scheme_name(self) -> str:
        """
        Returns the name of the layout's clocking scheme, e.g., `2DDWave` or `USE`.
        """

    def is_incoming_clocked(
        self,
        cz1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Evaluates whether clock zone `cz2` feeds information to clock zone
        `cz1`, i.e., whether `cz2` is clocked with a clock number that is
        lower by 1 modulo `num_clocks()`, or either zone is a synchronization
        element.

        Args:
            cz1: Base clock zone.
            cz2: Clock zone to check whether its clock number is lower by 1.

        Returns:
            `true` iff `cz2` can feed information to `cz1`.
        """

    def is_outgoing_clocked(
        self,
        cz1: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Evaluates whether clock zone `cz2` accepts information from clock zone
        `cz1`, i.e., whether `cz2` is clocked with a clock number that is
        higher by 1 modulo `num_clocks()`, or either zone is a synchronization
        element.

        Args:
            cz1: Base clock zone.
            cz2: Clock zone to check whether its clock number is higher by 1.

        Returns:
            `true` iff `cz2` can accept information from `cz1`.
        """

    def incoming_clocked_zones(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
        """
        Returns a container with all clock zones that are incoming to the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are incoming to `cz`.
        """

    def outgoing_clocked_zones(
        self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]:
        """
        Returns a container with all clock zones that are outgoing from the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are outgoing from `cz`.
        """

    def in_degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of incoming clock zones to the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s incoming clock zones.
        """

    def out_degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of outgoing clock zones from the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s outgoing clock zones.
        """

    def degree(self, cz: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of distinct incoming or outgoing neighboring clock
        zones.

        Args:
            cz: Base clock zone.

        Returns:
            Number of distinct clocked neighbors of `cz`.
        """

    def replace_clocking_scheme(self, name: str) -> None:
        """
        Replaces the clocking scheme by the predefined scheme of the given name. Clock-number overrides are discarded; synchronization elements are kept. Raises ValueError for an unknown name.
        """

    def obstruct_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Marks the given coordinate as obstructed.

        Args:
            c: clock_zone to obstruct.
        """

    def obstruct_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Marks the connection from coordinate `src` to coordinate `tgt` as
        obstructed.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Note:
            clock_zones marked this way will not be crossed with wires by path
            finding algorithms.
        """

    def clear_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Clears the obstruction status of the given coordinate `c` if the
        obstruction was manually marked via `obstruct_coordinate`.

        Args:
            c: clock_zone to clear.
        """

    def clear_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Clears the obstruction status of the connection from coordinate `src`
        to coordinate `tgt` if the obstruction was manually marked via
        `obstruct_connection`.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.
        """

    def clear_obstructed_coordinates(self) -> None:
        """
        Clears all obstructed coordinates that were manually marked via
        `obstruct_coordinate`.
        """

    def clear_obstructed_connections(self) -> None:
        """
        Clears all obstructed connections that were manually marked via
        `obstruct_connection`.
        """

    def is_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks if the given coordinate is obstructed of some sort.

        Args:
            c: Coordinate to check.

        Returns:
            `true` iff `c` is obstructed.
        """

    def is_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks if the given coordinate-coordinate connection is obstructed of
        some sort.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Returns:
            `true` iff the connection from `src` to `tgt` is obstructed.
        """

    def create_pi(
        self, name: str = "", t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...
    ) -> coords.OffsetCoordinate: ...
    def create_po(
        self,
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        name: str = "",
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def is_pi(self, n: int) -> bool:
        """
        Check whether `n` is a primary input.

        Args:
            n: Node to be checked.

        Returns:
            `true` iff `n` is a PI.
        """

    def is_po(self, n: int) -> bool:
        """
        Check whether `n` is a primary output.

        Args:
            n: Node to be checked.

        Returns:
            `true` iff `n` is a PO.
        """

    def is_pi_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary input.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PI.
        """

    def is_po_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary output.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PO.
        """

    def is_inv(self, arg: int, /) -> bool:
        """
        Returns whether `n` computes the binary inversion (NOT gate).

        Args:
            n: Node to check.

        Returns:
            `true` iff `n` is a NOT gate.
        """

    def is_and(self, arg: int, /) -> bool: ...
    def is_nand(self, arg: int, /) -> bool: ...
    def is_or(self, arg: int, /) -> bool: ...
    def is_nor(self, arg: int, /) -> bool: ...
    def is_xor(self, arg: int, /) -> bool: ...
    def is_xnor(self, arg: int, /) -> bool: ...
    def is_lt(self, arg: int, /) -> bool: ...
    def is_le(self, arg: int, /) -> bool: ...
    def is_gt(self, arg: int, /) -> bool: ...
    def is_ge(self, arg: int, /) -> bool: ...
    def is_maj(self, arg: int, /) -> bool: ...
    def is_fanout(self, arg: int, /) -> bool:
        """
        Returns whether `n` is a wire and has multiple outputs, thereby,
        acting as a fanout gate. Note that a fanout will return `true` for
        both `is_wire` and `is_fanout`.

        Args:
            n: Node to check.

        Returns:
            `true` iff `n` is a fanout gate.
        """

    def is_wire(self, arg: int, /) -> bool:
        """Equivalent to `is_buf`."""

    def set_layout_name(self, name: str) -> None: ...
    @property
    def name(self) -> str:
        """The layout name."""

    @name.setter
    def name(self, arg: str, /) -> None: ...
    def get_layout_name(self) -> str: ...
    def clone(self) -> EvenColumnHexGateLayout:
        """
        Clones the layout returning a deep copy.

        Returns:
            Deep copy of the layout.
        """

    def set_input_name(self, index: int, name: str) -> None: ...
    def get_input_name(self, index: int) -> str: ...
    def set_output_name(self, index: int, name: str) -> None: ...
    def get_output_name(self, index: int) -> str: ...
    def get_name(self, s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> str: ...
    def create_buf(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_not(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_and(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_nand(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_or(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_nor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_xor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_xnor(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_lt(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_le(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_gt(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_ge(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def create_maj(
        self,
        a: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        b: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int] = ...,
    ) -> coords.OffsetCoordinate: ...
    def num_pis(self) -> int: ...
    def num_pos(self) -> int: ...
    def num_gates(self) -> int:
        """
        Returns the number of placed nodes in the layout that do not compute
        the identity function.

        Returns:
            Number of gates in the layout.
        """

    def num_wires(self) -> int:
        """
        Returns the number of placed nodes in the layout that compute the
        identity function including PIs and POs.

        Returns:
            Number of wires in the layout.
        """

    def num_crossings(self) -> int:
        """
        Returns the number of placed nodes in the layout that compute the
        identity function and cross other nodes.

        Returns:
            Number of crossings in the layout.
        """

    def is_empty(self) -> bool:
        """
        Checks whether there are no gates or wires assigned to the layout's
        coordinates.

        Returns:
            `true` iff the layout is empty.
        """

    def fanin_size(self, n: int) -> int:
        """
        Returns the number of incoming, adjacently placed, and properly
        clocked signals to the given node.

        Args:
            n: Node to check.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            Number of fanins to `n`.
        """

    def fanout_size(self, n: int) -> int:
        """
        Returns the number of outgoing, adjacently placed, and properly
        clocked signals of the given node.

        Args:
            n: Node to check.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            Number of fanouts to `n`.
        """

    def get_node(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Fetches the node that is placed onto a tile pointed to by a given
        signal. If no node is placed there, the `const0` node is returned.

        Args:
            s: Pointer to a tile.

        Returns:
            Node at position `t` where `s` points at `t`; or 0 if no node is
            placed at `t`.
        """

    def get_tile(self, n: int) -> coords.OffsetCoordinate:
        """
        The inverse function of `get_node`. Fetches the tile that the provided
        node is placed on. Returns a default dead tile if the node is not
        placed.

        Args:
            n: Node whose location is desired.

        Returns:
            Tile at which `n` is placed or a default dead tile if `n` is not
            placed.
        """

    def make_signal(self, n: int) -> coords.OffsetCoordinate:
        """
        Invokes the same behavior as `get_tile(n)` but additionally casts the
        return value to a signal. That is, this function returns the signal
        representation of the tile that the node `n` is assigned to.

        Args:
            n: Node whose signal is desired.

        Returns:
            Signal that points to `n`.
        """

    def move_node(
        self,
        n: int,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        new_children: Sequence[coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]] = [],
    ) -> coords.OffsetCoordinate:
        """
        Moves a given node to a new position and also updates its children,
        i.e., incoming signals.

        Args:
            n: Node to move.
            t: Tile to move `n` to.
            new_children: New incoming signals to `n`.

        Returns:
            Signal pointing to `n`'s new tile.
        """

    def clear_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Removes all assigned nodes from the given tile and marks them as dead.

        Args:
            t: Tile whose nodes are to be removed.

        Note:
            This function does not reduce the number of nodes in the layout
            nor does it reduce the number of PIs that are being returned via
            `num_pis()` even if the tile to clear is an input tile. However,
            the number of POs is reduced if the tile to clear is an output
            tile. While this seems counter-intuitive and inconsistent, it is
            in line with mockturtle's understanding of nodes and primary
            outputs.
        """

    def is_gate_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_gate` (in
        accordance with `mockturtle`'s definition of gates).

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that is a neither a constant nor a PI.
        """

    def is_wire_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_wire`.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that computes the identity.
        """

    def is_empty_tile(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether `t` does not have a node assigned to it.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` is an empty tile.
        """

    def pis(self) -> list[coords.OffsetCoordinate]: ...
    def pos(self) -> list[coords.OffsetCoordinate]: ...
    def gates(self) -> list[coords.OffsetCoordinate]: ...
    def wires(self) -> list[coords.OffsetCoordinate]: ...
    def fanins(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]: ...
    def fanouts(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[coords.OffsetCoordinate]: ...
    def is_incoming_signal(
        self,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks whether signal `s` is incoming to tile `t`. That is, whether
        tile `t` hosts a node that has a fanin assigned to the tile that
        signal `s` points to.

        Args:
            t: Base tile.
            s: Signal pointing to a potential incoming tile to `t`.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `s` is incoming to `t`.
        """

    def has_no_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has no incoming tiles.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `t` does not have incoming tiles.
        """

    def has_northern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in northern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north(t)` is incoming to `t`.
        """

    def has_north_eastern_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in north-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north_east(t)` is incoming to `t`.
        """

    def has_eastern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `east(t)` is incoming to `t`.
        """

    def has_south_eastern_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in south-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south_east(t)` is incoming to `t`.
        """

    def has_southern_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in southern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south(t)` is incoming to `t`.
        """

    def has_south_western_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in south-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `south_west(t)` is incoming to `t`.
        """

    def has_western_incoming_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an incoming one in western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `west(t)` is incoming to `t`.
        """

    def has_north_western_incoming_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an incoming one in north-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanins.

        Returns:
            `true` iff `north_west(t)` is incoming to `t`.
        """

    def is_outgoing_signal(
        self,
        t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        s: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks whether signal `s` is outgoing from tile `t`. That is, whether
        tile `t` hosts a node that has a fanout assigned to the tile that
        signal `s` points to.

        Args:
            t: Base tile.
            s: Signal pointing to a potential outgoing tile of `t`.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `s` is outgoing from `t`.
        """

    def has_no_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has no outgoing tiles.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `t` does not have outgoing tiles.
        """

    def has_northern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in northern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north(t)` is outgoing from `t`.
        """

    def has_north_eastern_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in north-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north_east(t)` is outgoing from `t`.
        """

    def has_eastern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `east(t)` is outgoing from `t`.
        """

    def has_south_eastern_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in south-eastern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south_east(t)` is outgoing from `t`.
        """

    def has_southern_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in southern
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south(t)` is outgoing from `t`.
        """

    def has_south_western_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in south-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `south_west(t)` is outgoing from `t`.
        """

    def has_western_outgoing_signal(self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks whether the given tile has an outgoing one in western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `west(t)` is outgoing from `t`.
        """

    def has_north_western_outgoing_signal(
        self, t: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Checks whether the given tile has an outgoing one in north-western
        direction.

        Args:
            t: Base tile.

        Template Args:
            RespectClocking: Flag to indicate that the underlying clocking is
                             to be respected when evaluating fanouts.

        Returns:
            `true` iff `north_west(t)` is outgoing from `t`.
        """

    def bounding_box_2d(self) -> tuple[coords.OffsetCoordinate, coords.OffsetCoordinate]:
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

    def is_dead(self, n: int) -> bool:
        """
        Checks whether a node (not its assigned tile) is dead. Nodes can be
        dead for a variety of reasons. For instance if they are dangling (see
        the `mockturtle` API). In this layout type, nodes are also marked dead
        when they are not assigned to a tile (which is considered equivalent
        to dangling).

        Args:
            n: Node to check for liveliness.

        Returns:
            `true` iff `n` is dead.
        """

    def assign_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int], delay: int
    ) -> None:
        """
        Assigns a synchronization element to the provided clock zone.

        Args:
            cz: Clock zone to turn into a synchronization element.
            se: Number of full clock cycles to extend `cz`'s Hold phase by. If
                this value is 0, `cz` is turned back into a normal clock zone.
        """

    def is_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> bool:
        """
        Check whether the provided clock zone is a synchronization element.

        Args:
            cz: Clock zone to check.

        Returns:
            `true` iff `cz` is a synchronization element.
        """

    def get_synchronization_element(
        self, coordinate: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]
    ) -> int:
        """
        Returns the Hold phase extension in clock cycles of clock zone `cz`.

        Args:
            cz: Clock zone to check.

        Returns:
            Synchronization element value, i.e., Hold phase extension, of
            clock zone `cz`.
        """

    def num_se(self) -> int:
        """
        Counts zones with a nonzero Hold-phase extension. @return
        Synchronization element count.
        """

class Obstructions:
    """
    Explicit obstructions stored by layouts or supplied to a routing
    search.

    Copies are independent. This object contains no layout or occupancy
    information.

    Template Args:
        Coordinate: Coordinate identifying a position.
    """

    def __init__(self) -> None:
        """Creates empty routing constraints."""

    def obstruct_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Marks the given coordinate as obstructed.

        Args:
            c: Coordinate to obstruct.
        """

    def obstruct_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Marks the connection from coordinate `src` to coordinate `tgt` as
        obstructed.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Note:
            Coordinates marked this way will not be crossed with wires by path
            finding algorithms.
        """

    def clear_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Clears the obstruction status of the given coordinate `c` if the
        obstruction was manually marked via `obstruct_coordinate`.

        Args:
            c: Coordinate to clear.
        """

    def clear_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> None:
        """
        Clears the obstruction status of the connection from coordinate `src`
        to coordinate `tgt` if the obstruction was manually marked via
        `obstruct_connection`.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.
        """

    def clear_obstructed_coordinates(self) -> None:
        """
        Clears all obstructed coordinates that were manually marked via
        `obstruct_coordinate`.
        """

    def clear_obstructed_connections(self) -> None:
        """
        Clears all obstructed connections that were manually marked via
        `obstruct_connection`.
        """

    def is_obstructed_coordinate(self, c: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks if the given coordinate is obstructed of some sort.

        Args:
            c: Coordinate to check.

        Returns:
            `true` iff `c` is obstructed.
        """

    def is_obstructed_connection(
        self,
        src: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coords.OffsetCoordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Checks if the given coordinate-coordinate connection is obstructed of
        some sort.

        Args:
            src: Source coordinate.
            tgt: Target coordinate.

        Returns:
            `true` iff the connection from `src` to `tgt` is obstructed.
        """

@overload
def normalize_layout_coordinates(lyt: mnt.pyfiction._native.qca.QCALayout) -> mnt.pyfiction._native.qca.QCALayout: ...
@overload
def normalize_layout_coordinates(
    lyt: mnt.pyfiction._native.mol_qca.MolecularQCALayout,
) -> mnt.pyfiction._native.mol_qca.MolecularQCALayout: ...
@overload
def normalize_layout_coordinates(lyt: mnt.pyfiction._native.inml.INMLLayout) -> mnt.pyfiction._native.inml.INMLLayout:
    """
    Returns a copy of the given cell grid layout whose cells are shifted
    towards the origin, so that the smallest occupied x- and y-coordinates
    become 0. Cell types, names, and, where the layout has them, cell
    modes move with their cells; layers, the layout name, and the clocking
    stay unchanged. The dimensions shrink by the shift.

    Args:
        lyt: The layout to normalize.

    Template Args:
        Lyt: Cell grid layout type, e.g., `qca::layout`,
             `mol_qca::layout`, or `inml::layout`.

    Returns:
        Normalized copy of `lyt`.
    """
