# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction.layouts import arrangement, hexagonal_layout


def test_coordinate_iteration():
    layout = hexagonal_layout(arrangement.EVEN_ROW, (10, 10, 2))

    for t in layout.coordinates():
        assert t <= (9, 9, 1)
        assert layout.is_within_bounds(t)

    for t in layout.ground_coordinates():
        assert t.z == 0
        assert t <= (9, 9, 0)
        assert layout.is_within_bounds(t)

    # comparing the whole set rather than each element also catches a missing neighbor
    neighbors = {(t.x, t.y, t.z) for t in layout.adjacent_coordinates((2, 2))}
    assert neighbors == {(2, 1, 0), (3, 1, 0), (3, 2, 0), (3, 3, 0), (2, 3, 0), (1, 2, 0)}


@pytest.mark.parametrize("a", list(arrangement))
def test_arrangement_round_trips(a):
    assert hexagonal_layout(a, (3, 3)).get_arrangement() == a
    assert hexagonal_layout(a).get_arrangement() == a


def test_arrangement_is_required():
    with pytest.raises(TypeError):
        hexagonal_layout((3, 3))  # ty: ignore[invalid-argument-type]  # deliberately missing arrangement
