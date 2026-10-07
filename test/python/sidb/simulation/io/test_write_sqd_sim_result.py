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

from mnt.pyfiction.sidb import lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.simulation.engines import quickexact, quickexact_params
from mnt.pyfiction.sidb.simulation.io import write_sqd_sim_result

if TYPE_CHECKING:
    from pathlib import Path


def test_write_sqd_sim_result(tmp_path: Path) -> None:
    """The writer emits a SiQAD simulation result.

    Args:
        tmp_path: Temporary output directory.
    """

    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(2, 0, 0), sidb_dot_tag.NORMAL)

    result = quickexact(layout, quickexact_params())
    assert result.charge_distributions

    filename = tmp_path / "result.xml"
    write_sqd_sim_result(result, str(filename))

    text = filename.read_text()
    assert text.startswith("<?xml")
    assert "<sim_out>" in text
    assert "<dbdot" in text
