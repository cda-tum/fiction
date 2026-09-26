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
from mnt.pyfiction.physical_design import color_routing
from mnt.pyfiction.verification import gate_level_drvs


def test_drvs():
    # Create empty layout
    layout = cartesian_gate_layout(offset_coordinate(2, 5, 0), "2DDWave")

    # Create 2:1 MUX

    # Inputs
    layout.create_pi("x1", offset_coordinate(0, 3))
    layout.create_pi("x2", offset_coordinate(0, 0))
    layout.create_pi("x3", offset_coordinate(2, 0))

    # Wires
    layout.create_buf(layout.make_signal(layout.get_node(offset_coordinate(0, 0, 0))), offset_coordinate(0, 1))
    layout.create_buf(layout.make_signal(layout.get_node(offset_coordinate(0, 1, 0))), offset_coordinate(1, 1))

    # NOT
    layout.create_not(layout.make_signal(layout.get_node(offset_coordinate(0, 1, 0))), offset_coordinate(0, 2))

    # Wires
    layout.create_buf(layout.make_signal(layout.get_node(offset_coordinate(0, 2, 0))), offset_coordinate(1, 2))
    # AND
    layout.create_and(
        layout.make_signal(layout.get_node(offset_coordinate(0, 3, 0))),
        layout.make_signal(layout.get_node(offset_coordinate(1, 2, 0))),
        offset_coordinate(1, 3),
    )

    # AND
    layout.create_and(
        layout.make_signal(layout.get_node(offset_coordinate(1, 1, 0))),
        layout.make_signal(layout.get_node(offset_coordinate(2, 0, 0))),
        offset_coordinate(2, 1),
    )

    # Wires
    layout.create_buf(layout.make_signal(layout.get_node(offset_coordinate(2, 1, 0))), offset_coordinate(2, 2))

    # OR
    layout.create_or(
        layout.make_signal(layout.get_node(offset_coordinate(1, 3, 0))),
        layout.make_signal(layout.get_node(offset_coordinate(2, 2, 0))),
        offset_coordinate(2, 3),
    )

    # Outputs
    layout.create_po(layout.make_signal(layout.get_node(offset_coordinate(2, 3, 0))), "f1", offset_coordinate(2, 4))

    layout.move_node(layout.get_node(offset_coordinate(2, 4)), offset_coordinate(2, 5))

    color_routing(layout, [(offset_coordinate(2, 3), offset_coordinate(2, 5))])

    warnings, drvs = gate_level_drvs(layout)

    assert warnings == 0
    assert drvs == 0
