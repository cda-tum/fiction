# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.sidb import DefectType, Lattice, LatticeSite
from mnt.pyfiction.sidb.io import MissingPositionError, UnsupportedDefectIndexError, read_surface_defects

if TYPE_CHECKING:
    from pathlib import Path


def test_read_defect_matrix(tmp_path: Path) -> None:
    path = tmp_path / "defects.txt"
    path.write_text("[[0 1] [2 0]]", encoding="utf-8")
    layout = read_surface_defects(path, name="surface")
    assert layout.name == "surface"
    assert layout.lattice == Lattice.si_100_2x1()
    assert layout.num_dots() == 0
    assert layout.num_defects() == 2
    assert layout.get_defect(LatticeSite(1, 0, 0)).type == DefectType.DB
    assert layout.get_defect(LatticeSite(0, 0, 1)).type == DefectType.SI_VACANCY


@pytest.mark.parametrize(
    ("matrix", "error"),
    [("[[11]]", UnsupportedDefectIndexError), ("[[0 1] [1]]", MissingPositionError)],
)
def test_read_defect_matrix_rejects_invalid_input(tmp_path: Path, matrix: str, error: type[Exception]) -> None:
    path = tmp_path / "invalid.txt"
    path.write_text(matrix, encoding="utf-8")
    with pytest.raises(error):
        read_surface_defects(path)
