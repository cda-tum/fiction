# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction.layouts import cartesian_gate_layout
from mnt.pyfiction.physical_design import color_routing
from mnt.pyfiction.verification import gate_level_drv_params, gate_level_drvs


def test_drvs() -> None:
    layout = cartesian_gate_layout((3, 6, 1), "2DDWave")
    x1 = layout.create_pi("x1", (0, 3))
    x2 = layout.create_pi("x2", (0, 0))
    x3 = layout.create_pi("x3", (2, 0))
    first_wire = layout.create_buf(x2, (0, 1))
    second_wire = layout.create_buf(first_wire, (1, 1))
    inv = layout.create_not(first_wire, (0, 2))
    third_wire = layout.create_buf(inv, (1, 2))
    first_and = layout.create_and(x1, third_wire, (1, 3))
    second_and = layout.create_and(second_wire, x3, (2, 1))
    fourth_wire = layout.create_buf(second_and, (2, 2))
    result = layout.create_or(first_and, fourth_wire, (2, 3))
    output = layout.create_po(result, "f1", (2, 4))
    layout.move_node(output.object, (2, 5))
    color_routing(layout, [((2, 3), (2, 5))])

    warnings, drvs = gate_level_drvs(layout)

    assert warnings == 0
    assert drvs == 0


def test_drvs_check_placements_outside_extent() -> None:
    layout = cartesian_gate_layout((1, 1, 1), "2DDWave")
    source = layout.create_pi("a", (-2, 0))
    layout.create_po(source, "result", (-1, 0))
    params = gate_level_drv_params()
    params.non_adjacent_connections = False
    params.missing_connections = False
    params.crossing_gates = False
    params.clocked_data_flow = False
    params.has_io = False
    params.border_io = False
    assert gate_level_drvs(layout, params) == (0, 2)
    params.outside_extent = False
    assert gate_level_drvs(layout, params) == (0, 0)
