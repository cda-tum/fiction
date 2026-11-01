# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import time
from typing import TYPE_CHECKING

from mnt.pyfiction.sidb import lattice, lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.io import read_sqd_layout, write_sqd_layout

if TYPE_CHECKING:
    from pathlib import Path


def test_read_write(resources_dir: Path) -> None:
    """The SQD reader accepts fixtures for both silicon lattice orientations."""
    read_sqd_layout(str(resources_dir / "hex_11_inputsdbp_inv_straight_v0_manual.sqd"))
    read_sqd_layout(str(resources_dir / "AND_mu_032_111_surface.sqd"))


def test_read_write_sidb_layout(resources_dir: Path, tmp_path: Path) -> None:
    """SQD export and import preserve the lattice, name, and dot tags."""
    lyt = read_sqd_layout(str(resources_dir / "AND_mu_032_111_surface.sqd"), "and")
    assert lyt.get_layout_name() == "and"
    assert lyt.get_lattice() == lattice.si_111_1x1()
    assert lyt.num_dots() > 0

    lyt.assign_sidb(lattice_site(-3, 2, 1), sidb_dot_tag.LOGIC)

    path = tmp_path / "and.sqd"
    write_sqd_layout(lyt, str(path))

    assert read_sqd_layout(str(path), "and") == lyt


def test_write_snapshots_layout_for_progress_callback(tmp_path: Path) -> None:
    """A progress callback can mutate the source without changing the SQD snapshot."""
    lyt = sidb_layout()
    unvisited = lattice_site(1, 0, 0)
    lyt.assign_sidb(lattice_site(0, 0, 0))
    lyt.assign_sidb(unvisited)
    reports: list[tuple[int, int]] = []

    def on_progress(_task: str, done: int, total: int) -> None:
        reports.append((done, total))
        if done == 0:
            time.sleep(0.15)
        elif 0 < done < total:
            lyt.assign_sidb(unvisited, sidb_dot_tag.INPUT)

    path = tmp_path / "snapshot.sqd"
    write_sqd_layout(lyt, str(path), on_progress=on_progress)

    assert any(0 < done < total for done, total in reports)
    assert lyt.get_dot_tag(unvisited) == sidb_dot_tag.INPUT
    exported = read_sqd_layout(str(path))
    assert exported.num_dots() == 2
    assert exported.get_dot_tag(unvisited) == sidb_dot_tag.NORMAL
