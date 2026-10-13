# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction.fcn.io import write_qll_layout
from mnt.pyfiction.mol_qca import mol_qca_cell_type, mol_qca_layout

if TYPE_CHECKING:
    from pathlib import Path


def test_write_mol_qca_layout(tmp_path: Path) -> None:
    """Serialize every occupied cell inside a size-based molQCA frame."""
    filename = tmp_path / "mol_qca.qll"

    layout = mol_qca_layout((2, 1), "molQCA")
    layout.assign_cell_type((0, 0), mol_qca_cell_type.NORMAL1)
    layout.assign_cell_type((1, 0), mol_qca_cell_type.NORMAL2)

    write_qll_layout(layout, str(filename))

    qll = filename.read_text(encoding="utf-8")

    assert '<settings tech="MolFCN">' in qll
    assert '<item tech="MolFCN" name="Bisferrocene"/>' in qll
    assert '<property name="phase" value="0"/>' in qll
    assert '<property name="phase" value="1"/>' in qll
