# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction.inml import inml_layout, inml_magnet_type
from mnt.pyfiction.layouts.coords import offset_coordinate


def test_magnet_types() -> None:
    assert str(inml_magnet_type.INVERTER_MAGNET) == "inml_magnet_type.INVERTER_MAGNET"
    assert str(inml_magnet_type.FANOUT_COUPLER_MAGNET) == "inml_magnet_type.FANOUT_COUPLER_MAGNET"


def test_planar_clocked_layout() -> None:
    layout = inml_layout(offset_coordinate(7, 7, 1), "2DDWave", "inverter", 4, 4)

    assert layout.z() == 0
    assert layout.get_tile_size_x() == 4

    layout.assign_cell_type(offset_coordinate(0, 1), inml_magnet_type.INPUT)
    layout.assign_cell_type(offset_coordinate(1, 1), inml_magnet_type.INVERTER_MAGNET)
    layout.assign_cell_type(offset_coordinate(5, 1), inml_magnet_type.OUTPUT)

    assert layout.num_cells() == 3
    assert layout.get_cell_type(offset_coordinate(1, 1)) == inml_magnet_type.INVERTER_MAGNET
    assert layout.get_clock_number(offset_coordinate(1, 1)) == 0
    assert layout.get_clock_number(offset_coordinate(5, 1)) == 1
