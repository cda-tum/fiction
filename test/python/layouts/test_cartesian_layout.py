# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction.layouts import cartesian_layout
from mnt.pyfiction.layouts.coords import offset_coordinate


def test_coordinate_iteration():
    layout = cartesian_layout(offset_coordinate(9, 9, 1))

    for t in layout.coordinates():
        assert t <= offset_coordinate(9, 9, 1)
        assert layout.is_within_bounds(t)

    for t in layout.ground_coordinates():
        assert t.z == 0
        assert t <= offset_coordinate(9, 9, 0)
        assert layout.is_within_bounds(t)

    for t in layout.adjacent_coordinates(offset_coordinate(2, 2)):
        assert t in [(1, 2), (2, 1), (3, 2), (2, 3)]
