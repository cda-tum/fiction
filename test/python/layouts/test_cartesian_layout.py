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
    layout = cartesian_layout((10, 10, 2))

    coordinates = layout.coordinates()
    assert sorted((t.x, t.y, t.z) for t in coordinates) == sorted(
        (x, y, z) for z in range(2) for y in range(10) for x in range(10)
    )
    assert all(layout.is_within_bounds(t) for t in coordinates)

    ground_coordinates = layout.ground_coordinates()
    assert sorted((t.x, t.y, t.z) for t in ground_coordinates) == sorted(
        (x, y, 0) for y in range(10) for x in range(10)
    )
    assert all(layout.is_within_bounds(t) for t in ground_coordinates)

    neighbors = layout.adjacent_coordinates((2, 2))
    assert sorted((t.x, t.y, t.z) for t in neighbors) == [(1, 2, 0), (2, 1, 0), (2, 3, 0), (3, 2, 0)]
