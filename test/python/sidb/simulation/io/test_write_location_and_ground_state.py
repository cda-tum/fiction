# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``write_location_and_ground_state``."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction.sidb import DotTag, LatticeSite, SiDBLayout
from mnt.pyfiction.sidb.io import write_location_and_ground_state
from mnt.pyfiction.sidb.simulation import QuickExactParams, quickexact

if TYPE_CHECKING:
    from pathlib import Path


def test_write_location_and_ground_state(tmp_path: Path) -> None:
    """The writer emits one row per SiDB.

    Args:
        tmp_path: Temporary output directory.
    """

    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(2, 0, 0), DotTag.NORMAL)

    result = quickexact(layout, params=QuickExactParams())
    assert result.charge_distributions

    filename = tmp_path / "ground_state.txt"
    write_location_and_ground_state(result, str(filename))

    lines = filename.read_text().strip().split("\n")
    assert lines[0].startswith("x [nm]; y [nm];GS_0;")
    assert len(lines) == 3  # the header and one line per SiDB
    assert all(len(line.split(";")) == len(lines[0].split(";")) for line in lines[1:])
