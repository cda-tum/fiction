# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``write_sqd_sim_result``."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction.sidb import DotTag, LatticeSite, SiDBLayout
from mnt.pyfiction.sidb.simulation import QuickExactParams, quickexact
from mnt.pyfiction.sidb.simulation.io import write_sqd_sim_result

if TYPE_CHECKING:
    from pathlib import Path


def test_write_sqd_sim_result(tmp_path: Path) -> None:
    """The writer emits a SiQAD simulation result.

    Args:
        tmp_path: Temporary output directory.
    """

    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(2, 0, 0), DotTag.NORMAL)

    result = quickexact(layout, params=QuickExactParams())
    assert result.charge_distributions

    filename = tmp_path / "result.xml"
    write_sqd_sim_result(result, str(filename))

    text = filename.read_text()
    assert text.startswith("<?xml")
    assert "<sim_out>" in text
    assert "<dbdot" in text
