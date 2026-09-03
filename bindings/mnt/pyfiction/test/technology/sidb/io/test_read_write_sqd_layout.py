# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction import read_sqd_layout_100, read_sqd_layout_111


def test_read_write(resources_dir):
    read_sqd_layout_100(str(resources_dir / "hex_11_inputsdbp_inv_straight_v0_manual.sqd"))
    read_sqd_layout_111(str(resources_dir / "AND_mu_032_111_surface.sqd"))
