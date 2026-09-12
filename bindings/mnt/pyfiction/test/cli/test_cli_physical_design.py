# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of the physical design commands."""

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt import pyfiction
from mnt.pyfiction import cartesian_gate_layout, hexagonal_gate_layout, shifted_cartesian_gate_layout

if TYPE_CHECKING:
    from collections.abc import Callable

    from .conftest import Shell


def test_ortho(mux21_shell: Shell) -> None:
    mux21_shell.ok("ortho -v")
    layout = mux21_shell.session.gate_layouts.current()
    assert isinstance(layout, cartesian_gate_layout)
    assert layout.get_clocking_scheme_name() == "2DDWAVE"
    assert "Num gates" in mux21_shell.output


def test_ortho_three_clock_phases(mux21_shell: Shell) -> None:
    """-n 3 asks the heuristic for the three-phase clocking the library also supports."""
    mux21_shell.ok("ortho -n 3")
    three = mux21_shell.session.gate_layouts.current()
    mux21_shell.ok("ortho -n 4")
    four = mux21_shell.session.gate_layouts.current()
    assert three.num_clocks() == 3
    assert four.num_clocks() == 4


def test_ortho_converts_other_network_types(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f"read {resource('mux21.v')} --type aig; ortho")
    assert isinstance(shell.session.gate_layouts.current(), cartesian_gate_layout)


def test_gold(mux21_shell: Shell) -> None:
    mux21_shell.ok("gold -r -e high_efficiency -c wires --seed 1")
    assert isinstance(mux21_shell.session.gate_layouts.current(), cartesian_gate_layout)
    assert "at least 1" in mux21_shell.fails("gold -n 0")


def test_hex(mux21_shell: Shell) -> None:
    mux21_shell.ok("ortho; hex -i -o -p")
    assert isinstance(mux21_shell.session.gate_layouts.current(), hexagonal_gate_layout)
    assert "Cartesian layout is needed" in mux21_shell.fails("hex")


def test_optimize(mux21_shell: Shell) -> None:
    mux21_shell.ok("ortho")
    original = mux21_shell.session.gate_layouts.current()
    area_before = original.area()
    mux21_shell.ok("optimize -v; optimize -w")
    assert len(mux21_shell.session.gate_layouts) == 3
    assert original.area() == area_before
    assert mux21_shell.session.gate_layouts.items[1].area() <= area_before
    assert "must be positive" in mux21_shell.fails("optimize -t 0")


def test_optimize_needs_2ddwave(mux21_shell: Shell) -> None:
    mux21_shell.ok("ortho; hex")
    assert "Cartesian layout is needed" in mux21_shell.fails("optimize")


@pytest.mark.skipif(not hasattr(pyfiction, "exact_cartesian"), reason="pyfiction was built without Z3")
def test_exact(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f"read {resource('xor2.v')}; exact -x -b -s 2ddwave -t 60")
    layout = shell.session.gate_layouts.current()
    assert isinstance(layout, cartesian_gate_layout)
    assert layout.get_clocking_scheme_name() == "2DDWAVE"
    assert "not a clocking scheme" in shell.fails("exact -s nope")


@pytest.mark.skipif(not hasattr(pyfiction, "exact_cartesian"), reason="pyfiction was built without Z3")
def test_exact_topolinano(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f"read {resource('xor2.v')}; exact -x -b -s columnar --topolinano -t 60")
    assert isinstance(shell.session.gate_layouts.current(), shifted_cartesian_gate_layout)
    assert shell.session.log[-1]["result"]["gate_layout"]["topology"] == "odd_column_cartesian"  # type: ignore[index]
