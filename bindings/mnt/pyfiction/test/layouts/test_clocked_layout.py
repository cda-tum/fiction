# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction import clocked_cartesian_layout, clocked_hexagonal_layout, clocked_shifted_cartesian_layout

CLOCKED_LAYOUTS = [
    pytest.param(lambda: clocked_cartesian_layout((2, 2, 0), "2DDWave"), id="clocked_cartesian_layout"),
    pytest.param(lambda: clocked_shifted_cartesian_layout((2, 2, 0), "2DDWave"), id="clocked_shifted_cartesian_layout"),
    pytest.param(lambda: clocked_hexagonal_layout((2, 2, 0), "2DDWave"), id="clocked_hexagonal_layout"),
]


@pytest.mark.parametrize("make_layout", CLOCKED_LAYOUTS)
def test_clocked_layout_inheritance(make_layout):
    layout = make_layout()
    for t in layout.coordinates():
        assert t <= (9, 9, 1)
        assert layout.is_within_bounds(t)

    for t in layout.ground_coordinates():
        assert t.z == 0
        assert t <= (9, 9, 0)
        assert layout.is_within_bounds(t)

    for t in layout.adjacent_coordinates((2, 2)):
        assert t in [(1, 1), (1, 2), (2, 1), (3, 2), (2, 3)]


@pytest.mark.parametrize("make_layout", CLOCKED_LAYOUTS)
def test_clock_zone_iteration(make_layout):
    layout = make_layout()
    assert layout.incoming_clocked_zones((0, 0)) == []
    assert layout.outgoing_clocked_zones((2, 2)) == []

    for icz in layout.incoming_clocked_zones((1, 1)):
        assert icz in [layout.coord(1, 0), layout.coord(0, 1)]

    for icz in layout.outgoing_clocked_zones((1, 1)):
        assert icz in [layout.coord(1, 2), layout.coord(2, 1)]


@pytest.mark.parametrize(
    "layout",
    [
        pytest.param(clocked_cartesian_layout, id="clocked_cartesian_layout"),
        pytest.param(clocked_shifted_cartesian_layout, id="clocked_shifted_cartesian_layout"),
        pytest.param(clocked_hexagonal_layout, id="clocked_hexagonal_layout"),
    ],
)
def test_fetch_clocking_scheme(layout):
    layout((1, 1), "USE")
    layout((2, 2), "2DDWave")
    layout((3, 3), "RES")
    layout((4, 4), "ESR")
    layout((5, 5), "BANCS")

    with pytest.raises(RuntimeError):
        layout((1, 2), "3DDWave")
    with pytest.raises(RuntimeError):
        layout((1, 2), "SUE")
    with pytest.raises(RuntimeError):
        layout((1, 2), "PES")
