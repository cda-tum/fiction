# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction.inml import INMLLayout, InmlMagnetType


def test_magnet_types() -> None:
    assert str(InmlMagnetType.INVERTER_MAGNET) == "InmlMagnetType.INVERTER_MAGNET"
    assert str(InmlMagnetType.FANOUT_COUPLER_MAGNET) == "InmlMagnetType.FANOUT_COUPLER_MAGNET"


def test_planar_clocked_layout() -> None:
    layout = INMLLayout((7, 7, 1), "2DDWave", "inverter", 4, 4)

    assert layout.z() == 0
    assert layout.get_tile_size_x() == 4

    layout.assign_cell_type((0, 1), InmlMagnetType.INPUT)
    layout.assign_cell_type((1, 1), InmlMagnetType.INVERTER_MAGNET)
    layout.assign_cell_type((5, 1), InmlMagnetType.OUTPUT)

    assert layout.num_cells() == 3
    assert layout.get_cell_type((1, 1)) == InmlMagnetType.INVERTER_MAGNET
    assert layout.get_clock_number((1, 1)) == 0
    assert layout.get_clock_number((5, 1)) == 1
