# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction.layouts import cartesian_gate_layout, hexagonal_gate_layout, shifted_cartesian_gate_layout

if TYPE_CHECKING:
    from collections.abc import Callable

CLOCKED_LAYOUTS = [
    pytest.param(lambda: cartesian_gate_layout((2, 2, 0), "2DDWave"), id="cartesian_gate_layout"),
    pytest.param(lambda: shifted_cartesian_gate_layout((2, 2, 0), "2DDWave"), id="shifted_cartesian_gate_layout"),
    pytest.param(lambda: hexagonal_gate_layout((2, 2, 0), "2DDWave"), id="hexagonal_gate_layout"),
]


@pytest.mark.parametrize("make_layout", CLOCKED_LAYOUTS)
def test_layout_coordinate_access(make_layout):
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
        pytest.param(cartesian_gate_layout, id="cartesian_gate_layout"),
        pytest.param(shifted_cartesian_gate_layout, id="shifted_cartesian_gate_layout"),
        pytest.param(hexagonal_gate_layout, id="hexagonal_gate_layout"),
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


def test_clocking_scheme_name() -> None:
    layout = cartesian_gate_layout((2, 2), "2DDWave")
    assert layout.get_clocking_scheme_name() == "2DDWAVE"
    assert layout.is_clocking_scheme("2DDWAVE")
    assert not layout.is_clocking_scheme("USE")


@pytest.mark.parametrize("make_layout", CLOCKED_LAYOUTS)
def test_clock_number_applies_to_every_layer(make_layout: Callable[[], Any]) -> None:
    layout = make_layout()
    layout.assign_clock_number((1, 1, 0), 3)
    assert layout.get_clock_number((1, 1, 1)) == 3

    layout.assign_clock_number((0, 1, 1), 2)
    assert layout.get_clock_number((0, 1, 0)) == 2


@pytest.mark.parametrize("make_layout", CLOCKED_LAYOUTS)
def test_replace_clocking_scheme_keeps_synchronization_elements(make_layout: Callable[[], Any]) -> None:
    layout = make_layout()
    layout.assign_clock_number((1, 1), 3)
    layout.assign_synchronization_element((1, 1), 2)
    assert layout.is_synchronization_element((1, 1))

    layout.replace_clocking_scheme("RES")
    assert layout.is_clocking_scheme("RES")
    assert layout.is_regularly_clocked()
    assert layout.get_synchronization_element((1, 1)) == 2
    assert layout.num_se() == 1

    with pytest.raises(ValueError, match="Unknown clocking scheme"):
        layout.replace_clocking_scheme("3DDWave")
    assert layout.is_clocking_scheme("RES")
