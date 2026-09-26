# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Layout coordinates and their area and volume functions."""

from typing import overload

class offset_coordinate:
    """
    Unsigned offset coordinates.

    This implementation is optimized for memory-efficiency and fits within
    64 bits. Coordinates span from :math:`(0, 0, 0)` to :math:`(2^{31} -
    1, 2^{31} - 1, 1)`. Each coordinate has a dead indicator `d` that can
    be used to represent that it is not in use.
    """

    @overload
    def __init__(self) -> None:
        """Default constructor. Creates a dead coordinate at (0, 0, 0)."""

    @overload
    def __init__(self, int_repr: int) -> None:
        """
        Standard constructor. Instantiates a coordinate from an uint64_t,
        where the positions are encoded in the following four parts of the
        unsigned 64-bit integer (from MSB to LSB):
         - 1 bit for the dead indicator - 1 bit for the z position - 31 bit
           for the y position - 31 bit for the x position

        Args:
            t: Unsigned 64-bit integer to instantiate the coordinate from.
        """

    @overload
    def __init__(self, x: int, y: int, z: int = 0) -> None:
        """
        Standard constructor. Creates a non-dead coordinate at (x_, y_, z_).

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
    def __init__(self, c: offset_coordinate) -> None: ...
    @overload
    def __init__(self, tuple_repr: tuple) -> None: ...
    @property
    def x(self) -> int:
        """31 bit for the x coordinate."""

    @x.setter
    def x(self, arg: int, /) -> None: ...
    @property
    def y(self) -> int:
        """31 bit for the y coordinate."""

    @y.setter
    def y(self, arg: int, /) -> None: ...
    @property
    def z(self) -> int:
        """1 bit for the z coordinate."""

    @z.setter
    def z(self, arg: int, /) -> None: ...
    def __eq__(self, other: offset_coordinate) -> bool:
        """
        Compares against another coordinate for equality. Respects the dead
        indicator.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff both coordinates are identical.
        """

    def __ne__(self, other: offset_coordinate) -> bool:
        """
        Compares against another coordinate for inequality. Respects the dead
        indicator.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff both coordinates are not identical.
        """

    def __lt__(self, other: offset_coordinate) -> bool:
        """
        Determine whether this coordinate is "less than" another one. This is
        the case if z is smaller, or if z is equal but y is smaller, or if z
        and y are equal but x is smaller.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff this coordinate is "less than" the other coordinate.
        """

    def __gt__(self, other: offset_coordinate) -> bool:
        """
        Determine whether this coordinate is "greater than" another one. This
        is the case if the other one is "less than".

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff this coordinate is "greater than" the other coordinate.
        """

    def __le__(self, other: offset_coordinate) -> bool:
        """
        Determine whether this coordinate is "less than or equal to" another
        one. This is the case if this one is not "greater than" the other.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff this coordinate is "less than or equal to" the other
            coordinate.
        """

    def __ge__(self, other: offset_coordinate) -> bool:
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

class cube_coordinate:
    """
    Signed cube coordinates.

    This implementation allows for negative coordinate values and offers a
    balance between memory consumption and range of values. Coordinates
    span from :math:`(-2^{31}, -2^{31}, -2^{31})` to :math:`(2^{31} - 1,
    2^{31} - 1, 2^{31} - 1)`. Each coordinate has a dead indicator `d`
    that can be used to represent that it is not in use.
    """

    @overload
    def __init__(self) -> None:
        """Default constructor. Creates a dead coordinate at (0, 0, 0)."""

    @overload
    def __init__(self, x: int, y: int, z: int = 0) -> None:
        """
        Standard constructor. Creates a non-dead coordinate at (x_, y_, z_).

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
    def __init__(self, c: cube_coordinate) -> None: ...
    @overload
    def __init__(self, tuple_repr: tuple) -> None: ...
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
    def __eq__(self, other: cube_coordinate) -> bool:
        """
        Compares against another coordinate for equality. Respects the dead
        indicator.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff both coordinates are identical.
        """

    def __ne__(self, other: cube_coordinate) -> bool:
        """
        Compares against another coordinate for inequality. Respects the dead
        indicator.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff both coordinates are not identical.
        """

    def __lt__(self, other: cube_coordinate) -> bool:
        """
        Determine whether this coordinate is "less than" another one. This is
        the case if z is smaller, or if z is equal but y is smaller, or if z
        and y are equal but x is smaller.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff this coordinate is "less than" the other coordinate.
        """

    def __gt__(self, other: cube_coordinate) -> bool:
        """
        Determine whether this coordinate is "greater than" another one. This
        is the case if the other one is "less than".

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff this coordinate is "greater than" the other coordinate.
        """

    def __le__(self, other: cube_coordinate) -> bool:
        """
        Determine whether this coordinate is "less than or equal to" another
        one. This is the case if this one is not "greater than" the other.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff this coordinate is "less than or equal to" the other
            coordinate.
        """

    def __ge__(self, other: cube_coordinate) -> bool:
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

def offset_area(coord: offset_coordinate) -> int:
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

def cube_area(coord: cube_coordinate) -> int:
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

def offset_volume(coord: offset_coordinate) -> int:
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

def cube_volume(coord: cube_coordinate) -> int:
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
