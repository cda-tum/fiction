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

from mnt.pyfiction import (
    lattice_site,
    quickexact,
    quickexact_params,
    sidb_layout,
    sidb_technology,
    write_location_and_ground_state,
)

if TYPE_CHECKING:
    from pathlib import Path


def test_write_location_and_ground_state(tmp_path: Path) -> None:
    layout = sidb_layout()
    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(2, 0, 0), sidb_technology.cell_type.NORMAL)

    result = quickexact(layout, quickexact_params())
    assert result.charge_distributions

    filename = tmp_path / "ground_state.txt"
    write_location_and_ground_state(result, str(filename))

    lines = filename.read_text().strip().split("\n")
    assert lines[0].startswith("x [nm]; y [nm];GS_0;")
    assert len(lines) == 3  # the header and one line per SiDB
    assert all(len(line.split(";")) == len(lines[0].split(";")) for line in lines[1:])
