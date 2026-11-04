# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction.layouts import cartesian_layout


def test_coordinate_iteration():
    layout = cartesian_layout((10, 8, 2))

    coordinates = layout.coordinates()
    assert sorted((t.x, t.y, t.z) for t in coordinates) == sorted(
        (x, y, z) for z in range(2) for y in range(8) for x in range(10)
    )
    assert all(layout.is_within_bounds(t) for t in coordinates)
    assert not layout.is_within_bounds((10, 0, 0))
    assert not layout.is_within_bounds((0, 8, 0))
    assert not layout.is_within_bounds((0, 0, 2))

    ground_coordinates = layout.ground_coordinates()
    assert sorted((t.x, t.y, t.z) for t in ground_coordinates) == sorted((x, y, 0) for y in range(8) for x in range(10))
    assert all(layout.is_within_bounds(t) for t in ground_coordinates)

    neighbors = layout.adjacent_coordinates((2, 2))
    assert sorted((t.x, t.y, t.z) for t in neighbors) == [(1, 2, 0), (2, 1, 0), (2, 3, 0), (3, 2, 0)]

    assert sorted((t.x, t.y, t.z) for t in layout.adjacent_coordinates((0, 0))) == [(0, 1, 0), (1, 0, 0)]
