# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import tempfile
from pathlib import Path

from mnt.pyfiction.fcn.io import write_qll_layout
from mnt.pyfiction.layouts.coords import offset_coordinate
from mnt.pyfiction.mol_qca import mol_qca_cell_type, mol_qca_layout


def test_write_mol_qca_layout() -> None:
    with tempfile.TemporaryDirectory() as tmp_dir:
        filename = Path(tmp_dir) / "mol_qca.qll"

        layout = mol_qca_layout(offset_coordinate(1, 0), "molQCA")
        layout.assign_cell_type(offset_coordinate(0, 0), mol_qca_cell_type.NORMAL1)
        layout.assign_cell_type(offset_coordinate(1, 0), mol_qca_cell_type.NORMAL2)

        write_qll_layout(layout, str(filename))

        qll = filename.read_text(encoding="utf-8")

        assert '<settings tech="MolFCN">' in qll
        assert '<item tech="MolFCN" name="Bisferrocene"/>' in qll
        assert '<property name="phase" value="0"/>' in qll
        assert '<property name="phase" value="1"/>' in qll
