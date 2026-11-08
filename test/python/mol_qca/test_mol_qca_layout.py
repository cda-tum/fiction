# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import copy
import re
from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.mol_qca import mol_qca_cell_type, mol_qca_clock_number, mol_qca_layout

if TYPE_CHECKING:
    from collections.abc import Callable


@pytest.mark.parametrize(
    ("cell_type", "phase"),
    [
        (mol_qca_cell_type.EMPTY, 0),
        (mol_qca_cell_type.NORMAL1, 0),
        (mol_qca_cell_type.NORMAL2, 1),
        (mol_qca_cell_type.NORMAL3, 2),
        (mol_qca_cell_type.NORMAL4, 3),
        (mol_qca_cell_type.INPUT, 0),
        (mol_qca_cell_type.OUTPUT, 0),
        (mol_qca_cell_type.CONST_0, 0),
        (mol_qca_cell_type.CONST_1, 0),
    ],
)
def test_cell_types_carry_clock_phases(cell_type: mol_qca_cell_type, phase: int) -> None:
    assert mol_qca_clock_number(cell_type) == phase


@pytest.mark.parametrize("copy_layout", [copy.copy, copy.deepcopy])
def test_planar_layout(copy_layout: Callable[[mol_qca_layout], mol_qca_layout]) -> None:
    layout = mol_qca_layout(extent=(4, 1, 2), layout_name="wire")

    assert layout.layers() == 1
    assert layout.get_layout_name() == "wire"

    layout.assign_cell_type((0, 0), mol_qca_cell_type.INPUT)
    layout.assign_cell_type((1, 0), mol_qca_cell_type.NORMAL1)
    layout.assign_cell_type((2, 0), mol_qca_cell_type.NORMAL2)
    layout.assign_cell_type((3, 0), mol_qca_cell_type.OUTPUT)

    assert layout.num_cells() == 4
    assert layout.num_pis() == 1
    assert layout.num_pos() == 1
    assert mol_qca_clock_number(layout.get_cell_type((2, 0))) == 1
    # the representation colors inputs and outputs with ANSI escapes
    assert re.sub(r"\x1b\[[0-9;]*m", "", repr(layout)) == "iabo\n\n"

    duplicate = copy_layout(layout)
    assert duplicate == layout
    duplicate.assign_cell_type((1, 0), mol_qca_cell_type.EMPTY)
    assert layout.get_cell_type((1, 0)) == mol_qca_cell_type.NORMAL1
    assert duplicate != layout
