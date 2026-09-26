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

from mnt.pyfiction.layouts.coords import offset_coordinate
from mnt.pyfiction.mol_qca import mol_qca_cell_type, mol_qca_clock_number, mol_qca_layout


def test_cell_types_carry_clock_phases() -> None:
    assert mol_qca_clock_number(mol_qca_cell_type.NORMAL1) == 0
    assert mol_qca_clock_number(mol_qca_cell_type.NORMAL4) == 3
    assert mol_qca_clock_number(mol_qca_cell_type.INPUT) == 0


def test_planar_layout() -> None:
    layout = mol_qca_layout(offset_coordinate(3, 0, 1), "wire")

    assert layout.z() == 0
    assert layout.get_layout_name() == "wire"

    layout.assign_cell_type(offset_coordinate(0, 0), mol_qca_cell_type.INPUT)
    layout.assign_cell_type(offset_coordinate(1, 0), mol_qca_cell_type.NORMAL1)
    layout.assign_cell_type(offset_coordinate(2, 0), mol_qca_cell_type.NORMAL2)
    layout.assign_cell_type(offset_coordinate(3, 0), mol_qca_cell_type.OUTPUT)

    assert layout.num_cells() == 4
    assert layout.num_pis() == 1
    # the representation colors inputs and outputs with ANSI escapes
    assert re.sub(r"\x1b\[[0-9;]*m", "", repr(layout)) == "iabo\n\n"

    duplicate = copy.deepcopy(layout)
    duplicate.assign_cell_type(offset_coordinate(1, 0), mol_qca_cell_type.EMPTY)
    assert layout.get_cell_type(offset_coordinate(1, 0)) == mol_qca_cell_type.NORMAL1
    assert duplicate != layout
