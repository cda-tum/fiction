# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction.layouts import cartesian_gate_layout
from mnt.pyfiction.layouts.coords import offset_coordinate
from mnt.pyfiction.physical_design import route_path
from mnt.pyfiction.physical_design.path_finding import a_star


def test_empty_layout():
    lyt = cartesian_gate_layout(offset_coordinate(4, 2), "2DDWave")

    x1 = lyt.create_pi("x1", offset_coordinate(0, 1))
    lyt.create_po(x1, "f1", offset_coordinate(4, 1))

    route_path(
        lyt,
        [
            offset_coordinate(0, 1),
            offset_coordinate(1, 1),
            offset_coordinate(2, 1),
            offset_coordinate(3, 1),
            offset_coordinate(4, 1),
        ],
    )

    for x, y in [(0, 1), (1, 1), (2, 1), (3, 1), (4, 1)]:
        assert lyt.is_wire_tile(offset_coordinate(x, y))


def test_empty_layout_a_star():
    lyt = cartesian_gate_layout(offset_coordinate(4, 2), "2DDWave")

    x1 = lyt.create_pi("x1", offset_coordinate(0, 1))
    lyt.create_po(x1, "f1", offset_coordinate(4, 1))

    route_path(lyt, a_star(lyt, offset_coordinate(0, 1), offset_coordinate(4, 1)))

    for x, y in [(0, 1), (1, 1), (2, 1), (3, 1), (4, 1)]:
        assert lyt.is_wire_tile(offset_coordinate(x, y))
