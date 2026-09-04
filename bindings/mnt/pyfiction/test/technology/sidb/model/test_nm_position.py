# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction import sidb_100_lattice, sidb_111_lattice, sidb_cell_level_layout, sidb_nm_position


def test_layout_without_lattice_orientation() -> None:
    """The Cartesian SiDB origin maps to the physical origin."""
    layout_one = sidb_cell_level_layout((10, 10))

    result = sidb_nm_position(layout_one, (0, 0))

    assert result == (0, 0)


def test_layout_with_100_lattice_orientation() -> None:
    """The H-Si(100)-2x1 origin maps to the physical origin."""
    layout_one = sidb_100_lattice((10, 10))

    result = sidb_nm_position(layout_one, (0, 0))

    assert result == (0, 0)


def test_layout_with_111_lattice_orientation() -> None:
    """The H-Si(111)-1x1 origin maps to the physical origin."""
    layout_one = sidb_111_lattice((10, 10))

    result = sidb_nm_position(layout_one, (0, 0))

    assert result == (0, 0)
