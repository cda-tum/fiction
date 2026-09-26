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

from mnt.pyfiction.layouts.coords import offset_coordinate
from mnt.pyfiction.qca import qca_cell_mode, qca_cell_type, qca_layout


def test_cell_types_and_modes() -> None:
    assert str(qca_cell_type.CONST_0) == "qca_cell_type.CONST_0"
    assert str(qca_cell_mode.CROSSOVER) == "qca_cell_mode.CROSSOVER"


def test_geometry_is_cartesian() -> None:
    layout = qca_layout(offset_coordinate(9, 9, 1))

    for t in layout.coordinates():
        assert t <= offset_coordinate(9, 9, 1)
        assert layout.is_within_bounds(t)

    for t in layout.ground_coordinates():
        assert t.z == 0

    for t in layout.adjacent_coordinates(offset_coordinate(2, 2)):
        assert t in [(1, 2), (2, 1), (3, 2), (2, 3)]


def test_cell_type_and_mode_assignment() -> None:
    layout = qca_layout(offset_coordinate(4, 4, 1), "OPEN", "crossing")

    assert layout.is_empty()
    assert layout.get_layout_name() == "crossing"

    layout.assign_cell_type(offset_coordinate(0, 2), qca_cell_type.INPUT)
    layout.assign_cell_type(offset_coordinate(2, 2), qca_cell_type.NORMAL)
    layout.assign_cell_type(offset_coordinate(2, 2, 1), qca_cell_type.NORMAL)
    layout.assign_cell_type(offset_coordinate(4, 2), qca_cell_type.OUTPUT)
    layout.assign_cell_mode(offset_coordinate(2, 2, 1), qca_cell_mode.CROSSOVER)
    layout.assign_cell_name(offset_coordinate(0, 2), "a")

    assert layout.num_cells() == 4
    assert layout.num_pis() == 1
    assert layout.num_pos() == 1
    assert layout.is_pi(offset_coordinate(0, 2))
    assert layout.pis() == [(0, 2)]
    assert layout.get_cell_name(offset_coordinate(0, 2)) == "a"
    assert layout.get_cell_type(offset_coordinate(2, 2, 1)) == qca_cell_type.NORMAL
    assert layout.get_cell_mode(offset_coordinate(2, 2, 1)) == qca_cell_mode.CROSSOVER
    assert layout.get_cell_mode(offset_coordinate(2, 2)) == qca_cell_mode.NORMAL
    assert layout.is_empty_cell(offset_coordinate(1, 1))

    layout.assign_cell_type(offset_coordinate(2, 2, 1), qca_cell_type.EMPTY)
    assert layout.is_empty_cell(offset_coordinate(2, 2, 1))
    assert layout.get_cell_mode(offset_coordinate(2, 2, 1)) == qca_cell_mode.NORMAL


def test_clock_zones_and_synchronization_elements() -> None:
    """Clock numbers and synchronization elements belong to clock zones, i.e., to tiles of cells."""
    layout = qca_layout(offset_coordinate(4, 4), "2DDWave", "", 2, 2)
    assert layout.get_tile_size_x() == 2
    assert layout.is_clocking_scheme("2DDWAVE")
    with pytest.raises(ValueError, match="positive"):
        layout.set_tile_size_x(0)
    with pytest.raises(ValueError, match="positive"):
        qca_layout(offset_coordinate(4, 4), "2DDWave", "", 0, 1)

    assert layout.get_clock_zone(offset_coordinate(3, 2)) == (1, 1)
    layout.assign_clock_number(offset_coordinate(1, 1), 3)
    layout.assign_synchronization_element(offset_coordinate(1, 1), 2)
    assert layout.get_clock_number(offset_coordinate(2, 2)) == 3
    assert layout.get_synchronization_element(offset_coordinate(3, 3)) == 2
    assert layout.get_synchronization_element(offset_coordinate(1, 1)) == 0
    assert layout.num_se() == 1

    duplicate = copy.copy(layout)
    assert duplicate == layout
    duplicate.assign_clock_number(offset_coordinate(1, 1), 0)
    duplicate.assign_synchronization_element(offset_coordinate(1, 1), 0)
    assert layout.get_clock_number(offset_coordinate(2, 2)) == 3
    assert layout.num_se() == 1

    layout.replace_clocking_scheme("USE")
    assert layout.is_clocking_scheme("USE")
    assert layout.get_clocking_scheme_name() == "USE"
    with pytest.raises(ValueError, match="Unknown clocking scheme"):
        layout.replace_clocking_scheme("3DDWave")
    with pytest.raises(ValueError, match="clocking scheme"):
        qca_layout(offset_coordinate(4, 4), "3DDWave")
