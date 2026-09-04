# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction import cartesian_layout, cartesian_layout_cube, cube_coordinate


def test_coordinate_iteration():
    layout = cartesian_layout((9, 9, 1))

    for t in layout.coordinates():
        assert t <= (9, 9, 1)
        assert layout.is_within_bounds(t)

    for t in layout.ground_coordinates():
        assert t.z == 0
        assert t <= (9, 9, 0)
        assert layout.is_within_bounds(t)

    for t in layout.adjacent_coordinates((2, 2)):
        assert t in [(1, 2), (2, 1), (3, 2), (2, 3)]


def test_cube_coordinate_bounds():
    layout = cartesian_layout_cube(((-2, -1, 0), (1, 2, 1)))

    assert (layout.x_min(), layout.y_min(), layout.z_min()) == (-2, -1, 0)
    assert (layout.x(), layout.y(), layout.z()) == (1, 2, 1)
    assert layout.area() == 16

    coordinates = layout.coordinates()
    assert coordinates[0] == cube_coordinate(-2, -1, 0)
    assert coordinates[-1] == cube_coordinate(1, 2, 1)
    assert len(coordinates) == 32
    assert layout.west((-2, 0)) == cube_coordinate(-2, 0)
    assert layout.north((0, -1)) == cube_coordinate(0, -1)


def test_cube_coordinate_bounds_validation():
    with pytest.raises(ValueError, match="maximum must not be below the origin"):
        cartesian_layout_cube(cube_coordinate(-1, -1))

    with pytest.raises(ValueError, match="minimum must not exceed its maximum"):
        cartesian_layout_cube(((1, 1), (0, 0)))

    negative_layout = cartesian_layout_cube(((-3, -2, -1), (-1, -1, -1)))
    assert (negative_layout.x_min(), negative_layout.y_min(), negative_layout.z_min()) == (-3, -2, -1)
    assert (negative_layout.x(), negative_layout.y(), negative_layout.z()) == (-1, -1, -1)
