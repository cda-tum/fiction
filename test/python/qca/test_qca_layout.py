# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import copy
from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.qca import qca_cell_mode, qca_cell_type, qca_layout

if TYPE_CHECKING:
    from collections.abc import Callable


def test_cell_types_and_modes() -> None:
    assert str(qca_cell_type.CONST_0) == "qca_cell_type.CONST_0"
    assert str(qca_cell_mode.CROSSOVER) == "qca_cell_mode.CROSSOVER"


def test_empty_geometry_has_absent_cell_bounds() -> None:
    layout = qca_layout()
    assert layout.get_extent().width == 0
    assert layout.coordinates() == []
    assert layout.bounding_box() == (None, None)


def test_geometry_is_cartesian() -> None:
    layout = qca_layout(extent=(10, 10, 2))

    assert len(layout.coordinates()) == 200
    for t in layout.coordinates():
        assert t <= (9, 9, 1)
        assert layout.is_within_bounds(t)

    for t in layout.ground_coordinates():
        assert t.z == 0

    assert sorted((cell.x, cell.y, cell.z) for cell in layout.adjacent_coordinates((2, 2))) == [
        (1, 2, 0),
        (2, 1, 0),
        (2, 3, 0),
        (3, 2, 0),
    ]


def test_cell_type_and_mode_assignment() -> None:
    layout = qca_layout((5, 5, 2), "OPEN", "crossing")

    assert layout.is_empty()
    assert layout.get_layout_name() == "crossing"

    layout.assign_cell_type((0, 2), qca_cell_type.INPUT)
    layout.assign_cell_type((2, 2), qca_cell_type.NORMAL)
    layout.assign_cell_type((2, 2, 1), qca_cell_type.NORMAL)
    layout.assign_cell_type((4, 2), qca_cell_type.OUTPUT)
    layout.assign_cell_mode((2, 2, 1), qca_cell_mode.CROSSOVER)
    layout.assign_cell_name((0, 2), "a")

    assert layout.num_cells() == 4
    assert layout.num_pis() == 1
    assert layout.num_pos() == 1
    assert layout.is_pi((0, 2))
    assert layout.pis() == [(0, 2)]
    assert layout.get_cell_name((0, 2)) == "a"
    assert layout.get_cell_type((2, 2, 1)) == qca_cell_type.NORMAL
    assert layout.get_cell_mode((2, 2, 1)) == qca_cell_mode.CROSSOVER
    assert layout.get_cell_mode((2, 2)) == qca_cell_mode.NORMAL
    assert layout.is_empty_cell((1, 1))

    layout.assign_cell_type((2, 2, 1), qca_cell_type.EMPTY)
    assert layout.is_empty_cell((2, 2, 1))
    assert layout.get_cell_mode((2, 2, 1)) == qca_cell_mode.NORMAL
    layout.assign_cell_type((2, 2, 1), qca_cell_type.NORMAL)
    assert layout.get_cell_mode((2, 2, 1)) == qca_cell_mode.NORMAL


def test_crossing_modes_and_reset() -> None:
    """Each crossing cell stores its mode and normal mode clears an override."""
    layout = qca_layout((5, 5, 2))
    for cell in [(2, 2, 0), (2, 1, 1), (2, 2, 1)]:
        layout.assign_cell_type(cell, qca_cell_type.NORMAL)
    layout.assign_cell_mode((2, 1, 1), qca_cell_mode.VERTICAL)
    layout.assign_cell_mode((2, 2, 1), qca_cell_mode.CROSSOVER)

    assert layout.is_crossing_layer((2, 2, 1))
    assert layout.num_cells() == 3
    assert [layout.get_cell_mode(cell) for cell in [(2, 2, 0), (2, 1, 1), (2, 2, 1)]] == [
        qca_cell_mode.NORMAL,
        qca_cell_mode.VERTICAL,
        qca_cell_mode.CROSSOVER,
    ]
    layout.assign_cell_mode((2, 1, 1), qca_cell_mode.NORMAL)
    assert layout.get_cell_mode((2, 1, 1)) == qca_cell_mode.NORMAL


@pytest.mark.parametrize(("tile_x", "tile_y"), [(0, 1), (1, 0)])
def test_reject_zero_tile_sizes(tile_x: int, tile_y: int) -> None:
    with pytest.raises(ValueError, match="positive"):
        qca_layout((2, 2), "2DDWave", "", tile_x, tile_y)


def test_clocked_construction() -> None:
    layout = qca_layout((5, 5, 2), "USE", "crossing", 5, 5)
    assert layout.width() == 5
    assert layout.layers() == 2
    assert layout.get_layout_name() == "crossing"
    assert layout.is_clocking_scheme("USE")
    assert layout.get_tile_size_x() == 5
    assert layout.get_tile_size_y() == 5


@pytest.mark.parametrize("copy_layout", [copy.copy, copy.deepcopy])
def test_clock_zones_and_synchronization_elements(copy_layout: Callable[[qca_layout], qca_layout]) -> None:
    """Clock numbers and synchronization elements belong to clock zones, i.e., to tiles of cells."""
    layout = qca_layout((5, 5), "2DDWave", "", 2, 2)
    assert layout.get_tile_size_x() == 2
    assert layout.is_clocking_scheme("2DDWAVE")
    with pytest.raises(ValueError, match="positive"):
        layout.set_tile_size_x(0)
    with pytest.raises(ValueError, match="positive"):
        qca_layout((5, 5), "2DDWave", "", 0, 1)

    assert layout.get_clock_zone((3, 2)) == (1, 1)
    layout.assign_clock_number((1, 1), 3)
    layout.assign_synchronization_element((1, 1), 2)
    assert layout.get_clock_number((2, 2)) == 3
    assert layout.get_synchronization_element((3, 3)) == 2
    assert layout.get_synchronization_element((1, 1)) == 0
    assert layout.is_synchronization_element((3, 2, 1))
    assert not layout.is_synchronization_element((1, 1))
    assert layout.get_synchronization_element((4, 4)) == 0
    assert layout.num_se() == 1

    duplicate = copy_layout(layout)
    assert duplicate == layout
    duplicate.assign_clock_number((1, 1), 0)
    duplicate.assign_synchronization_element((1, 1), 0)
    duplicate.assign_cell_type((0, 0), qca_cell_type.NORMAL)
    duplicate.assign_cell_mode((0, 0), qca_cell_mode.ROTATED)
    assert duplicate.num_se() == 0
    assert layout.get_clock_number((2, 2)) == 3
    assert layout.num_se() == 1
    assert layout.is_empty()
    assert duplicate != layout

    layout.replace_clocking_scheme("USE")
    assert layout.is_clocking_scheme("USE")
    assert layout.get_clocking_scheme_name() == "USE"
    with pytest.raises(ValueError, match="Unknown clocking scheme"):
        layout.replace_clocking_scheme("3DDWave")
    with pytest.raises(ValueError, match="clocking scheme"):
        qca_layout((5, 5), "3DDWave")
