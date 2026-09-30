# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Coordinates, layout topologies, clocking, and gate-level layouts."""

import enum
from collections.abc import Sequence
from typing import TypeAlias, overload

import mnt.pyfiction.inml
import mnt.pyfiction.mol_qca
import mnt.pyfiction.qca
from mnt.pyfiction.layouts import io as io

class coordinate:
    """
    Signed coordinates.

    An coordinate coordinate defines a location via an coordinate from a
    fixed point (origin). Each axis is a signed 32-bit integer. The
    default-constructed coordinate is invalid; it has all axes set to
    `INVALID_AXIS` and stands for "no coordinate", e.g., a neighbor
    outside of a layout or the tile of a node that is not placed.

    Gate-level layouts pack a coordinate into a 64-bit signal with
    `explicit operator uint64_t`. This encoding holds 31-bit signed x and
    y values and a single z bit.
    """

    @overload
    def __init__(self) -> None:
        """Default constructor. Creates the invalid coordinate."""

    @overload
    def __init__(self, x: int, y: int, z: int = 0) -> None:
        """
        Standard constructor. Creates a coordinate at (x_, y_, z_).

        Args:
            x_: x position.
            y_: y position.
            z_: z position.

        Template Args:
            X: Type of x.
            Y: Type of y.
            Z: Type of z.
        """

    @overload
    def __init__(self, c: coordinate) -> None: ...
    @overload
    def __init__(self, tuple_repr: tuple[int, int] | tuple[int, int, int]) -> None: ...
    @property
    def x(self) -> int:
        """x coordinate."""

    @x.setter
    def x(self, arg: int, /) -> None: ...
    @property
    def y(self) -> int:
        """y coordinate."""

    @y.setter
    def y(self, arg: int, /) -> None: ...
    @property
    def z(self) -> int:
        """z coordinate."""

    @z.setter
    def z(self, arg: int, /) -> None: ...
    def is_valid(self) -> bool:
        """
        Returns whether the coordinate is valid, i.e., whether it differs from
        the default-constructed coordinate.

        Returns:
            `true` iff the coordinate is valid.
        """

    def __eq__(self, other: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Compares against another coordinate for equality. All invalid
        coordinates are equal.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff both coordinates are identical.
        """

    def __ne__(self, other: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Compares against another coordinate for inequality.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff both coordinates are not identical.
        """

    def __lt__(self, other: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Determine whether this coordinate is "less than" another one. This is
        the case if z is smaller, or if z is equal but y is smaller, or if z
        and y are equal but x is smaller.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff this coordinate is "less than" the other coordinate.
        """

    def __gt__(self, other: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Determine whether this coordinate is "greater than" another one. This
        is the case if the other one is "less than".

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff this coordinate is "greater than" the other coordinate.
        """

    def __le__(self, other: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Determine whether this coordinate is "less than or equal to" another
        one. This is the case if this one is not "greater than" the other.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff this coordinate is "less than or equal to" the other
            coordinate.
        """

    def __ge__(self, other: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Determine whether this coordinate is "greater than or equal to"
        another one. This is the case if this one is not "less than" the
        other.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff this coordinate is "greater than or equal to" the other
            coordinate.
        """

    def __hash__(self) -> int:
        """Returns a hash value of the coordinate."""

def area(coord: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
    """
    Computes the area of a given coordinate assuming its origin is (0, 0,
    0). Calculates :math:`(|x| + 1) \\cdot (|y| + 1)`.

    Args:
        coord: Coordinate.

    Template Args:
        CoordinateType: Coordinate type.

    Returns:
        Area of coord.
    """

def volume(coord: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
    """
    Computes the volume of a given coordinate assuming its origin is (0,
    0, 0). Calculates :math:`(|x| + 1) \\cdot (|y| + 1) \\cdot (|z| + 1)`.

    Args:
        coord: Coordinate.

    Template Args:
        CoordinateType: Coordinate type.

    Returns:
        Volume of coord.
    """

class arrangement(enum.Enum):
    """
    Arrangement of the shifted rows or columns of a shifted Cartesian or
    hexagonal layout.
    """

    ODD_ROW = 0
    """Odd rows are shifted."""

    EVEN_ROW = 1
    """Even rows are shifted."""

    ODD_COLUMN = 2
    """Odd columns are shifted."""

    EVEN_COLUMN = 3
    """Even columns are shifted."""

class cartesian_layout:
    """
    A layout type that utilizes offset coordinates to represent a
    Cartesian grid. Its faces are organized in the following way:

    .. code-block:: text

        +-------+-------+-------+-------+
        |       |       |       |       |
        | (0,0) | (1,0) | (2,0) | (3,0) |
        |       |       |       |       |
        +-------+-------+-------+-------+
        |       |       |       |       |
        | (0,1) | (1,1) | (2,1) | (3,1) |
        |       |       |       |       |
        +-------+-------+-------+-------+
        |       |       |       |       |
        | (0,2) | (1,2) | (2,2) | (3,2) |
        |       |       |       |       |
        +-------+-------+-------+-------+
    """

    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(self, dimension: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Standard constructor. The given aspect ratio points to the highest
        possible coordinate in the layout. That means in the ASCII layout
        above `ar = (3,2)`. Consequently, with `ar = (0,0)`, the layout has
        exactly one coordinate.

        Args:
            ar: Highest possible position in the layout.

        Raises:
            std::invalid_argument: If an axis of `ar` is negative.
        """

    def coord(self, x: int, y: int, z: int = 0) -> coordinate:
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
            A coordinate in the layout of type `coordinate`.

        Note:
            This function is equivalent to calling `coordinate(x, y, z)`.
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

    def resize(self, dimension: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Updates the layout's dimensions, effectively resizing it.

        Args:
            ar: New aspect ratio.

        Raises:
            std::invalid_argument: If an axis of `ar` is negative.
        """

    def north(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def north_east(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def east(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def south_east(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def south(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def south_west(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def west(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def north_west(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def above(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate that is directly above a given coordinate `c`,
        i.e., the face whose z-dimension is higher by 1. If `c`'s z-dimension
        is already at maximum, `c` is returned instead.

        Args:
            c: Coordinate whose above counterpart is desired.

        Returns:
            Coordinate directly above `c`.
        """

    def below(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate that is directly below a given coordinate `c`,
        i.e., the face whose z-dimension is lower by 1. If `c`'s z-dimension
        is already at minimum, `c` is returned instead.

        Args:
            c: Coordinate whose below counterpart is desired.

        Returns:
            Coordinate directly below `c`.
        """

    def is_north_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is directly north of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is directly north of `c1`.
        """

    def is_east_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is directly east of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is directly east of `c1`.
        """

    def is_south_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is directly south of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is directly south of `c1`.
        """

    def is_west_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is directly west of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is directly west of `c1`.
        """

    def is_adjacent_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
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
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
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

    def is_above(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is directly above coordinate `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is directly above `c1`.
        """

    def is_below(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is directly below coordinate `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is directly below `c1`.
        """

    def is_northwards_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is somewhere north of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is somewhere north of `c1`.
        """

    def is_eastwards_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is somewhere east of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is somewhere east of `c1`.
        """

    def is_southwards_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is somewhere south of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is somewhere south of `c1`.
        """

    def is_westwards_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is somewhere west of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is somewhere west of `c1`.
        """

    def is_at_northern_border(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located at the layout's
        northern border where y is minimal.

        Args:
            c: Coordinate to check for border location.

        Returns:
            `true` iff `c` is located at the layout's northern border.
        """

    def is_at_eastern_border(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located at the layout's
        eastern border where x is maximal.

        Args:
            c: Coordinate to check for border location.

        Returns:
            `true` iff `c` is located at the layout's northern border.
        """

    def is_at_southern_border(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located at the layout's
        southern border where y is maximal.

        Args:
            c: Coordinate to check for border location.

        Returns:
            `true` iff `c` is located at the layout's southern border.
        """

    def is_at_western_border(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located at the layout's
        western border where x is minimal.

        Args:
            c: Coordinate to check for border location.

        Returns:
            `true` iff `c` is located at the layout's western border.
        """

    def is_at_any_border(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located at any of the layout's
        borders where x or y are either minimal or maximal.

        Args:
            c: Coordinate to check for border location.

        Returns:
            `true` iff `c` is located at any of the layout's borders.
        """

    def northern_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate with the same x and z values as a given
        coordinate but that is located at the layout's northern border.

        Args:
            c: Coordinate whose border counterpart is desired.

        Returns:
            The northern border equivalent of `c`.
        """

    def eastern_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate with the same y and z values as a given
        coordinate but that is located at the layout's eastern border.

        Args:
            c: Coordinate whose border counterpart is desired.

        Returns:
            The eastern border equivalent of `c`.
        """

    def southern_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate with the same x and z values as a given
        coordinate but that is located at the layout's southern border.

        Args:
            c: Coordinate whose border counterpart is desired.

        Returns:
            The southern border equivalent of `c`.
        """

    def western_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate with the same y and z values as a given
        coordinate but that is located at the layout's western border.

        Args:
            c: Coordinate whose border counterpart is desired.

        Returns:
            The western border equivalent of `c`.
        """

    def is_ground_layer(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in the ground layer
        where z is minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in ground layer.
        """

    def is_crossing_layer(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in a crossing layer
        where z is not minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in a crossing layer.
        """

    def is_within_bounds(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located within the layout
        bounds.

        Args:
            c: Coordinate to check for boundary.

        Returns:
            `true` iff `c` is located within the layout bounds.
        """

    def coordinates(self) -> list[coordinate]:
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

    def ground_coordinates(self) -> list[coordinate]:
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

    def adjacent_coordinates(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]:
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
        self, c: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[tuple[coordinate, coordinate]]:
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

stacked_cartesian_layout: TypeAlias = cartesian_layout

class shifted_cartesian_layout:
    """
    A layout type that utilizes offset coordinates to represent a
    Cartesian layout with shifted coordinates. In this implementation, odd
    columns are vertically shifted. Its faces are organized in the following
    way:

    .. code-block:: text

              +-------+       +-------+
              |       |       |       |
      +-------+ (1,0) +-------+ (3,0) |
      |       |       |       |       |
      | (0,0) +-------+ (2,0) +-------+
      |       |       |       |       |
      +-------+ (1,1) +-------+ (3,1) |
      |       |       |       |       |
      | (0,1) +-------+ (2,1) +-------+
      |       |       |       |       |
      +-------+ (1,2) +-------+ (3,2) |
              |       |       |       |
              +-------+       +-------+
    """

    @overload
    def __init__(self, arrangement: arrangement) -> None: ...
    @overload
    def __init__(
        self, arrangement: arrangement, dimension: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> None:
        """
        Standard constructor. The given aspect ratio points to the highest
        possible coordinate in the layout. That means in the
        `arrangement::EVEN_COLUMN` ASCII layout representation above `ar =
        (3,2)`. Consequently, with `ar = (0,0)`, the layout has exactly one
        coordinate.

        Args:
            a: Arrangement of the shifted rows or columns. It cannot change
               after construction.
            ar: Highest possible position in the layout.
        """

    def get_arrangement(self) -> arrangement:
        """
        Returns the arrangement of the shifted rows or columns.

        Returns:
            Arrangement fixed at construction.
        """

    def coord(self, x: int, y: int, z: int = 0) -> coordinate:
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
            A coordinate in the layout of type `coordinate`.

        Note:
            This function is equivalent to calling `coordinate(x, y, z)`.
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

    def resize(self, dimension: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Updates the layout's dimensions, effectively resizing it.

        Args:
            ar: New aspect ratio.

        Raises:
            std::invalid_argument: If an axis of `ar` is negative.
        """

    def north(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def north_east(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def east(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def south_east(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def south(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def south_west(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def west(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def north_west(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def above(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate that is directly above a given coordinate `c`,
        i.e., the face whose z-dimension is higher by 1. If `c`'s z-dimension
        is already at maximum, `c` is returned instead.

        Args:
            c: Coordinate whose above counterpart is desired.

        Returns:
            Coordinate directly above `c`.
        """

    def below(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate that is directly below a given coordinate `c`,
        i.e., the face whose z-dimension is lower by 1. If `c`'s z-dimension
        is already at minimum, `c` is returned instead.

        Args:
            c: Coordinate whose below counterpart is desired.

        Returns:
            Coordinate directly below `c`.
        """

    def is_north_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is directly north of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is directly north of `c1`.
        """

    def is_east_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is directly east of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is directly east of `c1`.
        """

    def is_south_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is directly south of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is directly south of `c1`.
        """

    def is_west_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is directly west of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is directly west of `c1`.
        """

    def is_adjacent_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
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
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
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

    def is_above(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is directly above coordinate `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is directly above `c1`.
        """

    def is_below(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is directly below coordinate `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is directly below `c1`.
        """

    def is_northwards_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is somewhere north of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is somewhere north of `c1`.
        """

    def is_eastwards_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is somewhere east of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is somewhere east of `c1`.
        """

    def is_southwards_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is somewhere south of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is somewhere south of `c1`.
        """

    def is_westwards_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is somewhere west of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is somewhere west of `c1`.
        """

    def is_at_northern_border(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located at the layout's
        northern border where y is minimal.

        Args:
            c: Coordinate to check for border location.

        Returns:
            `true` iff `c` is located at the layout's northern border.
        """

    def is_at_eastern_border(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located at the layout's
        eastern border where x is maximal.

        Args:
            c: Coordinate to check for border location.

        Returns:
            `true` iff `c` is located at the layout's northern border.
        """

    def is_at_southern_border(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located at the layout's
        southern border where y is maximal.

        Args:
            c: Coordinate to check for border location.

        Returns:
            `true` iff `c` is located at the layout's southern border.
        """

    def is_at_western_border(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located at the layout's
        western border where x is minimal.

        Args:
            c: Coordinate to check for border location.

        Returns:
            `true` iff `c` is located at the layout's western border.
        """

    def is_at_any_border(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located at any of the layout's
        borders where x or y are either minimal or maximal.

        Args:
            c: Coordinate to check for border location.

        Returns:
            `true` iff `c` is located at any of the layout's borders.
        """

    def northern_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate with the same x and z values as a given
        coordinate but that is located at the layout's northern border.

        Args:
            c: Coordinate whose border counterpart is desired.

        Returns:
            The northern border equivalent of `c`.
        """

    def eastern_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate with the same y and z values as a given
        coordinate but that is located at the layout's eastern border.

        Args:
            c: Coordinate whose border counterpart is desired.

        Returns:
            The eastern border equivalent of `c`.
        """

    def southern_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate with the same x and z values as a given
        coordinate but that is located at the layout's southern border.

        Args:
            c: Coordinate whose border counterpart is desired.

        Returns:
            The southern border equivalent of `c`.
        """

    def western_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate with the same y and z values as a given
        coordinate but that is located at the layout's western border.

        Args:
            c: Coordinate whose border counterpart is desired.

        Returns:
            The western border equivalent of `c`.
        """

    def is_ground_layer(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in the ground layer
        where z is minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in ground layer.
        """

    def is_crossing_layer(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in a crossing layer
        where z is not minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in a crossing layer.
        """

    def is_within_bounds(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located within the layout
        bounds.

        Args:
            c: Coordinate to check for boundary.

        Returns:
            `true` iff `c` is located within the layout bounds.
        """

    def coordinates(self) -> list[coordinate]:
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

    def ground_coordinates(self) -> list[coordinate]:
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

    def adjacent_coordinates(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]:
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
        self, c: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[tuple[coordinate, coordinate]]:
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

class hexagonal_layout:
    """
    A layout type that utilizes offset coordinates to represent a
    hexagonal grid. In this implementation, the hexagons are in the pointy-top
    orientation with even rows horizontally shifted. Its faces are organized
    in the following way:

    .. code-block:: text

               / \\     / \\     / \\
             /     \\ /     \\ /     \\
            | (0,0) | (1,0) | (2,0) |
            |       |       |       |
           / \\     / \\     / \\     /
         /     \\ /     \\ /     \\ /
        | (0,1) | (1,1) | (2,1) |
        |       |       |       |
         \\     / \\     / \\     / \\
           \\ /     \\ /     \\ /     \\
            | (0,2) | (1,2) | (2,2) |
            |       |       |       |
             \\     / \\     / \\     /
               \\ /     \\ /     \\ /

    Other representations would be using cube or axial coordinates for
    instance, but since we want the layouts to be rectangular-ish, offset
    coordinates make the most sense here.

    https://www.redblobgames.com/grids/hexagons/ is a wonderful resource
    on the topic.
    """

    @overload
    def __init__(self, arrangement: arrangement) -> None: ...
    @overload
    def __init__(
        self, arrangement: arrangement, dimension: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> None:
        """
        Standard constructor. The given aspect ratio points to the highest
        possible coordinate in the layout. That means in the
        `arrangement::EVEN_COLUMN` ASCII layout representation above `ar =
        (3,2)`. Consequently, with `ar = (0,0)`, the layout has exactly one
        coordinate.

        Args:
            a: Arrangement of the shifted rows or columns. It cannot change
               after construction.
            ar: Highest possible position in the layout.

        Raises:
            std::invalid_argument: If an axis of `ar` is negative.
        """

    def get_arrangement(self) -> arrangement:
        """
        Returns the arrangement of the shifted rows or columns.

        Returns:
            Arrangement fixed at construction.
        """

    def coord(self, x: int, y: int, z: int = 0) -> coordinate:
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
            A coordinate in the layout of type `coordinate`.

        Note:
            This function is equivalent to calling `coordinate(x, y, z)`.
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

    def resize(self, dimension: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Updates the layout's dimensions, effectively resizing it.

        Args:
            ar: New aspect ratio.

        Raises:
            std::invalid_argument: If an axis of `ar` is negative.
        """

    def north(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def north_east(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate that is located in north-eastern direction of a
        given coordinate `c`. Depending on the arrangement of the layout, the
        dimension values of the returned coordinate may differ.

        Args:
            c: Coordinate whose north-eastern counterpart is desired.

        Returns:
            Coordinate directly north-eastern of `c`.
        """

    def east(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def south_east(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate that is located in south-eastern direction of a
        given coordinate `c`. Depending on the arrangement of the layout, the
        dimension values of the returned coordinate may differ.

        Args:
            c: Coordinate whose south-eastern counterpart is desired.

        Returns:
            Coordinate directly south-eastern of `c`.
        """

    def south(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def south_west(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate that is located in south-western direction of a
        given coordinate `c`. Depending on the arrangement of the layout, the
        dimension values of the returned coordinate may differ.

        Args:
            c: Coordinate whose south-western counterpart is desired.

        Returns:
            Coordinate directly south-western of `c`.
        """

    def west(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
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

    def north_west(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate that is located in north-western direction of a
        given coordinate `c`. Depending on the arrangement of the layout, the
        dimension values of the returned coordinate may differ.

        Args:
            c: Coordinate whose north-western counterpart is desired.

        Returns:
            Coordinate directly north-western of `c`.
        """

    def above(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate that is directly above a given coordinate `c`,
        i.e., the face whose z-dimension is higher by 1. If `c`'s z-dimension
        is already at maximum, `c` is returned instead.

        Args:
            c: Coordinate whose above counterpart is desired.

        Returns:
            Coordinate directly above `c`.
        """

    def below(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate that is directly below a given coordinate `c`,
        i.e., the face whose z-dimension is lower by 1. If `c`'s z-dimension
        is already at minimum, `c` is returned instead.

        Args:
            c: Coordinate whose below counterpart is desired.

        Returns:
            Coordinate directly below `c`.
        """

    def is_north_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is directly north of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is directly north of `c1`.
        """

    def is_east_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is directly east of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is directly east of `c1`.
        """

    def is_south_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is directly south of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is directly south of `c1`.
        """

    def is_west_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is directly west of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is directly west of `c1`.
        """

    def is_adjacent_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is either north, north-east, east,
        south-east, south, south-west, west, or north-west of coordinate `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is directly adjacent to `c1` in one of the six
            different ordinal directions possible for the layout's
            arrangement.
        """

    def is_adjacent_elevation_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Similar to is_adjacent_of but also considers `c1`'s elevation, i.e.,
        if `c2` is adjacent to `above(c1)` or `below(c1)`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is either adjacent of `c1` or `c1`'s elevations.
        """

    def is_above(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is directly above coordinate `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is directly above `c1`.
        """

    def is_below(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is directly below coordinate `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is directly below `c1`.
        """

    def is_northwards_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is somewhere north of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is somewhere north of `c1`.
        """

    def is_eastwards_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is somewhere east of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is somewhere east of `c1`.
        """

    def is_southwards_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is somewhere south of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is somewhere south of `c1`.
        """

    def is_westwards_of(
        self,
        c1: coordinate | tuple[int, int] | tuple[int, int, int],
        c2: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> bool:
        """
        Returns `true` iff coordinate `c2` is somewhere west of coordinate
        `c1`.

        Args:
            c1: Base coordinate.
            c2: Coordinate to test for its location in relation to `c1`.

        Returns:
            `true` iff `c2` is somewhere west of `c1`.
        """

    def is_at_northern_border(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located at the layout's
        northern border where y is minimal.

        Args:
            c: Coordinate to check for border location.

        Returns:
            `true` iff `c` is located at the layout's northern border.
        """

    def is_at_eastern_border(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located at the layout's
        eastern border where x is maximal.

        Args:
            c: Coordinate to check for border location.

        Returns:
            `true` iff `c` is located at the layout's northern border.
        """

    def is_at_southern_border(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located at the layout's
        southern border where y is maximal.

        Args:
            c: Coordinate to check for border location.

        Returns:
            `true` iff `c` is located at the layout's southern border.
        """

    def is_at_western_border(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located at the layout's
        western border where x is minimal.

        Args:
            c: Coordinate to check for border location.

        Returns:
            `true` iff `c` is located at the layout's western border.
        """

    def is_at_any_border(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located at any of the layout's
        borders where x or y are either minimal or maximal.

        Args:
            c: Coordinate to check for border location.

        Returns:
            `true` iff `c` is located at any of the layout's borders.
        """

    def northern_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate with the same x and z values as a given
        coordinate but that is located at the layout's northern border.

        Args:
            c: Coordinate whose border counterpart is desired.

        Returns:
            The northern border equivalent of `c`.
        """

    def eastern_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate with the same y and z values as a given
        coordinate but that is located at the layout's eastern border.

        Args:
            c: Coordinate whose border counterpart is desired.

        Returns:
            The eastern border equivalent of `c`.
        """

    def southern_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate with the same x and z values as a given
        coordinate but that is located at the layout's southern border.

        Args:
            c: Coordinate whose border counterpart is desired.

        Returns:
            The southern border equivalent of `c`.
        """

    def western_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate:
        """
        Returns the coordinate with the same y and z values as a given
        coordinate but that is located at the layout's western border.

        Args:
            c: Coordinate whose border counterpart is desired.

        Returns:
            The western border equivalent of `c`.
        """

    def is_ground_layer(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in the ground layer
        where z is minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in ground layer.
        """

    def is_crossing_layer(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located in a crossing layer
        where z is not minimal.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c` is in a crossing layer.
        """

    def is_within_bounds(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the given coordinate is located within the layout
        bounds.

        Args:
            c: Coordinate to check for boundary.

        Returns:
            `true` iff `c` is located within the layout bounds.
        """

    def coordinates(self) -> list[coordinate]:
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

    def ground_coordinates(self) -> list[coordinate]:
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

    def adjacent_coordinates(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]:
        """
        Returns a container that contains all coordinates that are adjacent to
        a given one. Thereby, cardinal and ordinal directions are being
        considered, i.e., the container will contain all coordinates `ac` for
        which `is_adjacent(c, ac)` returns `true`.

        Coordinates that are outside of the layout bounds are not considered.
        Thereby, the size of the returned container is at most 6.

        Args:
            c: Coordinate whose adjacent ones are desired.

        Returns:
            A container that contains all of `c`'s adjacent coordinates.
        """

    def adjacent_opposite_coordinates(
        self, c: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> list[tuple[coordinate, coordinate]]:
        """
        Returns a container that contains all coordinates pairs of opposing
        adjacent coordinates with respect to a given one. In this hexagonal
        layout, the container content depends on the arrangement.

        In case of a row arrangement (pointy-top), the container will contain
        (`east(c)`, `west(c)`), (`north_east(c)`, `south_west(c)`),
        (`north_west(c)`, `south_east(c)`). In case of a column arrangement
        (flat-top), the container will contain (`north(c)`, `south(c)`),
        (`north_east(c)`, `south_west(c)`), (`north_west(c)`, `south_east(c)`)
        instead.

        This function comes in handy when straight lines on the layout are to
        be examined.

        Coordinates outside of the layout bounds are not being considered.

        Args:
            c: Coordinate whose opposite ones are desired.

        Returns:
            A container that contains pairs of `c`'s opposing coordinates.
        """

class cartesian_gate_layout(cartesian_layout):
    """
    A gate-level FCN layout owns gates, clocking, synchronization delays,
    and persistent obstructions. Clock zones are tiles in the coordinate
    geometry supplied by `CoordinateLayout`. The gate_level_layout class
    fulfills the requirements of a `mockturtle` logic network so that it
    can be used in many of `mockturtle`'s algorithms. Since a layout has
    to assign fixed positions to its gates (logic nodes), most generative
    member functions like `create_pi`, `create_po`, `create_and`, etc.
    require additional coordinate parameters. Consequently, `mockturtle`'s
    algorithms cannot be used to generate gate_level_layout networks. To
    make the class compliant with the API anyways, these member functions
    have their parameters defaulted but they are, in fact required to
    create meaningful layouts.

    The following notion is utilized in this implementation:
    - a node `n` is an index representing the `n`th created gate. All
      properties of said gate, e.g., its type and
    position, are stored independently and can be requested from the
    layout. An empty layout has 2 nodes, namely `const0` and `const1` as
    required by `mockturtle`. At the moment, they are not used for
    anything meaningful but could be.

    - a signal is an unsigned integer representation of a `tile`, i.e., a
      coordinate in the layout. It can be seen as a
    pointer to a position. Consequently, the utilized coordinates need to
    be convertible to `uint64_t`.

    - the creation of PIs and POs creates nodes (the latter in contrast to
      other `mockturtle` networks) that have a
    position on the layout.

    - the creation of buffers (`create_buf`) creates nodes as well. A
      buffer with more than one output is a fanout such
    that `is_fanout` will return `true` on it. However, it is also still a
    buffer (`is_buf` returns `true` as well). Buffers and wires are used
    interchangeably.

    - each node has an associated gate function. PIs, POs, and buffers
      compute the identity function.

    - signals (pointers to tiles) cannot be inverting. Thereby, inverter
      nodes (gates) have to be created that can be
    checked for via is_inv.

    - each `create_...` function requires a tile parameter that determines
      its placement. If the provided tile is dead,
    the location will not be stored and the node will not count towards
    number of gates or wires.

    - a node can be overwritten by creating another node on its location.
      This can, however, lead to unwanted effects and
    should be avoided.

    - nodes can be moved via the `move_node` function. This function can
      also be used to update their children, i.e.,
    incoming signals.

    Most implementation details regarding `mockturtle`-specific functions
    are borrowed from `mockturtle/networks/klut.hpp`. Therefore,
    `mockturtle` API functions are only sporadically documented where
    their behavior might differ. Information on their functionality can be
    found in `mockturtle`'s docs.

    Template Args:
        CoordinateLayout: Coordinate geometry used for gate placement.
    """

    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(self, dimension: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
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
        dimension: coordinate | tuple[int, int] | tuple[int, int, int],
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

    def assign_clock_number(self, cz: coordinate | tuple[int, int] | tuple[int, int, int], cn: int) -> None:
        """
        Overrides the clock number of a tile in the stored scheme. The clock
        number applies to every layer of the tile, so the z-coordinate of `cz`
        is ignored.

        Args:
            cz: Clock zone to override.
            cn: New clock number for `cz`.
        """

    def get_clock_number(self, cz: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
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
        cz1: coordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coordinate | tuple[int, int] | tuple[int, int, int],
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
        cz1: coordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coordinate | tuple[int, int] | tuple[int, int, int],
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

    def incoming_clocked_zones(self, cz: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]:
        """
        Returns a container with all clock zones that are incoming to the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are incoming to `cz`.
        """

    def outgoing_clocked_zones(self, cz: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]:
        """
        Returns a container with all clock zones that are outgoing from the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are outgoing from `cz`.
        """

    def in_degree(self, cz: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of incoming clock zones to the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s incoming clock zones.
        """

    def out_degree(self, cz: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of outgoing clock zones from the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s outgoing clock zones.
        """

    def degree(self, cz: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
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

    def obstruct_coordinate(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Marks the given coordinate as obstructed.

        Args:
            c: clock_zone to obstruct.
        """

    def obstruct_connection(
        self,
        src: coordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coordinate | tuple[int, int] | tuple[int, int, int],
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

    def clear_obstructed_coordinate(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Clears the obstruction status of the given coordinate `c` if the
        obstruction was manually marked via `obstruct_coordinate`.

        Args:
            c: clock_zone to clear.
        """

    def clear_obstructed_connection(
        self,
        src: coordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coordinate | tuple[int, int] | tuple[int, int, int],
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

    def is_obstructed_coordinate(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks if the given coordinate is obstructed of some sort.

        Args:
            c: Coordinate to check.

        Returns:
            `true` iff `c` is obstructed.
        """

    def is_obstructed_connection(
        self,
        src: coordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coordinate | tuple[int, int] | tuple[int, int, int],
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

    def create_pi(self, name: str = "", t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_po(
        self, s: int, name: str = "", t: coordinate | tuple[int, int] | tuple[int, int, int] = ...
    ) -> int: ...
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

    def is_pi_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary input.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PI.
        """

    def is_po_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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
    def get_layout_name(self) -> str: ...
    def clone(self) -> cartesian_gate_layout:
        """
        Clones the layout returning a deep copy.

        Returns:
            Deep copy of the layout.
        """

    def set_input_name(self, index: int, name: str) -> None: ...
    def get_input_name(self, index: int) -> str: ...
    def set_output_name(self, index: int, name: str) -> None: ...
    def get_output_name(self, index: int) -> str: ...
    def get_name(self, s: int) -> str: ...
    def create_buf(self, a: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_not(self, a: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_and(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_nand(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_or(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_nor(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_xor(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_xnor(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_lt(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_le(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_gt(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_ge(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_maj(
        self, a: int, b: int, c: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...
    ) -> int: ...
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

    def get_node(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Fetches the node that is placed onto a tile pointed to by a given
        signal. If no node is placed there, the `const0` node is returned.

        Args:
            s: Pointer to a tile.

        Returns:
            Node at position `t` where `s` points at `t`; or 0 if no node is
            placed at `t`.
        """

    def get_tile(self, n: int) -> coordinate:
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

    def make_signal(self, n: int) -> int:
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
        self, n: int, t: coordinate | tuple[int, int] | tuple[int, int, int], new_children: Sequence[int] = []
    ) -> int:
        """
        Moves a given node to a new position and also updates its children,
        i.e., incoming signals.

        Args:
            n: Node to move.
            t: Tile to move `n` to.
            new_children: New incoming signals to `n`.

        Returns:
            Signal pointing to `n`'s new tile.

        Raises:
            std::out_of_range: If `t` has no signal encoding.
        """

    def clear_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
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

    def is_gate_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_gate` (in
        accordance with `mockturtle`'s definition of gates).

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that is a neither a constant nor a PI.
        """

    def is_wire_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_wire`.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that computes the identity.
        """

    def is_empty_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether `t` does not have a node assigned to it.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` is an empty tile.
        """

    def pis(self) -> list[coordinate]: ...
    def pos(self) -> list[coordinate]: ...
    def gates(self) -> list[coordinate]: ...
    def wires(self) -> list[coordinate]: ...
    def fanins(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]: ...
    def fanouts(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]: ...
    def is_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int], s: int) -> bool:
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

    def has_no_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_northern_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_north_eastern_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_eastern_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_south_eastern_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_southern_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_south_western_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_western_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_north_western_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def is_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int], s: int) -> bool:
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

    def has_no_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_northern_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_north_eastern_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_eastern_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_south_eastern_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_southern_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_south_western_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_western_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_north_western_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def bounding_box_2d(self) -> tuple[coordinate, coordinate]:
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
        self, coordinate: coordinate | tuple[int, int] | tuple[int, int, int], delay: int
    ) -> None:
        """
        Assigns a synchronization element to the provided clock zone.

        Args:
            cz: Clock zone to turn into a synchronization element.
            se: Number of full clock cycles to extend `cz`'s Hold phase by. If
                this value is 0, `cz` is turned back into a normal clock zone.
        """

    def is_synchronization_element(self, coordinate: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether the provided clock zone is a synchronization element.

        Args:
            cz: Clock zone to check.

        Returns:
            `true` iff `cz` is a synchronization element.
        """

    def get_synchronization_element(self, coordinate: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
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

class shifted_cartesian_gate_layout(shifted_cartesian_layout):
    """
    A gate-level FCN layout owns gates, clocking, synchronization delays,
    and persistent obstructions. Clock zones are tiles in the coordinate
    geometry supplied by `CoordinateLayout`. The gate_level_layout class
    fulfills the requirements of a `mockturtle` logic network so that it
    can be used in many of `mockturtle`'s algorithms. Since a layout has
    to assign fixed positions to its gates (logic nodes), most generative
    member functions like `create_pi`, `create_po`, `create_and`, etc.
    require additional coordinate parameters. Consequently, `mockturtle`'s
    algorithms cannot be used to generate gate_level_layout networks. To
    make the class compliant with the API anyways, these member functions
    have their parameters defaulted but they are, in fact required to
    create meaningful layouts.

    The following notion is utilized in this implementation:
    - a node `n` is an index representing the `n`th created gate. All
      properties of said gate, e.g., its type and
    position, are stored independently and can be requested from the
    layout. An empty layout has 2 nodes, namely `const0` and `const1` as
    required by `mockturtle`. At the moment, they are not used for
    anything meaningful but could be.

    - a signal is an unsigned integer representation of a `tile`, i.e., a
      coordinate in the layout. It can be seen as a
    pointer to a position. Consequently, the utilized coordinates need to
    be convertible to `uint64_t`.

    - the creation of PIs and POs creates nodes (the latter in contrast to
      other `mockturtle` networks) that have a
    position on the layout.

    - the creation of buffers (`create_buf`) creates nodes as well. A
      buffer with more than one output is a fanout such
    that `is_fanout` will return `true` on it. However, it is also still a
    buffer (`is_buf` returns `true` as well). Buffers and wires are used
    interchangeably.

    - each node has an associated gate function. PIs, POs, and buffers
      compute the identity function.

    - signals (pointers to tiles) cannot be inverting. Thereby, inverter
      nodes (gates) have to be created that can be
    checked for via is_inv.

    - each `create_...` function requires a tile parameter that determines
      its placement. If the provided tile is dead,
    the location will not be stored and the node will not count towards
    number of gates or wires.

    - a node can be overwritten by creating another node on its location.
      This can, however, lead to unwanted effects and
    should be avoided.

    - nodes can be moved via the `move_node` function. This function can
      also be used to update their children, i.e.,
    incoming signals.

    Most implementation details regarding `mockturtle`-specific functions
    are borrowed from `mockturtle/networks/klut.hpp`. Therefore,
    `mockturtle` API functions are only sporadically documented where
    their behavior might differ. Information on their functionality can be
    found in `mockturtle`'s docs.

    Template Args:
        CoordinateLayout: Coordinate geometry used for gate placement.
    """

    @overload
    def __init__(self, arrangement: arrangement) -> None: ...
    @overload
    def __init__(
        self, arrangement: arrangement, dimension: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> None:
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
        arrangement: arrangement,
        dimension: coordinate | tuple[int, int] | tuple[int, int, int],
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

    def assign_clock_number(self, cz: coordinate | tuple[int, int] | tuple[int, int, int], cn: int) -> None:
        """
        Overrides the clock number of a tile in the stored scheme. The clock
        number applies to every layer of the tile, so the z-coordinate of `cz`
        is ignored.

        Args:
            cz: Clock zone to override.
            cn: New clock number for `cz`.
        """

    def get_clock_number(self, cz: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
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
        cz1: coordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coordinate | tuple[int, int] | tuple[int, int, int],
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
        cz1: coordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coordinate | tuple[int, int] | tuple[int, int, int],
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

    def incoming_clocked_zones(self, cz: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]:
        """
        Returns a container with all clock zones that are incoming to the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are incoming to `cz`.
        """

    def outgoing_clocked_zones(self, cz: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]:
        """
        Returns a container with all clock zones that are outgoing from the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are outgoing from `cz`.
        """

    def in_degree(self, cz: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of incoming clock zones to the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s incoming clock zones.
        """

    def out_degree(self, cz: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of outgoing clock zones from the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s outgoing clock zones.
        """

    def degree(self, cz: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
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

    def obstruct_coordinate(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Marks the given coordinate as obstructed.

        Args:
            c: clock_zone to obstruct.
        """

    def obstruct_connection(
        self,
        src: coordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coordinate | tuple[int, int] | tuple[int, int, int],
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

    def clear_obstructed_coordinate(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Clears the obstruction status of the given coordinate `c` if the
        obstruction was manually marked via `obstruct_coordinate`.

        Args:
            c: clock_zone to clear.
        """

    def clear_obstructed_connection(
        self,
        src: coordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coordinate | tuple[int, int] | tuple[int, int, int],
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

    def is_obstructed_coordinate(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks if the given coordinate is obstructed of some sort.

        Args:
            c: Coordinate to check.

        Returns:
            `true` iff `c` is obstructed.
        """

    def is_obstructed_connection(
        self,
        src: coordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coordinate | tuple[int, int] | tuple[int, int, int],
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

    def create_pi(self, name: str = "", t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_po(
        self, s: int, name: str = "", t: coordinate | tuple[int, int] | tuple[int, int, int] = ...
    ) -> int: ...
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

    def is_pi_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary input.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PI.
        """

    def is_po_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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
    def get_layout_name(self) -> str: ...
    def clone(self) -> shifted_cartesian_gate_layout:
        """
        Clones the layout returning a deep copy.

        Returns:
            Deep copy of the layout.
        """

    def set_input_name(self, index: int, name: str) -> None: ...
    def get_input_name(self, index: int) -> str: ...
    def set_output_name(self, index: int, name: str) -> None: ...
    def get_output_name(self, index: int) -> str: ...
    def get_name(self, s: int) -> str: ...
    def create_buf(self, a: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_not(self, a: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_and(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_nand(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_or(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_nor(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_xor(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_xnor(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_lt(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_le(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_gt(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_ge(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_maj(
        self, a: int, b: int, c: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...
    ) -> int: ...
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

    def get_node(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Fetches the node that is placed onto a tile pointed to by a given
        signal. If no node is placed there, the `const0` node is returned.

        Args:
            s: Pointer to a tile.

        Returns:
            Node at position `t` where `s` points at `t`; or 0 if no node is
            placed at `t`.
        """

    def get_tile(self, n: int) -> coordinate:
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

    def make_signal(self, n: int) -> int:
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
        self, n: int, t: coordinate | tuple[int, int] | tuple[int, int, int], new_children: Sequence[int] = []
    ) -> int:
        """
        Moves a given node to a new position and also updates its children,
        i.e., incoming signals.

        Args:
            n: Node to move.
            t: Tile to move `n` to.
            new_children: New incoming signals to `n`.

        Returns:
            Signal pointing to `n`'s new tile.

        Raises:
            std::out_of_range: If `t` has no signal encoding.
        """

    def clear_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
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

    def is_gate_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_gate` (in
        accordance with `mockturtle`'s definition of gates).

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that is a neither a constant nor a PI.
        """

    def is_wire_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_wire`.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that computes the identity.
        """

    def is_empty_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether `t` does not have a node assigned to it.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` is an empty tile.
        """

    def pis(self) -> list[coordinate]: ...
    def pos(self) -> list[coordinate]: ...
    def gates(self) -> list[coordinate]: ...
    def wires(self) -> list[coordinate]: ...
    def fanins(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]: ...
    def fanouts(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]: ...
    def is_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int], s: int) -> bool:
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

    def has_no_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_northern_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_north_eastern_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_eastern_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_south_eastern_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_southern_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_south_western_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_western_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_north_western_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def is_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int], s: int) -> bool:
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

    def has_no_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_northern_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_north_eastern_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_eastern_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_south_eastern_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_southern_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_south_western_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_western_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_north_western_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def bounding_box_2d(self) -> tuple[coordinate, coordinate]:
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
        self, coordinate: coordinate | tuple[int, int] | tuple[int, int, int], delay: int
    ) -> None:
        """
        Assigns a synchronization element to the provided clock zone.

        Args:
            cz: Clock zone to turn into a synchronization element.
            se: Number of full clock cycles to extend `cz`'s Hold phase by. If
                this value is 0, `cz` is turned back into a normal clock zone.
        """

    def is_synchronization_element(self, coordinate: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether the provided clock zone is a synchronization element.

        Args:
            cz: Clock zone to check.

        Returns:
            `true` iff `cz` is a synchronization element.
        """

    def get_synchronization_element(self, coordinate: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
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

class hexagonal_gate_layout(hexagonal_layout):
    """
    A gate-level FCN layout owns gates, clocking, synchronization delays,
    and persistent obstructions. Clock zones are tiles in the coordinate
    geometry supplied by `CoordinateLayout`. The gate_level_layout class
    fulfills the requirements of a `mockturtle` logic network so that it
    can be used in many of `mockturtle`'s algorithms. Since a layout has
    to assign fixed positions to its gates (logic nodes), most generative
    member functions like `create_pi`, `create_po`, `create_and`, etc.
    require additional coordinate parameters. Consequently, `mockturtle`'s
    algorithms cannot be used to generate gate_level_layout networks. To
    make the class compliant with the API anyways, these member functions
    have their parameters defaulted but they are, in fact required to
    create meaningful layouts.

    The following notion is utilized in this implementation:
    - a node `n` is an index representing the `n`th created gate. All
      properties of said gate, e.g., its type and
    position, are stored independently and can be requested from the
    layout. An empty layout has 2 nodes, namely `const0` and `const1` as
    required by `mockturtle`. At the moment, they are not used for
    anything meaningful but could be.

    - a signal is an unsigned integer representation of a `tile`, i.e., a
      coordinate in the layout. It can be seen as a
    pointer to a position. Consequently, the utilized coordinates need to
    be convertible to `uint64_t`.

    - the creation of PIs and POs creates nodes (the latter in contrast to
      other `mockturtle` networks) that have a
    position on the layout.

    - the creation of buffers (`create_buf`) creates nodes as well. A
      buffer with more than one output is a fanout such
    that `is_fanout` will return `true` on it. However, it is also still a
    buffer (`is_buf` returns `true` as well). Buffers and wires are used
    interchangeably.

    - each node has an associated gate function. PIs, POs, and buffers
      compute the identity function.

    - signals (pointers to tiles) cannot be inverting. Thereby, inverter
      nodes (gates) have to be created that can be
    checked for via is_inv.

    - each `create_...` function requires a tile parameter that determines
      its placement. If the provided tile is dead,
    the location will not be stored and the node will not count towards
    number of gates or wires.

    - a node can be overwritten by creating another node on its location.
      This can, however, lead to unwanted effects and
    should be avoided.

    - nodes can be moved via the `move_node` function. This function can
      also be used to update their children, i.e.,
    incoming signals.

    Most implementation details regarding `mockturtle`-specific functions
    are borrowed from `mockturtle/networks/klut.hpp`. Therefore,
    `mockturtle` API functions are only sporadically documented where
    their behavior might differ. Information on their functionality can be
    found in `mockturtle`'s docs.

    Template Args:
        CoordinateLayout: Coordinate geometry used for gate placement.
    """

    @overload
    def __init__(self, arrangement: arrangement) -> None: ...
    @overload
    def __init__(
        self, arrangement: arrangement, dimension: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> None:
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
        arrangement: arrangement,
        dimension: coordinate | tuple[int, int] | tuple[int, int, int],
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

    def assign_clock_number(self, cz: coordinate | tuple[int, int] | tuple[int, int, int], cn: int) -> None:
        """
        Overrides the clock number of a tile in the stored scheme. The clock
        number applies to every layer of the tile, so the z-coordinate of `cz`
        is ignored.

        Args:
            cz: Clock zone to override.
            cn: New clock number for `cz`.
        """

    def get_clock_number(self, cz: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
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
        cz1: coordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coordinate | tuple[int, int] | tuple[int, int, int],
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
        cz1: coordinate | tuple[int, int] | tuple[int, int, int],
        cz2: coordinate | tuple[int, int] | tuple[int, int, int],
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

    def incoming_clocked_zones(self, cz: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]:
        """
        Returns a container with all clock zones that are incoming to the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are incoming to `cz`.
        """

    def outgoing_clocked_zones(self, cz: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]:
        """
        Returns a container with all clock zones that are outgoing from the
        given one.

        Args:
            cz: Base clock zone.

        Returns:
            A container with all clock zones that are outgoing from `cz`.
        """

    def in_degree(self, cz: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of incoming clock zones to the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s incoming clock zones.
        """

    def out_degree(self, cz: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Returns the number of outgoing clock zones from the given one.

        Args:
            cz: Base clock zone.

        Returns:
            Number of `cz`'s outgoing clock zones.
        """

    def degree(self, cz: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
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

    def obstruct_coordinate(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Marks the given coordinate as obstructed.

        Args:
            c: clock_zone to obstruct.
        """

    def obstruct_connection(
        self,
        src: coordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coordinate | tuple[int, int] | tuple[int, int, int],
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

    def clear_obstructed_coordinate(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Clears the obstruction status of the given coordinate `c` if the
        obstruction was manually marked via `obstruct_coordinate`.

        Args:
            c: clock_zone to clear.
        """

    def clear_obstructed_connection(
        self,
        src: coordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coordinate | tuple[int, int] | tuple[int, int, int],
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

    def is_obstructed_coordinate(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks if the given coordinate is obstructed of some sort.

        Args:
            c: Coordinate to check.

        Returns:
            `true` iff `c` is obstructed.
        """

    def is_obstructed_connection(
        self,
        src: coordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coordinate | tuple[int, int] | tuple[int, int, int],
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

    def create_pi(self, name: str = "", t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_po(
        self, s: int, name: str = "", t: coordinate | tuple[int, int] | tuple[int, int, int] = ...
    ) -> int: ...
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

    def is_pi_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether tile `t` hosts a primary input.

        Args:
            t: Tile to be checked.

        Returns:
            `true` iff the node located at tile `t` is a PI.
        """

    def is_po_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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
    def get_layout_name(self) -> str: ...
    def clone(self) -> hexagonal_gate_layout:
        """
        Clones the layout returning a deep copy.

        Returns:
            Deep copy of the layout.
        """

    def set_input_name(self, index: int, name: str) -> None: ...
    def get_input_name(self, index: int) -> str: ...
    def set_output_name(self, index: int, name: str) -> None: ...
    def get_output_name(self, index: int) -> str: ...
    def get_name(self, s: int) -> str: ...
    def create_buf(self, a: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_not(self, a: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_and(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_nand(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_or(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_nor(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_xor(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_xnor(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_lt(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_le(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_gt(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_ge(self, a: int, b: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...) -> int: ...
    def create_maj(
        self, a: int, b: int, c: int, t: coordinate | tuple[int, int] | tuple[int, int, int] = ...
    ) -> int: ...
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

    def get_node(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
        """
        Fetches the node that is placed onto a tile pointed to by a given
        signal. If no node is placed there, the `const0` node is returned.

        Args:
            s: Pointer to a tile.

        Returns:
            Node at position `t` where `s` points at `t`; or 0 if no node is
            placed at `t`.
        """

    def get_tile(self, n: int) -> coordinate:
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

    def make_signal(self, n: int) -> int:
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
        self, n: int, t: coordinate | tuple[int, int] | tuple[int, int, int], new_children: Sequence[int] = []
    ) -> int:
        """
        Moves a given node to a new position and also updates its children,
        i.e., incoming signals.

        Args:
            n: Node to move.
            t: Tile to move `n` to.
            new_children: New incoming signals to `n`.

        Returns:
            Signal pointing to `n`'s new tile.

        Raises:
            std::out_of_range: If `t` has no signal encoding.
        """

    def clear_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
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

    def is_gate_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_gate` (in
        accordance with `mockturtle`'s definition of gates).

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that is a neither a constant nor a PI.
        """

    def is_wire_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether the node assigned to `t` fulfills `is_wire`.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` hosts a node that computes the identity.
        """

    def is_empty_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Returns whether `t` does not have a node assigned to it.

        Args:
            t: Tile to check.

        Returns:
            `true` iff `t` is an empty tile.
        """

    def pis(self) -> list[coordinate]: ...
    def pos(self) -> list[coordinate]: ...
    def gates(self) -> list[coordinate]: ...
    def wires(self) -> list[coordinate]: ...
    def fanins(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]: ...
    def fanouts(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]: ...
    def is_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int], s: int) -> bool:
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

    def has_no_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_northern_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_north_eastern_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_eastern_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_south_eastern_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_southern_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_south_western_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_western_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_north_western_incoming_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def is_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int], s: int) -> bool:
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

    def has_no_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_northern_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_north_eastern_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_eastern_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_south_eastern_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_southern_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_south_western_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_western_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def has_north_western_outgoing_signal(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

    def bounding_box_2d(self) -> tuple[coordinate, coordinate]:
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
        self, coordinate: coordinate | tuple[int, int] | tuple[int, int, int], delay: int
    ) -> None:
        """
        Assigns a synchronization element to the provided clock zone.

        Args:
            cz: Clock zone to turn into a synchronization element.
            se: Number of full clock cycles to extend `cz`'s Hold phase by. If
                this value is 0, `cz` is turned back into a normal clock zone.
        """

    def is_synchronization_element(self, coordinate: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Check whether the provided clock zone is a synchronization element.

        Args:
            cz: Clock zone to check.

        Returns:
            `true` iff `cz` is a synchronization element.
        """

    def get_synchronization_element(self, coordinate: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
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

class obstructions:
    """
    Explicit obstructions stored by layouts or supplied to a routing
    search.

    Copies are independent. This object contains no layout or occupancy
    information.
    """

    def __init__(self) -> None:
        """Creates empty routing constraints."""

    def obstruct_coordinate(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Marks the given coordinate as obstructed.

        Args:
            c: Coordinate to obstruct.
        """

    def obstruct_connection(
        self,
        src: coordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coordinate | tuple[int, int] | tuple[int, int, int],
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

    def clear_obstructed_coordinate(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Clears the obstruction status of the given coordinate `c` if the
        obstruction was manually marked via `obstruct_coordinate`.

        Args:
            c: Coordinate to clear.
        """

    def clear_obstructed_connection(
        self,
        src: coordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coordinate | tuple[int, int] | tuple[int, int, int],
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

    def is_obstructed_coordinate(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Checks if the given coordinate is obstructed of some sort.

        Args:
            c: Coordinate to check.

        Returns:
            `true` iff `c` is obstructed.
        """

    def is_obstructed_connection(
        self,
        src: coordinate | tuple[int, int] | tuple[int, int, int],
        tgt: coordinate | tuple[int, int] | tuple[int, int, int],
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
def num_adjacent_coordinates(
    lyt: cartesian_gate_layout, c: coordinate | tuple[int, int] | tuple[int, int, int]
) -> int: ...
@overload
def num_adjacent_coordinates(
    lyt: shifted_cartesian_gate_layout, c: coordinate | tuple[int, int] | tuple[int, int, int]
) -> int: ...
@overload
def num_adjacent_coordinates(lyt: hexagonal_gate_layout, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> int:
    """
    Returns the number of adjacent coordinates of a given one. This is not
    a constant value because `c` could be located at a layout border.

    Args:
        lyt: Layout.
        c: Coordinate whose number of adjacencies are required.

    Template Args:
        Lyt: Layout type.

    Returns:
        Number of `c`'s adjacent coordinates.
    """

@overload
def normalize_layout_coordinates(lyt: mnt.pyfiction.qca.qca_layout) -> mnt.pyfiction.qca.qca_layout: ...
@overload
def normalize_layout_coordinates(lyt: mnt.pyfiction.mol_qca.mol_qca_layout) -> mnt.pyfiction.mol_qca.mol_qca_layout: ...
@overload
def normalize_layout_coordinates(lyt: mnt.pyfiction.inml.inml_layout) -> mnt.pyfiction.inml.inml_layout:
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

def random_coordinate(
    coordinate1: coordinate | tuple[int, int] | tuple[int, int, int],
    coordinate_2: coordinate | tuple[int, int] | tuple[int, int, int],
) -> coordinate:
    """
    Generates a random coordinate within the region spanned by two given
    coordinates. The two given coordinates form the top left corner and
    the bottom right corner of the spanned region.

    Args:
        coordinate1: Top left Coordinate.
        coordinate2: Bottom right Coordinate (coordinate order is not
                     important, automatically swapped if necessary).

    Template Args:
        CoordinateType: The coordinate implementation to be used.

    Returns:
        Randomly generated coordinate.
    """
