# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction import (
    lattice,
    lattice_site,
    read_sqd_layout,
    sidb_technology,
    write_sqd_layout,
)

if TYPE_CHECKING:
    from pathlib import Path


def test_read_write(resources_dir: Path) -> None:
    """The SQD reader accepts fixtures for both silicon lattice orientations."""
    read_sqd_layout(str(resources_dir / "hex_11_inputsdbp_inv_straight_v0_manual.sqd"))
    read_sqd_layout(str(resources_dir / "AND_mu_032_111_surface.sqd"))


def test_read_write_sidb_layout(resources_dir: Path, tmp_path: Path) -> None:
    """SQD export and import preserve the lattice, name, and cell types."""
    lyt = read_sqd_layout(str(resources_dir / "AND_mu_032_111_surface.sqd"), "and")
    assert lyt.get_layout_name() == "and"
    assert lyt.get_lattice() == lattice.si_111_1x1()
    assert lyt.num_cells() > 0

    lyt.assign_cell_type(lattice_site(-3, 2, 1), sidb_technology.cell_type.LOGIC)

    path = tmp_path / "and.sqd"
    write_sqd_layout(lyt, str(path))

    assert read_sqd_layout(str(path), "and") == lyt
