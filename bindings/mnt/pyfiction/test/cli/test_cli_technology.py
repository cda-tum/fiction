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
    from collections.abc import Callable

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
    assert "Lattice" in mux21_shell.ok("ps -c")


def test_cell_topology_mismatch(mux21_shell: Shell) -> None:
    mux21_shell.ok("ortho")
    assert "needs a even_row_hex layout" in mux21_shell.fails("cell -l bestagon")
    assert "needs a odd_column_cartesian layout" in mux21_shell.fails("cell -l topolinano")
    mux21_shell.ok("hex")
    assert "needs a cartesian layout" in mux21_shell.fails("cell")


@pytest.mark.parametrize("commands", ["cell", "cell -l sim7-mol", "hex; cell -l bestagon"])
def test_area(mux21_shell: Shell, commands: str) -> None:
    """Every technology has an area model, and every one of them honors the cell dimensions."""
    mux21_shell.ok(f"ortho; {commands}")
    mux21_shell.ok("area")
    default = mux21_shell.session.log[-1]["result"]
    assert isinstance(default, dict)
    assert default["area_nm2"] > 0
    # only the overrides the user gave are reported; the rest are the technology's own
    assert set(default) == {"area_nm2"}

    mux21_shell.ok("area -x 20 -y 20 --hspace 1 --vspace 1")
    overridden = mux21_shell.session.log[-1]["result"]
    assert isinstance(overridden, dict)
    assert overridden["width_nm"] == 20.0
    assert overridden["area_nm2"] != default["area_nm2"]


def test_area_of_a_sidb_layout(shell: Shell, resource: Callable[[str], str]) -> None:
    """SiDB area comes from the bounding box, and the cell dimensions apply to it too."""
    shell.ok(f"read {resource('siqad_or_gate.sqd')}; area")
    default = shell.session.log[-1]["result"]
    assert isinstance(default, dict)
    assert default["area_nm2"] > 0
    shell.ok("area --hspace 1 --vspace 1")
    widened = shell.session.log[-1]["result"]
    assert isinstance(widened, dict)
    assert widened["area_nm2"] > default["area_nm2"]


@pytest.mark.parametrize("spelling", ["qca-one", "QCAONE", "QCA ONE", "qca_one", "QcA-oNe"])
def test_cell_library_spellings(mux21_shell: Shell, spelling: str) -> None:
    """Hyphens, underscores, spaces, and case are ignored in a gate library name, as in the C++ shell."""
    mux21_shell.ok(f"ortho; cell -l '{spelling}'")
    assert len(mux21_shell.session.cell_layouts) == 1
