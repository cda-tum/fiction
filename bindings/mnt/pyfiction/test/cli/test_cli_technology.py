# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of the gate library and area commands."""

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction import mol_qca_layout, qca_layout, sidb_layout

if TYPE_CHECKING:
    from .conftest import Shell


def test_cell_qca_one_and_sim7(mux21_shell: Shell) -> None:
    mux21_shell.ok("ortho; cell; cell -l SIM7-MOL")
    assert isinstance(mux21_shell.session.cell_layouts.items[0].layout, qca_layout)
    assert isinstance(mux21_shell.session.cell_layouts.items[1].layout, mol_qca_layout)
    assert mux21_shell.session.log[-1]["result"]["cell_layout"]["technology"] == "molQCA"  # type: ignore[index]


def test_cell_bestagon(mux21_shell: Shell) -> None:
    mux21_shell.ok("ortho; hex; cell -l bestagon")
    entry = mux21_shell.session.cell_layouts.current()
    assert isinstance(entry.layout, sidb_layout)
    assert "lattice" in mux21_shell.ok("ps -c")


def test_cell_topology_mismatch(mux21_shell: Shell) -> None:
    mux21_shell.ok("ortho")
    assert "needs a hexagonal layout" in mux21_shell.fails("cell -l bestagon")
    assert "needs a shifted_cartesian layout" in mux21_shell.fails("cell -l topolinano")
    mux21_shell.ok("hex")
    assert "needs a cartesian layout" in mux21_shell.fails("cell")


@pytest.mark.parametrize(
    ("commands", "expected"),
    [("cell", 18.0), ("cell -l sim7-mol", None), ("hex; cell -l bestagon", 0.0)],
)
def test_area(mux21_shell: Shell, commands: str, expected: float | None) -> None:
    mux21_shell.ok(f"ortho; {commands}")
    if expected is None:
        assert "no area model" in mux21_shell.fails("area")
        return
    mux21_shell.ok("area")
    result = mux21_shell.session.log[-1]["result"]
    assert result["cell_width_nm"] == expected  # type: ignore[index]
    assert result["area_nm2"] > 0  # type: ignore[index]
    mux21_shell.ok("area -x 20 -y 20 --hspace 1 --vspace 1")
    assert mux21_shell.session.log[-1]["result"]["cell_width_nm"] == 20.0  # type: ignore[index]
