# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction.layouts import arrangement, shifted_cartesian_layout


def test_coordinate_iteration():
    layout = shifted_cartesian_layout(arrangement.ODD_COLUMN, (10, 10, 2))

    for t in layout.coordinates():
        assert t <= (9, 9, 1)
        assert layout.is_within_bounds(t)

    for t in layout.ground_coordinates():
        assert t.z == 0
        assert t <= (9, 9, 0)
        assert layout.is_within_bounds(t)

    for t in layout.adjacent_coordinates((2, 2)):
        assert t in [(1, 1), (1, 2), (2, 1), (3, 1), (3, 2), (2, 3)]


@pytest.mark.parametrize("a", list(arrangement))
def test_arrangement_round_trips(a):
    assert shifted_cartesian_layout(a, (3, 3)).get_arrangement() == a
    assert shifted_cartesian_layout(a).get_arrangement() == a


def test_arrangement_is_required():
    with pytest.raises(TypeError):
        shifted_cartesian_layout((3, 3))  # ty: ignore[invalid-argument-type]  # deliberately missing arrangement
