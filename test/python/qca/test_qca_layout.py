# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import copy

import pytest

from mnt.pyfiction.qca import QcaCellMode, QcaCellType, QCALayout


def test_cell_types_and_modes() -> None:
    assert str(QcaCellType.CONST_0) == "QcaCellType.CONST_0"
    assert str(QcaCellMode.CROSSOVER) == "QcaCellMode.CROSSOVER"


def test_geometry_is_cartesian() -> None:
    layout = QCALayout((9, 9, 1))

    for t in layout.coordinates():
        assert t <= (9, 9, 1)
        assert layout.is_within_bounds(t)

    for t in layout.ground_coordinates():
        assert t.z == 0

    for t in layout.adjacent_coordinates((2, 2)):
        assert t in [(1, 2), (2, 1), (3, 2), (2, 3)]


def test_cell_type_and_mode_assignment() -> None:
    layout = QCALayout((4, 4, 1), "OPEN", "crossing")

    assert layout.is_empty()
    assert layout.name == "crossing"

    layout.assign_cell_type((0, 2), QcaCellType.INPUT)
    layout.assign_cell_type((2, 2), QcaCellType.NORMAL)
    layout.assign_cell_type((2, 2, 1), QcaCellType.NORMAL)
    layout.assign_cell_type((4, 2), QcaCellType.OUTPUT)
    layout.assign_cell_mode((2, 2, 1), QcaCellMode.CROSSOVER)
    layout.assign_cell_name((0, 2), "a")

    assert layout.num_cells() == 4
    assert layout.num_pis() == 1
    assert layout.num_pos() == 1
    assert layout.is_pi((0, 2))
    assert layout.pis() == [(0, 2)]
    assert layout.get_cell_name((0, 2)) == "a"
    assert layout.get_cell_type((2, 2, 1)) == QcaCellType.NORMAL
    assert layout.get_cell_mode((2, 2, 1)) == QcaCellMode.CROSSOVER
    assert layout.get_cell_mode((2, 2)) == QcaCellMode.NORMAL
    assert layout.is_empty_cell((1, 1))

    layout.assign_cell_type((2, 2, 1), QcaCellType.EMPTY)
    assert layout.is_empty_cell((2, 2, 1))
    assert layout.get_cell_mode((2, 2, 1)) == QcaCellMode.NORMAL


def test_clock_zones_and_synchronization_elements() -> None:
    """Clock numbers and synchronization elements belong to clock zones, i.e., to tiles of cells."""
    layout = QCALayout((4, 4), "2DDWave", "", 2, 2)
    assert layout.get_tile_size_x() == 2
    assert layout.is_clocking_scheme("2DDWAVE")
    with pytest.raises(ValueError, match="positive"):
        layout.set_tile_size_x(0)
    with pytest.raises(ValueError, match="positive"):
        QCALayout((4, 4), "2DDWave", "", 0, 1)

    assert layout.get_clock_zone((3, 2)) == (1, 1)
    layout.assign_clock_number((1, 1), 3)
    layout.assign_synchronization_element((1, 1), 2)
    assert layout.get_clock_number((2, 2)) == 3
    assert layout.get_synchronization_element((3, 3)) == 2
    assert layout.get_synchronization_element((1, 1)) == 0
    assert layout.num_se() == 1

    duplicate = copy.copy(layout)
    assert duplicate == layout
    duplicate.assign_clock_number((1, 1), 0)
    duplicate.assign_synchronization_element((1, 1), 0)
    assert layout.get_clock_number((2, 2)) == 3
    assert layout.num_se() == 1

    layout.replace_clocking_scheme("USE")
    assert layout.is_clocking_scheme("USE")
    assert layout.get_clocking_scheme_name() == "USE"
    with pytest.raises(ValueError, match="Unknown clocking scheme"):
        layout.replace_clocking_scheme("3DDWave")
    with pytest.raises(ValueError, match="clocking scheme"):
        QCALayout((4, 4), "3DDWave")
