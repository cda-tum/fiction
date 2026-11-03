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

import pytest

from mnt.pyfiction.sidb import charge_distribution, lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.simulation import sidb_simulation_result
from mnt.pyfiction.sidb.simulation.engines import quickexact
from mnt.pyfiction.sidb.simulation.io import write_location_and_ground_state

if TYPE_CHECKING:
    from pathlib import Path


def test_write_location_and_ground_state(tmp_path: Path) -> None:
    """The writer emits one row per SiDB.

    Args:
        tmp_path: Temporary output directory.
    """

    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(2, 0, 0), sidb_dot_tag.NORMAL)

    result = quickexact(layout)
    assert result.charge_distributions

    filename = tmp_path / "ground_state.txt"
    write_location_and_ground_state(result, str(filename))

    lines = filename.read_text().strip().split("\n")
    assert lines[0].startswith("x [nm]; y [nm];GS_0;")
    assert len(lines) == 3  # the header and one line per SiDB
    assert all(len(line.split(";")) == len(lines[0].split(";")) for line in lines[1:])


@pytest.mark.parametrize(
    "energy",
    [
        pytest.param(None, id="mismatched-sites"),
        pytest.param(float("nan"), id="nan"),
        pytest.param(float("inf"), id="positive-infinity"),
        pytest.param(float("-inf"), id="negative-infinity"),
    ],
)
def test_invalid_input_preserves_destination(energy: float | None, tmp_path: Path) -> None:
    """Invalid distributions raise ValueError and preserve an existing file.

    Args:
        energy: Nonfinite energy, or None for mismatched distribution sites.
        tmp_path: Temporary output directory.
    """

    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
    distribution = charge_distribution() if energy is None else charge_distribution(layout)
    if energy is not None:
        distribution.assign_energy(energy)
    result = sidb_simulation_result()
    result.layout = layout
    result.charge_distributions = [distribution]
    filename = tmp_path / "ground_state.txt"
    filename.write_text("original")

    # The API promises the exception type, not diagnostic wording.
    with pytest.raises(ValueError):  # ruff: ignore[pytest-raises-too-broad]
        write_location_and_ground_state(result, str(filename))

    assert filename.read_text() == "original"
    assert list(tmp_path.iterdir()) == [filename]
