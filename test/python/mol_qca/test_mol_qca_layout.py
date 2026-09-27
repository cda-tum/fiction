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

from mnt.pyfiction.mol_qca import MolecularQCALayout, MolQcaCellType, mol_qca_clock_number


def test_cell_types_carry_clock_phases() -> None:
    assert mol_qca_clock_number(MolQcaCellType.NORMAL1) == 0
    assert mol_qca_clock_number(MolQcaCellType.NORMAL4) == 3
    assert mol_qca_clock_number(MolQcaCellType.INPUT) == 0


def test_planar_layout() -> None:
    layout = MolecularQCALayout((3, 0, 1), "wire")

    assert layout.z() == 0
    assert layout.get_layout_name() == "wire"

    layout.assign_cell_type((0, 0), MolQcaCellType.INPUT)
    layout.assign_cell_type((1, 0), MolQcaCellType.NORMAL1)
    layout.assign_cell_type((2, 0), MolQcaCellType.NORMAL2)
    layout.assign_cell_type((3, 0), MolQcaCellType.OUTPUT)

    assert layout.num_cells() == 4
    assert layout.num_pis() == 1
    # the representation colors inputs and outputs with ANSI escapes
    assert re.sub(r"\x1b\[[0-9;]*m", "", repr(layout)) == "iabo\n\n"

    duplicate = copy.deepcopy(layout)
    duplicate.assign_cell_type((1, 0), MolQcaCellType.EMPTY)
    assert layout.get_cell_type((1, 0)) == MolQcaCellType.NORMAL1
    assert duplicate != layout
