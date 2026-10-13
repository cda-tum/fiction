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
from typing import overload

import mnt.pyfiction.inml
import mnt.pyfiction.mol_qca
import mnt.pyfiction.qca
import mnt.pyfiction.synthesis
from mnt.pyfiction.layouts import io as io

class coordinate:
    """
    Signed coordinates.

    A coordinate defines a location relative to a fixed point (origin).
    Each axis is a signed 32-bit integer. The default-constructed
    coordinate is the origin. Every signed 32-bit axis value identifies a
    position.
    """

    @overload
    def __init__(self) -> None:
        """Default constructor. Creates the origin."""

    @overload
    def __init__(self, x: int, y: int, z: int = 0) -> None:
        """
        Standard constructor. Creates a coordinate at (coordinate_x,
        coordinate_y, coordinate_z).

        Args:
            coordinate_x: x position.
            coordinate_y: y position.
            coordinate_z: z position.

        Template Args:
            X: Type of x.
            Y: Type of y.
            Z: Type of z.

        Raises:
            std::overflow_error: If an axis is outside the signed 32-bit
                                 range.
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
    def __eq__(self, other: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Compares against another coordinate for equality, axis by axis.

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

class extent:
    """Nonnegative width, height, and layer counts of a zero-origin layout."""

    @overload
    def __init__(self) -> None:
        """Creates an empty extent."""

    @overload
    def __init__(self, width: int, height: int, layers: int = 1) -> None:
        """
        Creates checked sizes. Two axes describe one layer. Each size lies between zero and 2147483648.
        """

    @overload
    def __init__(self, extent: extent | tuple[int, int] | tuple[int, int, int]) -> None: ...
    @overload
    def __init__(self, extent: tuple[int, int] | tuple[int, int, int]) -> None: ...
    @property
    def width(self) -> int:
        """Checked width in coordinates."""

    @width.setter
    def width(self, arg: int, /) -> None: ...
    @property
    def height(self) -> int:
        """Checked height in coordinates."""

    @height.setter
    def height(self, arg: int, /) -> None: ...
    @property
    def layers(self) -> int:
        """Checked number of layers."""

    @layers.setter
    def layers(self, arg: int, /) -> None: ...
    def __eq__(self, other: extent | tuple[int, int] | tuple[int, int, int]) -> bool: ...

    __hash__: None = None

def area(extent: extent | tuple[int, int] | tuple[int, int, int]) -> int:
    """
    Computes width times height.

    Args:
        extent: Axis sizes.

    Returns:
        Area.
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
    def __init__(self, extent: extent | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Creates geometry with half-open, zero-origin bounds. The default
        extent is empty.

        Args:
            extent: Axis sizes.

        Raises:
            std::invalid_argument: If a size exceeds the coordinate domain.
            std::out_of_range: If the layer count exceeds two.
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

        Raises:
            std::overflow_error: If an axis is outside the signed 32-bit
                                 range.

        Note:
            This function is equivalent to calling `coordinate(x, y, z)`.
        """

    def width(self) -> int:
        """Returns the width count."""

    def height(self) -> int:
        """Returns the height count."""

    def layers(self) -> int:
        """Returns the layers count."""

    def get_extent(self) -> extent:
        """Returns the layout extent."""

    def last_coordinate(self) -> coordinate | None:
        """Returns the last coordinate, or None for empty geometry."""

    def contains_coordinate(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """Tests the half-open geometry bounds."""

    def area(self) -> int:
        """
        Returns:
            Width times height.
        """

    def resize(self, extent: extent | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Changes the geometry's axis sizes.

        Args:
            extent: Axis sizes.

        Raises:
            std::invalid_argument: If a size exceeds the coordinate domain.
            std::out_of_range: If the layer count exceeds two. The dimensions
                               remain unchanged.
        """

    def north(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the north neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def north_east(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the north-east neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def east(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the east neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def south_east(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the south-east neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def south(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the south neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def south_west(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the south-west neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def west(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the west neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def north_west(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the north-west neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def above(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the above neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def below(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the below neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
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

    def northern_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Projects a coordinate to the northern border.

        Args:
            c: Coordinate to project.

        Returns:
            Projection, or no value if the projection lies outside the
            geometry.
        """

    def eastern_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Projects a coordinate to the eastern border.

        Args:
            c: Coordinate to project.

        Returns:
            Projection, or no value if the projection lies outside the
            geometry.
        """

    def southern_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Projects a coordinate to the southern border.

        Args:
            c: Coordinate to project.

        Returns:
            Projection, or no value if the projection lies outside the
            geometry.
        """

    def western_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Projects a coordinate to the western border.

        Args:
            c: Coordinate to project.

        Returns:
            Projection, or no value if the projection lies outside the
            geometry.
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
        Returns whether the given coordinate is located in the crossing layer
        at z = 1.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c.z` is 1.
        """

    def is_within_bounds(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Args:
            c: Coordinate. @return Whether the geometry contains the
               coordinate.
        """

    def coordinates(
        self,
        start: coordinate | tuple[int, int] | tuple[int, int, int] | None = None,
        stop: coordinate | tuple[int, int] | tuple[int, int, int] | None = None,
    ) -> list[coordinate]:
        """
        Returns coordinates in z/y/x order from the inclusive start to the exclusive stop. None uses the frame boundary.
        """

    def ground_coordinates(
        self,
        start: coordinate | tuple[int, int] | tuple[int, int, int] | None = None,
        stop: coordinate | tuple[int, int] | tuple[int, int, int] | None = None,
    ) -> list[coordinate]:
        """
        Returns layer-zero coordinates from the inclusive start to the exclusive stop. None uses the frame boundary. Bounds outside layer zero raise ValueError.
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

    def __copy__(self) -> cartesian_layout:
        """Returns an independent geometry copy."""

    def __deepcopy__(self, memo: dict) -> cartesian_layout:
        """Returns an independent geometry copy."""

class shifted_cartesian_layout:
    """
    A layout type that utilizes offset coordinates to represent a
    Cartesian layout with shifted rows or columns selected by its arrangement.
    This example uses arrangement.ODD_COLUMN:

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
    def __init__(self, arrangement: arrangement, extent: extent | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Creates geometry with half-open, zero-origin bounds. The default
        extent is empty.

        Args:
            a: Arrangement of shifted rows or columns.
            extent: Axis sizes.

        Raises:
            std::invalid_argument: If a size exceeds the coordinate domain.
            std::out_of_range: If the layer count exceeds two.
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

        Raises:
            std::overflow_error: If an axis is outside the signed 32-bit
                                 range.

        Note:
            This function is equivalent to calling `coordinate(x, y, z)`.
        """

    def width(self) -> int:
        """Returns the width count."""

    def height(self) -> int:
        """Returns the height count."""

    def layers(self) -> int:
        """Returns the layers count."""

    def get_extent(self) -> extent:
        """Returns the layout extent."""

    def last_coordinate(self) -> coordinate | None:
        """Returns the last coordinate, or None for empty geometry."""

    def contains_coordinate(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """Tests the half-open geometry bounds."""

    def area(self) -> int:
        """
        Returns:
            Width times height.
        """

    def resize(self, extent: extent | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Changes the geometry's axis sizes.

        Args:
            extent: Axis sizes.

        Raises:
            std::invalid_argument: If a size exceeds the coordinate domain.
            std::out_of_range: If the layer count exceeds two. The dimensions
                               remain unchanged.
        """

    def north(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the north neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def north_east(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the north-east neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def east(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the east neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def south_east(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the south-east neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def south(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the south neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def south_west(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the south-west neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def west(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the west neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def north_west(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the north-west neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def above(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the above neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def below(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the below neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
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

    def northern_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Projects a coordinate to the northern border.

        Args:
            c: Coordinate to project.

        Returns:
            Projection, or no value if the projection lies outside the
            geometry.
        """

    def eastern_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Projects a coordinate to the eastern border.

        Args:
            c: Coordinate to project.

        Returns:
            Projection, or no value if the projection lies outside the
            geometry.
        """

    def southern_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Projects a coordinate to the southern border.

        Args:
            c: Coordinate to project.

        Returns:
            Projection, or no value if the projection lies outside the
            geometry.
        """

    def western_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Projects a coordinate to the western border.

        Args:
            c: Coordinate to project.

        Returns:
            Projection, or no value if the projection lies outside the
            geometry.
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
        Returns whether the given coordinate is located in the crossing layer
        at z = 1.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c.z` is 1.
        """

    def is_within_bounds(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Args:
            c: Coordinate. @return Whether the geometry contains the
               coordinate.
        """

    def coordinates(
        self,
        start: coordinate | tuple[int, int] | tuple[int, int, int] | None = None,
        stop: coordinate | tuple[int, int] | tuple[int, int, int] | None = None,
    ) -> list[coordinate]:
        """
        Returns coordinates in z/y/x order from the inclusive start to the exclusive stop. None uses the frame boundary.
        """

    def ground_coordinates(
        self,
        start: coordinate | tuple[int, int] | tuple[int, int, int] | None = None,
        stop: coordinate | tuple[int, int] | tuple[int, int, int] | None = None,
    ) -> list[coordinate]:
        """
        Returns layer-zero coordinates from the inclusive start to the exclusive stop. None uses the frame boundary. Bounds outside layer zero raise ValueError.
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

    def __copy__(self) -> shifted_cartesian_layout:
        """Returns an independent geometry copy."""

    def __deepcopy__(self, memo: dict) -> shifted_cartesian_layout:
        """Returns an independent geometry copy."""

class hexagonal_layout:
    """
    A layout type that utilizes offset coordinates to represent a
    hexagonal grid. Its arrangement selects shifted rows or columns. Row
    arrangements use pointy-top hexagons; column arrangements use flat-top
    hexagons. This example uses arrangement.EVEN_ROW:

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
    def __init__(self, arrangement: arrangement, extent: extent | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Creates geometry with half-open, zero-origin bounds. The default
        extent is empty.

        Args:
            a: Arrangement of shifted rows or columns.
            extent: Axis sizes.

        Raises:
            std::invalid_argument: If a size exceeds the coordinate domain.
            std::out_of_range: If the layer count exceeds two.
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

        Raises:
            std::overflow_error: If an axis is outside the signed 32-bit
                                 range.

        Note:
            This function is equivalent to calling `coordinate(x, y, z)`.
        """

    def width(self) -> int:
        """Returns the width count."""

    def height(self) -> int:
        """Returns the height count."""

    def layers(self) -> int:
        """Returns the layers count."""

    def get_extent(self) -> extent:
        """Returns the layout extent."""

    def last_coordinate(self) -> coordinate | None:
        """Returns the last coordinate, or None for empty geometry."""

    def contains_coordinate(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """Tests the half-open geometry bounds."""

    def area(self) -> int:
        """
        Returns:
            Width times height.
        """

    def resize(self, extent: extent | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Changes the geometry's axis sizes.

        Args:
            extent: Axis sizes.

        Raises:
            std::invalid_argument: If a size exceeds the coordinate domain.
            std::out_of_range: If the layer count exceeds two. The dimensions
                               remain unchanged.
        """

    def north(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the north neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def north_east(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the north-east neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def east(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the east neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def south_east(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the south-east neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def south(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the south neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def south_west(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the south-west neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def west(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the west neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def north_west(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the north-west neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def above(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the above neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
        """

    def below(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Returns the below neighbor when both coordinates lie inside the
        geometry.

        Args:
            c: Base coordinate.

        Returns:
            Neighbor, or no value at a boundary or outside the geometry.
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

    def northern_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Projects a coordinate to the northern border.

        Args:
            c: Coordinate to project.

        Returns:
            Projection, or no value if the projection lies outside the
            geometry.
        """

    def eastern_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Projects a coordinate to the eastern border.

        Args:
            c: Coordinate to project.

        Returns:
            Projection, or no value if the projection lies outside the
            geometry.
        """

    def southern_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Projects a coordinate to the southern border.

        Args:
            c: Coordinate to project.

        Returns:
            Projection, or no value if the projection lies outside the
            geometry.
        """

    def western_border_of(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> coordinate | None:
        """
        Projects a coordinate to the western border.

        Args:
            c: Coordinate to project.

        Returns:
            Projection, or no value if the projection lies outside the
            geometry.
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
        Returns whether the given coordinate is located in the crossing layer
        at z = 1.

        Args:
            c: Coordinate to check for elevation.

        Returns:
            `true` iff `c.z` is 1.
        """

    def is_within_bounds(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Args:
            c: Coordinate. @return Whether the geometry contains the
               coordinate.
        """

    def coordinates(
        self,
        start: coordinate | tuple[int, int] | tuple[int, int, int] | None = None,
        stop: coordinate | tuple[int, int] | tuple[int, int, int] | None = None,
    ) -> list[coordinate]:
        """
        Returns coordinates in z/y/x order from the inclusive start to the exclusive stop. None uses the frame boundary.
        """

    def ground_coordinates(
        self,
        start: coordinate | tuple[int, int] | tuple[int, int, int] | None = None,
        stop: coordinate | tuple[int, int] | tuple[int, int, int] | None = None,
    ) -> list[coordinate]:
        """
        Returns layer-zero coordinates from the inclusive start to the exclusive stop. None uses the frame boundary. Bounds outside layer zero raise ValueError.
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

    def __copy__(self) -> hexagonal_layout:
        """Returns an independent geometry copy."""

    def __deepcopy__(self, memo: dict) -> hexagonal_layout:
        """Returns an independent geometry copy."""

class layout_object_id:
    """Layout-local generation-checked object identity."""

    def __init__(self, index: int, generation: int) -> None: ...
    @property
    def index(self) -> int: ...
    @property
    def generation(self) -> int: ...
    def __eq__(self, arg: layout_object_id, /) -> bool: ...
    def __hash__(self) -> int: ...

class layout_input_port:
    """
    A destination input port, identified by its object and truth-table argument index. A connected input port is a sink of its source object's output.
    """

    def __init__(self, object: layout_object_id, index: int) -> None: ...
    @property
    def object(self) -> layout_object_id: ...
    @property
    def index(self) -> int: ...
    def __eq__(self, arg: layout_input_port, /) -> bool: ...
    def __hash__(self) -> int: ...

class cartesian_gate_layout(cartesian_layout):
    """
    Placed FCN objects, ordered ports, clocking, and obstructions.

    Objects have stable identities independent of their coordinates.
    Connections describe declared topology; physical validation checks
    adjacency, clocking, and geometry separately. Copies own independent
    state. A sink is a destination input port driven by a source object's
    output. One destination object may receive the same source at several
    input indices; each connected input port is a distinct sink. Visitors
    may edit coordinates, names, and capabilities. Object and terminal
    visitors must not create or remove objects, change terminal order, or
    replace the layout during traversal. Connection visitors must also
    preserve the traversed input or sink connections, as specified on each
    visitor.

    Template Args:
        CoordinateLayout: Coordinate geometry used for placement.
    """

    @overload
    def __init__(self) -> None: ...
    @overload
    def __init__(self, extent: extent | tuple[int, int] | tuple[int, int, int]) -> None:
        """Creates an empty layout with the given geometry and name."""

    @overload
    def __init__(
        self,
        extent: extent | tuple[int, int] | tuple[int, int, int],
        clocking_scheme: str = "2DDWave",
        layout_name: str = "",
    ) -> None:
        """
        Creates an empty layout with the given geometry, clocking, and name.

        Raises:
            ValueError: The clocking scheme name is unknown.
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

    def obstructed_coordinates(self) -> list[coordinate]:
        """
        Returns manual coordinate obstructions in unspecified order, without implicit occupancy.
        """

    def obstructed_connections(self) -> list[tuple[coordinate, coordinate]]:
        """
        Returns manual directed-connection obstructions in unspecified order, without physical connections.
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

    def create_pi(self, name: str, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> layout_object_id:
        """
        Creates a primary input at `t`. Occupied coordinates reject without
        mutation.
        """

    @overload
    def create_po(
        self, s: layout_object_id, name: str, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a primary output driven by `s` at `t`."""

    @overload
    def create_po(self, name: str, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> layout_object_id:
        """Creates a primary output with its input disconnected."""

    def is_pi(self, n: layout_object_id) -> bool:
        """Returns whether an object is a primary input."""

    def is_po(self, n: layout_object_id) -> bool:
        """Returns whether an object is a primary output."""

    def is_pi_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """Returns whether the coordinate hosts a pi."""

    def is_po_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """Returns whether the coordinate hosts a po."""

    def is_inv(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes INV."""

    def is_and(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes AND."""

    def is_nand(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes NAND."""

    def is_or(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes OR."""

    def is_nor(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes NOR."""

    def is_xor(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes XOR."""

    def is_xnor(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes XNOR."""

    def is_lt(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes LT."""

    def is_le(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes LE."""

    def is_gt(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes GT."""

    def is_ge(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes GE."""

    def is_maj(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes MAJ."""

    def is_fanout(self, arg: layout_object_id, /) -> bool:
        """Returns whether an identity object drives more than one input port."""

    def is_buf(self, arg: layout_object_id, /) -> bool:
        """Returns whether an object computes the identity function."""

    def is_wire(self, arg: layout_object_id, /) -> bool:
        """Returns whether an object is a physical wire rather than a terminal."""

    def set_layout_name(self, name: str) -> None:
        """Sets the layout name."""

    def get_layout_name(self) -> str:
        """Returns the layout name."""

    def clone(self) -> cartesian_gate_layout:
        """Returns an independent value copy."""

    def __copy__(self) -> cartesian_gate_layout:
        """
        Returns an independent layout copy, including placed objects and metadata.
        """

    def __deepcopy__(self, memo: dict) -> cartesian_gate_layout:
        """
        Returns an independent layout copy, including placed objects and metadata.
        """

    def set_input_name(self, index: int, name: str) -> None:
        """Sets the input name at an interface index."""

    def get_input_name(self, index: int) -> str:
        """Returns the input name at an interface index."""

    def set_output_name(self, index: int, name: str) -> None:
        """Sets the output name at an interface index."""

    def get_output_name(self, index: int) -> str:
        """Returns the output name at an interface index."""

    def get_name(self, object: layout_object_id) -> str:
        """Returns an object's name, or an empty string for an unnamed object."""

    @overload
    def create_buf(
        self, a: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a wire driven by `a`."""

    @overload
    def create_buf(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> layout_object_id:
        """Creates a wire with its input disconnected."""

    def create_not(
        self, a: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a NOT gate."""

    def create_and(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a AND gate."""

    def create_nand(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a NAND gate."""

    def create_or(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a OR gate."""

    def create_nor(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a NOR gate."""

    def create_xor(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a XOR gate."""

    def create_xnor(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a XNOR gate."""

    def create_lt(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a LT gate."""

    def create_le(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a LE gate."""

    def create_gt(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a GT gate."""

    def create_ge(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a GE gate."""

    def create_maj(
        self,
        a: layout_object_id,
        b: layout_object_id,
        c: layout_object_id,
        t: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> layout_object_id:
        """Creates a majority gate."""

    def num_pis(self) -> int:
        """Counts primary inputs."""

    def num_pos(self) -> int:
        """Counts primary outputs."""

    def num_gates(self) -> int:
        """Counts logic gates, excluding wires and terminals."""

    def num_wires(self) -> int:
        """Counts physical wires, excluding primary inputs and outputs."""

    def num_crossings(self) -> int:
        """Counts crossing-layer wires above occupied ground-layer tiles."""

    def is_empty(self) -> bool:
        """Returns whether the layout has no objects."""

    def create_gate(
        self,
        inputs: Sequence[layout_object_id],
        function: mnt.pyfiction.synthesis.dynamic_truth_table,
        t: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> layout_object_id:
        """
        Creates a placed gate. Input indices follow truth-table variable order; trailing inputs may be disconnected.
        """

    def object_function(self, object: layout_object_id) -> mnt.pyfiction.synthesis.dynamic_truth_table:
        """Returns the object's truth table."""

    def size(self) -> int:
        """Counts live objects."""

    def fanin_size(self, object: layout_object_id) -> int:
        """Counts connected input slots, irrespective of physical legality."""

    def fanout_size(self, object: layout_object_id) -> int:
        """
        Counts destination input ports driven by this object's output.

        Each connected input port is a sink, including multiple ports on the
        same destination object.
        """

    def input_count(self, object: layout_object_id) -> int:
        """Returns the number of input slots, including disconnected slots."""

    def find_object(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> layout_object_id | None:
        """Finds the object at a coordinate; empty coordinates have no identity."""

    def contains(self, object: layout_object_id) -> bool:
        """Returns whether this identity names a live object."""

    def get_tile(self, object: layout_object_id) -> coordinate:
        """
        Returns the object's coordinate. @throws std::invalid_argument If the
        identity is stale.
        """

    def source(self, input: layout_input_port) -> layout_object_id | None:
        """Returns the declared source of an input, or no source if disconnected."""

    def connect(self, source: layout_object_id, input: layout_input_port) -> None:
        """
        Connects an output to an ordered input, replacing the input's existing
        source.

        Port and identity checks precede mutation. Adjacency, clocking, and
        geometry need not be valid during editing.
        """

    def disconnect(self, input: layout_input_port) -> None:
        """Disconnects one input without changing the indices of other inputs."""

    def remove(self, object: layout_object_id) -> None:
        """
        Removes an object and disconnects all inputs and sinks. Stale
        identities reject.
        """

    def move_object(
        self, object: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Moves an object without changing its identity or connections."""

    def pi_at(self, index: int) -> layout_object_id:
        """Returns a primary input in declared interface order."""

    def po_at(self, index: int) -> layout_object_id:
        """Returns a primary output in declared interface order."""

    def set_input_order(self, order: Sequence[layout_object_id]) -> None:
        """
        Sets the complete input permutation. Invalid orders reject without
        mutation.
        """

    def set_output_order(self, order: Sequence[layout_object_id]) -> None:
        """
        Sets the complete output permutation. Invalid orders reject without
        mutation.
        """

    def set_name(self, object: layout_object_id, name: str) -> None:
        """Sets an object's name."""

    def inputs(self, object: layout_object_id) -> list[layout_object_id | None]:
        """
        Returns input sources in port-index order, including None for disconnected ports.
        """

    def sinks(self, object: layout_object_id) -> list[layout_input_port]:
        """
        Returns the destination input ports driven by this object's output. Each sink identifies the destination object and its input index. If the source drives several inputs of one destination, each input is a separate sink. Connections need not be physically legal; order is unspecified.
        """

    def clear_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """Removes the occupant of a coordinate if present."""

    def is_gate_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """Returns whether the coordinate hosts a gate."""

    def is_wire_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """Returns whether the coordinate hosts a wire."""

    def is_empty_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """Returns whether a coordinate has no occupant."""

    def pis(self) -> list[layout_object_id]: ...
    def pos(self) -> list[layout_object_id]: ...
    def gates(self) -> list[layout_object_id]: ...
    def wires(self) -> list[layout_object_id]: ...
    def incoming_data_flow(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]: ...
    def outgoing_data_flow(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]:
        """
        Returns adjacent, outgoing-clocked destination coordinates once per connected input port. Multiple connected inputs on one destination yield repeated coordinates; order is unspecified.
        """

    def is_incoming_signal(
        self,
        t: coordinate | tuple[int, int] | tuple[int, int, int],
        s: coordinate | tuple[int, int] | tuple[int, int, int] | None,
    ) -> bool:
        """Checks for a physical incoming connection from the given x/y location."""

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

    def is_outgoing_signal(
        self,
        t: coordinate | tuple[int, int] | tuple[int, int, int],
        s: coordinate | tuple[int, int] | tuple[int, int, int] | None,
    ) -> bool:
        """Checks for a physical outgoing connection to the given x/y location."""

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

    def bounding_box(self) -> tuple[coordinate | None, coordinate | None]:
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
    Placed FCN objects, ordered ports, clocking, and obstructions.

    Objects have stable identities independent of their coordinates.
    Connections describe declared topology; physical validation checks
    adjacency, clocking, and geometry separately. Copies own independent
    state. A sink is a destination input port driven by a source object's
    output. One destination object may receive the same source at several
    input indices; each connected input port is a distinct sink. Visitors
    may edit coordinates, names, and capabilities. Object and terminal
    visitors must not create or remove objects, change terminal order, or
    replace the layout during traversal. Connection visitors must also
    preserve the traversed input or sink connections, as specified on each
    visitor.

    Template Args:
        CoordinateLayout: Coordinate geometry used for placement.
    """

    @overload
    def __init__(self, arrangement: arrangement) -> None: ...
    @overload
    def __init__(self, arrangement: arrangement, extent: extent | tuple[int, int] | tuple[int, int, int]) -> None:
        """Creates an empty layout with shifted rows or columns."""

    @overload
    def __init__(
        self,
        arrangement: arrangement,
        extent: extent | tuple[int, int] | tuple[int, int, int],
        clocking_scheme: str = "2DDWave",
        layout_name: str = "",
    ) -> None:
        """
        Creates an empty layout with shifted rows or columns and clocking.

        Raises:
            ValueError: The clocking scheme name is unknown.
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

    def obstructed_coordinates(self) -> list[coordinate]:
        """
        Returns manual coordinate obstructions in unspecified order, without implicit occupancy.
        """

    def obstructed_connections(self) -> list[tuple[coordinate, coordinate]]:
        """
        Returns manual directed-connection obstructions in unspecified order, without physical connections.
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

    def create_pi(self, name: str, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> layout_object_id:
        """
        Creates a primary input at `t`. Occupied coordinates reject without
        mutation.
        """

    @overload
    def create_po(
        self, s: layout_object_id, name: str, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a primary output driven by `s` at `t`."""

    @overload
    def create_po(self, name: str, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> layout_object_id:
        """Creates a primary output with its input disconnected."""

    def is_pi(self, n: layout_object_id) -> bool:
        """Returns whether an object is a primary input."""

    def is_po(self, n: layout_object_id) -> bool:
        """Returns whether an object is a primary output."""

    def is_pi_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """Returns whether the coordinate hosts a pi."""

    def is_po_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """Returns whether the coordinate hosts a po."""

    def is_inv(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes INV."""

    def is_and(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes AND."""

    def is_nand(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes NAND."""

    def is_or(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes OR."""

    def is_nor(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes NOR."""

    def is_xor(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes XOR."""

    def is_xnor(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes XNOR."""

    def is_lt(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes LT."""

    def is_le(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes LE."""

    def is_gt(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes GT."""

    def is_ge(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes GE."""

    def is_maj(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes MAJ."""

    def is_fanout(self, arg: layout_object_id, /) -> bool:
        """Returns whether an identity object drives more than one input port."""

    def is_buf(self, arg: layout_object_id, /) -> bool:
        """Returns whether an object computes the identity function."""

    def is_wire(self, arg: layout_object_id, /) -> bool:
        """Returns whether an object is a physical wire rather than a terminal."""

    def set_layout_name(self, name: str) -> None:
        """Sets the layout name."""

    def get_layout_name(self) -> str:
        """Returns the layout name."""

    def clone(self) -> shifted_cartesian_gate_layout:
        """Returns an independent value copy."""

    def __copy__(self) -> shifted_cartesian_gate_layout:
        """
        Returns an independent layout copy, including placed objects and metadata.
        """

    def __deepcopy__(self, memo: dict) -> shifted_cartesian_gate_layout:
        """
        Returns an independent layout copy, including placed objects and metadata.
        """

    def set_input_name(self, index: int, name: str) -> None:
        """Sets the input name at an interface index."""

    def get_input_name(self, index: int) -> str:
        """Returns the input name at an interface index."""

    def set_output_name(self, index: int, name: str) -> None:
        """Sets the output name at an interface index."""

    def get_output_name(self, index: int) -> str:
        """Returns the output name at an interface index."""

    def get_name(self, object: layout_object_id) -> str:
        """Returns an object's name, or an empty string for an unnamed object."""

    @overload
    def create_buf(
        self, a: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a wire driven by `a`."""

    @overload
    def create_buf(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> layout_object_id:
        """Creates a wire with its input disconnected."""

    def create_not(
        self, a: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a NOT gate."""

    def create_and(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a AND gate."""

    def create_nand(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a NAND gate."""

    def create_or(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a OR gate."""

    def create_nor(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a NOR gate."""

    def create_xor(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a XOR gate."""

    def create_xnor(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a XNOR gate."""

    def create_lt(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a LT gate."""

    def create_le(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a LE gate."""

    def create_gt(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a GT gate."""

    def create_ge(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a GE gate."""

    def create_maj(
        self,
        a: layout_object_id,
        b: layout_object_id,
        c: layout_object_id,
        t: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> layout_object_id:
        """Creates a majority gate."""

    def num_pis(self) -> int:
        """Counts primary inputs."""

    def num_pos(self) -> int:
        """Counts primary outputs."""

    def num_gates(self) -> int:
        """Counts logic gates, excluding wires and terminals."""

    def num_wires(self) -> int:
        """Counts physical wires, excluding primary inputs and outputs."""

    def num_crossings(self) -> int:
        """Counts crossing-layer wires above occupied ground-layer tiles."""

    def is_empty(self) -> bool:
        """Returns whether the layout has no objects."""

    def create_gate(
        self,
        inputs: Sequence[layout_object_id],
        function: mnt.pyfiction.synthesis.dynamic_truth_table,
        t: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> layout_object_id:
        """
        Creates a placed gate. Input indices follow truth-table variable order; trailing inputs may be disconnected.
        """

    def object_function(self, object: layout_object_id) -> mnt.pyfiction.synthesis.dynamic_truth_table:
        """Returns the object's truth table."""

    def size(self) -> int:
        """Counts live objects."""

    def fanin_size(self, object: layout_object_id) -> int:
        """Counts connected input slots, irrespective of physical legality."""

    def fanout_size(self, object: layout_object_id) -> int:
        """
        Counts destination input ports driven by this object's output.

        Each connected input port is a sink, including multiple ports on the
        same destination object.
        """

    def input_count(self, object: layout_object_id) -> int:
        """Returns the number of input slots, including disconnected slots."""

    def find_object(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> layout_object_id | None:
        """Finds the object at a coordinate; empty coordinates have no identity."""

    def contains(self, object: layout_object_id) -> bool:
        """Returns whether this identity names a live object."""

    def get_tile(self, object: layout_object_id) -> coordinate:
        """
        Returns the object's coordinate. @throws std::invalid_argument If the
        identity is stale.
        """

    def source(self, input: layout_input_port) -> layout_object_id | None:
        """Returns the declared source of an input, or no source if disconnected."""

    def connect(self, source: layout_object_id, input: layout_input_port) -> None:
        """
        Connects an output to an ordered input, replacing the input's existing
        source.

        Port and identity checks precede mutation. Adjacency, clocking, and
        geometry need not be valid during editing.
        """

    def disconnect(self, input: layout_input_port) -> None:
        """Disconnects one input without changing the indices of other inputs."""

    def remove(self, object: layout_object_id) -> None:
        """
        Removes an object and disconnects all inputs and sinks. Stale
        identities reject.
        """

    def move_object(
        self, object: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Moves an object without changing its identity or connections."""

    def pi_at(self, index: int) -> layout_object_id:
        """Returns a primary input in declared interface order."""

    def po_at(self, index: int) -> layout_object_id:
        """Returns a primary output in declared interface order."""

    def set_input_order(self, order: Sequence[layout_object_id]) -> None:
        """
        Sets the complete input permutation. Invalid orders reject without
        mutation.
        """

    def set_output_order(self, order: Sequence[layout_object_id]) -> None:
        """
        Sets the complete output permutation. Invalid orders reject without
        mutation.
        """

    def set_name(self, object: layout_object_id, name: str) -> None:
        """Sets an object's name."""

    def inputs(self, object: layout_object_id) -> list[layout_object_id | None]:
        """
        Returns input sources in port-index order, including None for disconnected ports.
        """

    def sinks(self, object: layout_object_id) -> list[layout_input_port]:
        """
        Returns the destination input ports driven by this object's output. Each sink identifies the destination object and its input index. If the source drives several inputs of one destination, each input is a separate sink. Connections need not be physically legal; order is unspecified.
        """

    def clear_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """Removes the occupant of a coordinate if present."""

    def is_gate_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """Returns whether the coordinate hosts a gate."""

    def is_wire_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """Returns whether the coordinate hosts a wire."""

    def is_empty_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """Returns whether a coordinate has no occupant."""

    def pis(self) -> list[layout_object_id]: ...
    def pos(self) -> list[layout_object_id]: ...
    def gates(self) -> list[layout_object_id]: ...
    def wires(self) -> list[layout_object_id]: ...
    def incoming_data_flow(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]: ...
    def outgoing_data_flow(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]:
        """
        Returns adjacent, outgoing-clocked destination coordinates once per connected input port. Multiple connected inputs on one destination yield repeated coordinates; order is unspecified.
        """

    def is_incoming_signal(
        self,
        t: coordinate | tuple[int, int] | tuple[int, int, int],
        s: coordinate | tuple[int, int] | tuple[int, int, int] | None,
    ) -> bool:
        """Checks for a physical incoming connection from the given x/y location."""

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

    def is_outgoing_signal(
        self,
        t: coordinate | tuple[int, int] | tuple[int, int, int],
        s: coordinate | tuple[int, int] | tuple[int, int, int] | None,
    ) -> bool:
        """Checks for a physical outgoing connection to the given x/y location."""

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

    def bounding_box(self) -> tuple[coordinate | None, coordinate | None]:
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
    Placed FCN objects, ordered ports, clocking, and obstructions.

    Objects have stable identities independent of their coordinates.
    Connections describe declared topology; physical validation checks
    adjacency, clocking, and geometry separately. Copies own independent
    state. A sink is a destination input port driven by a source object's
    output. One destination object may receive the same source at several
    input indices; each connected input port is a distinct sink. Visitors
    may edit coordinates, names, and capabilities. Object and terminal
    visitors must not create or remove objects, change terminal order, or
    replace the layout during traversal. Connection visitors must also
    preserve the traversed input or sink connections, as specified on each
    visitor.

    Template Args:
        CoordinateLayout: Coordinate geometry used for placement.
    """

    @overload
    def __init__(self, arrangement: arrangement) -> None: ...
    @overload
    def __init__(self, arrangement: arrangement, extent: extent | tuple[int, int] | tuple[int, int, int]) -> None:
        """Creates an empty layout with shifted rows or columns."""

    @overload
    def __init__(
        self,
        arrangement: arrangement,
        extent: extent | tuple[int, int] | tuple[int, int, int],
        clocking_scheme: str = "2DDWave",
        layout_name: str = "",
    ) -> None:
        """
        Creates an empty layout with shifted rows or columns and clocking.

        Raises:
            ValueError: The clocking scheme name is unknown.
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

    def obstructed_coordinates(self) -> list[coordinate]:
        """
        Returns manual coordinate obstructions in unspecified order, without implicit occupancy.
        """

    def obstructed_connections(self) -> list[tuple[coordinate, coordinate]]:
        """
        Returns manual directed-connection obstructions in unspecified order, without physical connections.
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

    def create_pi(self, name: str, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> layout_object_id:
        """
        Creates a primary input at `t`. Occupied coordinates reject without
        mutation.
        """

    @overload
    def create_po(
        self, s: layout_object_id, name: str, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a primary output driven by `s` at `t`."""

    @overload
    def create_po(self, name: str, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> layout_object_id:
        """Creates a primary output with its input disconnected."""

    def is_pi(self, n: layout_object_id) -> bool:
        """Returns whether an object is a primary input."""

    def is_po(self, n: layout_object_id) -> bool:
        """Returns whether an object is a primary output."""

    def is_pi_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """Returns whether the coordinate hosts a pi."""

    def is_po_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """Returns whether the coordinate hosts a po."""

    def is_inv(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes INV."""

    def is_and(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes AND."""

    def is_nand(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes NAND."""

    def is_or(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes OR."""

    def is_nor(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes NOR."""

    def is_xor(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes XOR."""

    def is_xnor(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes XNOR."""

    def is_lt(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes LT."""

    def is_le(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes LE."""

    def is_gt(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes GT."""

    def is_ge(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes GE."""

    def is_maj(self, arg: layout_object_id, /) -> bool:
        """Returns whether the object computes MAJ."""

    def is_fanout(self, arg: layout_object_id, /) -> bool:
        """Returns whether an identity object drives more than one input port."""

    def is_buf(self, arg: layout_object_id, /) -> bool:
        """Returns whether an object computes the identity function."""

    def is_wire(self, arg: layout_object_id, /) -> bool:
        """Returns whether an object is a physical wire rather than a terminal."""

    def set_layout_name(self, name: str) -> None:
        """Sets the layout name."""

    def get_layout_name(self) -> str:
        """Returns the layout name."""

    def clone(self) -> hexagonal_gate_layout:
        """Returns an independent value copy."""

    def __copy__(self) -> hexagonal_gate_layout:
        """
        Returns an independent layout copy, including placed objects and metadata.
        """

    def __deepcopy__(self, memo: dict) -> hexagonal_gate_layout:
        """
        Returns an independent layout copy, including placed objects and metadata.
        """

    def set_input_name(self, index: int, name: str) -> None:
        """Sets the input name at an interface index."""

    def get_input_name(self, index: int) -> str:
        """Returns the input name at an interface index."""

    def set_output_name(self, index: int, name: str) -> None:
        """Sets the output name at an interface index."""

    def get_output_name(self, index: int) -> str:
        """Returns the output name at an interface index."""

    def get_name(self, object: layout_object_id) -> str:
        """Returns an object's name, or an empty string for an unnamed object."""

    @overload
    def create_buf(
        self, a: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a wire driven by `a`."""

    @overload
    def create_buf(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> layout_object_id:
        """Creates a wire with its input disconnected."""

    def create_not(
        self, a: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a NOT gate."""

    def create_and(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a AND gate."""

    def create_nand(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a NAND gate."""

    def create_or(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a OR gate."""

    def create_nor(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a NOR gate."""

    def create_xor(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a XOR gate."""

    def create_xnor(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a XNOR gate."""

    def create_lt(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a LT gate."""

    def create_le(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a LE gate."""

    def create_gt(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a GT gate."""

    def create_ge(
        self, a: layout_object_id, b: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Creates a GE gate."""

    def create_maj(
        self,
        a: layout_object_id,
        b: layout_object_id,
        c: layout_object_id,
        t: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> layout_object_id:
        """Creates a majority gate."""

    def num_pis(self) -> int:
        """Counts primary inputs."""

    def num_pos(self) -> int:
        """Counts primary outputs."""

    def num_gates(self) -> int:
        """Counts logic gates, excluding wires and terminals."""

    def num_wires(self) -> int:
        """Counts physical wires, excluding primary inputs and outputs."""

    def num_crossings(self) -> int:
        """Counts crossing-layer wires above occupied ground-layer tiles."""

    def is_empty(self) -> bool:
        """Returns whether the layout has no objects."""

    def create_gate(
        self,
        inputs: Sequence[layout_object_id],
        function: mnt.pyfiction.synthesis.dynamic_truth_table,
        t: coordinate | tuple[int, int] | tuple[int, int, int],
    ) -> layout_object_id:
        """
        Creates a placed gate. Input indices follow truth-table variable order; trailing inputs may be disconnected.
        """

    def object_function(self, object: layout_object_id) -> mnt.pyfiction.synthesis.dynamic_truth_table:
        """Returns the object's truth table."""

    def size(self) -> int:
        """Counts live objects."""

    def fanin_size(self, object: layout_object_id) -> int:
        """Counts connected input slots, irrespective of physical legality."""

    def fanout_size(self, object: layout_object_id) -> int:
        """
        Counts destination input ports driven by this object's output.

        Each connected input port is a sink, including multiple ports on the
        same destination object.
        """

    def input_count(self, object: layout_object_id) -> int:
        """Returns the number of input slots, including disconnected slots."""

    def find_object(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> layout_object_id | None:
        """Finds the object at a coordinate; empty coordinates have no identity."""

    def contains(self, object: layout_object_id) -> bool:
        """Returns whether this identity names a live object."""

    def get_tile(self, object: layout_object_id) -> coordinate:
        """
        Returns the object's coordinate. @throws std::invalid_argument If the
        identity is stale.
        """

    def source(self, input: layout_input_port) -> layout_object_id | None:
        """Returns the declared source of an input, or no source if disconnected."""

    def connect(self, source: layout_object_id, input: layout_input_port) -> None:
        """
        Connects an output to an ordered input, replacing the input's existing
        source.

        Port and identity checks precede mutation. Adjacency, clocking, and
        geometry need not be valid during editing.
        """

    def disconnect(self, input: layout_input_port) -> None:
        """Disconnects one input without changing the indices of other inputs."""

    def remove(self, object: layout_object_id) -> None:
        """
        Removes an object and disconnects all inputs and sinks. Stale
        identities reject.
        """

    def move_object(
        self, object: layout_object_id, t: coordinate | tuple[int, int] | tuple[int, int, int]
    ) -> layout_object_id:
        """Moves an object without changing its identity or connections."""

    def pi_at(self, index: int) -> layout_object_id:
        """Returns a primary input in declared interface order."""

    def po_at(self, index: int) -> layout_object_id:
        """Returns a primary output in declared interface order."""

    def set_input_order(self, order: Sequence[layout_object_id]) -> None:
        """
        Sets the complete input permutation. Invalid orders reject without
        mutation.
        """

    def set_output_order(self, order: Sequence[layout_object_id]) -> None:
        """
        Sets the complete output permutation. Invalid orders reject without
        mutation.
        """

    def set_name(self, object: layout_object_id, name: str) -> None:
        """Sets an object's name."""

    def inputs(self, object: layout_object_id) -> list[layout_object_id | None]:
        """
        Returns input sources in port-index order, including None for disconnected ports.
        """

    def sinks(self, object: layout_object_id) -> list[layout_input_port]:
        """
        Returns the destination input ports driven by this object's output. Each sink identifies the destination object and its input index. If the source drives several inputs of one destination, each input is a separate sink. Connections need not be physically legal; order is unspecified.
        """

    def clear_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """Removes the occupant of a coordinate if present."""

    def is_gate_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """Returns whether the coordinate hosts a gate."""

    def is_wire_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """Returns whether the coordinate hosts a wire."""

    def is_empty_tile(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """Returns whether a coordinate has no occupant."""

    def pis(self) -> list[layout_object_id]: ...
    def pos(self) -> list[layout_object_id]: ...
    def gates(self) -> list[layout_object_id]: ...
    def wires(self) -> list[layout_object_id]: ...
    def incoming_data_flow(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]: ...
    def outgoing_data_flow(self, t: coordinate | tuple[int, int] | tuple[int, int, int]) -> list[coordinate]:
        """
        Returns adjacent, outgoing-clocked destination coordinates once per connected input port. Multiple connected inputs on one destination yield repeated coordinates; order is unspecified.
        """

    def is_incoming_signal(
        self,
        t: coordinate | tuple[int, int] | tuple[int, int, int],
        s: coordinate | tuple[int, int] | tuple[int, int, int] | None,
    ) -> bool:
        """Checks for a physical incoming connection from the given x/y location."""

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

    def is_outgoing_signal(
        self,
        t: coordinate | tuple[int, int] | tuple[int, int, int],
        s: coordinate | tuple[int, int] | tuple[int, int, int] | None,
    ) -> bool:
        """Checks for a physical outgoing connection to the given x/y location."""

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

    def bounding_box(self) -> tuple[coordinate | None, coordinate | None]:
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
    Layout-independent coordinate and directed-connection constraints for
    routing searches.

    Gate layouts expose persistent obstruction methods directly. Routing
    algorithms accept this separate object for temporary constraints and
    combine it with the layout's persistent obstructions and occupancy.
    Copies are independent. This object contains no layout or occupancy
    information.
    """

    def __init__(self) -> None:
        """Creates empty layout-independent routing-search constraints."""

    def obstruct_coordinate(self, c: coordinate | tuple[int, int] | tuple[int, int, int]) -> None:
        """
        Marks the given coordinate as obstructed.

        Args:
            c: Coordinate to obstruct.

        Raises:
            std::bad_alloc: If allocation fails.
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

        Raises:
            std::bad_alloc: If allocation fails.

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

    def obstructed_coordinates(self) -> list[coordinate]:
        """Returns explicit coordinate obstructions in unspecified order."""

    def obstructed_connections(self) -> list[tuple[coordinate, coordinate]]:
        """
        Returns explicit directed-connection obstructions in unspecified order.
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
    modes move with their cells. Layer coordinates, the layout name, and
    the clocking stay unchanged. The extent preserves trailing padding and
    encloses translated x/y coordinates and occupied nonnegative layers.

    Args:
        lyt: The layout to normalize.

    Template Args:
        Lyt: Cell grid layout type, e.g., `qca::layout`,
             `mol_qca::layout`, or `inml::layout`.

    Returns:
        Normalized copy of `lyt`.

    Raises:
        std::invalid_argument: If the normalized extent exceeds the signed
                               coordinate domain.
    """

def random_coordinate(
    corner1: coordinate | tuple[int, int] | tuple[int, int, int],
    corner2: coordinate | tuple[int, int] | tuple[int, int, int],
) -> coordinate:
    """
    Generates a random coordinate with each axis inside the inclusive
    region spanned by two coordinates.

    Args:
        corner1: One corner of the region.
        corner2: Opposite corner of the region; axes may appear in either
                 order.

    Template Args:
        CoordinateType: Coordinate type to generate.

    Returns:
        Random coordinate between the corresponding corner axes.
    """
