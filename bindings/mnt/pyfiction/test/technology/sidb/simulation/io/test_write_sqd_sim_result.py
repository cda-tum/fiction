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

from mnt.pyfiction import (
    charge_distribution_surface_100,
    quickexact,
    quickexact_params,
    sidb_100_lattice,
    sidb_technology,
    write_sqd_sim_result,
)

if TYPE_CHECKING:
    from pathlib import Path


def test_write_sqd_sim_result(tmp_path: Path) -> None:
    layout = sidb_100_lattice((2, 1))
    layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((2, 0), sidb_technology.cell_type.NORMAL)

    result = quickexact(charge_distribution_surface_100(layout), quickexact_params())
    assert result.charge_distributions

    filename = tmp_path / "result.xml"
    write_sqd_sim_result(result, str(filename))

    text = filename.read_text()
    assert text.startswith("<?xml")
    assert "<sim_out>" in text
