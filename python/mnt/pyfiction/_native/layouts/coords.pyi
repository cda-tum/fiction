# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Immutable layout coordinates."""

from typing import overload

class OffsetCoordinate:
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
    def __init__(self, c: OffsetCoordinate) -> None: ...
    @overload
    def __init__(self, tuple_repr: tuple[int, int] | tuple[int, int, int]) -> None: ...
    @property
    def x(self) -> int:
        """31 bit for the x coordinate."""

    @property
    def y(self) -> int:
        """31 bit for the y coordinate."""

    @property
    def z(self) -> int:
        """1 bit for the z coordinate."""

    def __eq__(self, other: OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Compares against another coordinate for equality. Respects the dead
        indicator.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff both coordinates are identical.
        """

    def __ne__(self, other: OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Compares against another coordinate for inequality. Respects the dead
        indicator.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff both coordinates are not identical.
        """

    def __lt__(self, other: OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Determine whether this coordinate is "less than" another one. This is
        the case if z is smaller, or if z is equal but y is smaller, or if z
        and y are equal but x is smaller.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff this coordinate is "less than" the other coordinate.
        """

    def __gt__(self, other: OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Determine whether this coordinate is "greater than" another one. This
        is the case if the other one is "less than".

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff this coordinate is "greater than" the other coordinate.
        """

    def __le__(self, other: OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Determine whether this coordinate is "less than or equal to" another
        one. This is the case if this one is not "greater than" the other.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff this coordinate is "less than or equal to" the other
            coordinate.
        """

    def __ge__(self, other: OffsetCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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

class CubeCoordinate:
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
    def __init__(self, c: CubeCoordinate) -> None: ...
    @overload
    def __init__(self, tuple_repr: tuple[int, int] | tuple[int, int, int]) -> None: ...
    @property
    def x(self) -> int:
        """x coordinate."""

    @property
    def y(self) -> int:
        """y coordinate."""

    @property
    def z(self) -> int:
        """z coordinate."""

    def __eq__(self, other: CubeCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Compares against another coordinate for equality. Respects the dead
        indicator.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff both coordinates are identical.
        """

    def __ne__(self, other: CubeCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Compares against another coordinate for inequality. Respects the dead
        indicator.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff both coordinates are not identical.
        """

    def __lt__(self, other: CubeCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Determine whether this coordinate is "less than" another one. This is
        the case if z is smaller, or if z is equal but y is smaller, or if z
        and y are equal but x is smaller.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff this coordinate is "less than" the other coordinate.
        """

    def __gt__(self, other: CubeCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Determine whether this coordinate is "greater than" another one. This
        is the case if the other one is "less than".

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff this coordinate is "greater than" the other coordinate.
        """

    def __le__(self, other: CubeCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
        """
        Determine whether this coordinate is "less than or equal to" another
        one. This is the case if this one is not "greater than" the other.

        Args:
            other: Right-hand side coordinate.

        Returns:
            `true` iff this coordinate is "less than or equal to" the other
            coordinate.
        """

    def __ge__(self, other: CubeCoordinate | tuple[int, int] | tuple[int, int, int]) -> bool:
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
