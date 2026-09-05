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

import pytest

from mnt.pyfiction import (
    lattice_site,
    quickexact,
    quickexact_params,
    sidb_100_lattice,
    sidb_111_lattice,
    sidb_layout,
    sidb_technology,
    write_sqd_sim_result,
)

if TYPE_CHECKING:
    from pathlib import Path


def test_write_sqd_sim_result(tmp_path: Path) -> None:
    """The writer emits a SiQAD simulation result.

    Args:
        tmp_path: Temporary output directory.
    """

    layout = sidb_layout()
    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(2, 0, 0), sidb_technology.cell_type.NORMAL)

    result = quickexact(layout, quickexact_params())
    assert result.charge_distributions

    filename = tmp_path / "result.xml"
    write_sqd_sim_result(result, str(filename))

    text = filename.read_text()
    assert text.startswith("<?xml")
    assert "<sim_out>" in text
    assert "<dbdot" in text


@pytest.mark.parametrize(
    "layout",
    [pytest.param(sidb_100_lattice(), id="100"), pytest.param(sidb_111_lattice(), id="111")],
)
def test_write_legacy_sqd_sim_result(tmp_path: Path, layout: sidb_100_lattice | sidb_111_lattice) -> None:
    """The writer accepts transitional simulation results.

    Args:
        tmp_path: Temporary output directory.
        layout: Cell-level layout to test.
    """

    layout.assign_cell_type((0, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((2, 0, 0), sidb_technology.cell_type.NORMAL)

    result = quickexact(layout, quickexact_params())
    assert result.charge_distributions

    filename = tmp_path / "legacy-result.xml"
    write_sqd_sim_result(result, str(filename))

    assert "<sim_out>" in filename.read_text()
