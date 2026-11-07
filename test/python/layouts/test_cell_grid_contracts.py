# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import copy

from mnt.pyfiction.layouts import coordinate
from mnt.pyfiction.qca import qca_cell_type, qca_layout


def test_cell_grid_visits_outside_cells_and_clears_names() -> None:
    layout = qca_layout((4, 4), "OPEN", "AND")
    for position, kind in [
        ((0, 2), qca_cell_type.INPUT),
        ((2, 4), qca_cell_type.INPUT),
        ((2, 2), qca_cell_type.NORMAL),
        ((4, 2), qca_cell_type.OUTPUT),
    ]:
        layout.assign_cell_type(position, kind)
    layout.assign_cell_name((0, 2), "a")
    layout.assign_cell_name((2, 4), "b")
    layout.assign_cell_name((4, 2), "f")
    assert sorted(layout.cells()) == sorted(coordinate(c) for c in [(0, 2), (2, 4), (2, 2), (4, 2)])
    assert sorted(layout.pis()) == [coordinate(0, 2), coordinate(2, 4)]
    assert layout.pos() == [coordinate(4, 2)]
    assert (layout.num_cells(), layout.num_pis(), layout.num_pos()) == (4, 2, 1)
    assert layout.is_pi((0, 2))
    assert layout.is_po((4, 2))
    assert not layout.is_pi((2, 2))
    assert layout.is_empty_cell((1, 1))
    assert layout.get_cell_type((1, 1)) == qca_cell_type.EMPTY
    assert not layout.get_cell_name((2, 2))
    layout.assign_cell_name((2, 4), "")
    assert not layout.get_cell_name((2, 4))
    assert layout.get_cell_type((2, 4)) == qca_cell_type.INPUT
    layout.assign_cell_type((0, 2), qca_cell_type.EMPTY)
    assert not layout.get_cell_name((0, 2))
    assert layout.is_empty_cell((0, 2))
    assert (layout.num_cells(), layout.num_pis()) == (3, 1)


def test_cell_grid_copy_isolates_extent_names_and_cells() -> None:
    layout = qca_layout((5, 5), "OPEN", "original")
    layout.assign_cell_type((0, 2), qca_cell_type.NORMAL)
    for clone in (copy.copy(layout), copy.deepcopy(layout)):
        assert clone == layout
        clone.resize((10, 10))
        clone.set_layout_name("copy")
        clone.assign_cell_type((0, 2), qca_cell_type.INPUT)
        clone.assign_cell_name((0, 2), "a")
        assert clone != layout
        assert layout.width() == 5
        assert layout.get_layout_name() == "original"
        assert layout.get_cell_type((0, 2)) == qca_cell_type.NORMAL
        assert not layout.get_cell_name((0, 2))
