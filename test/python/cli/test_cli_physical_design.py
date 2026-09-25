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

from mnt.fiction.cli.topologies import FGL_READERS
from mnt.pyfiction import physical_design
from mnt.pyfiction.layouts import cartesian_gate_layout, hexagonal_gate_layout, shifted_cartesian_gate_layout
from mnt.pyfiction.networks import simulate_outputs

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path

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
    shell.ok(f'read "{resource("mux21.v")}" --type aig; ortho')
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


@pytest.mark.skipif(not hasattr(physical_design, "exact_cartesian"), reason="pyfiction was built without Z3")
def test_exact(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f'read "{resource("xor2.v")}"; exact -x -b -s 2ddwave -t 60')
    layout = shell.session.gate_layouts.current()
    assert isinstance(layout, cartesian_gate_layout)
    assert layout.get_clocking_scheme_name() == "2DDWAVE"
    assert "not a clocking scheme" in shell.fails("exact -s nope")


@pytest.mark.skipif(not hasattr(physical_design, "exact_cartesian"), reason="pyfiction was built without Z3")
def test_exact_topolinano(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f'read "{resource("xor2.v")}"; exact -x -b -s columnar --topolinano -t 60')
    assert isinstance(shell.session.gate_layouts.current(), shifted_cartesian_gate_layout)
    assert shell.session.log[-1]["result"]["gate_layout"]["topology"] == "odd_column_cartesian"  # type: ignore[index]


@pytest.mark.parametrize("topology", ["odd_row_hex", "even_row_hex", "odd_column_hex", "even_column_hex"])
@pytest.mark.parametrize("phases", [3, 4])
def test_direct_hexagonal_ortho_variants(
    shell: Shell, resource: Callable[[str], str], tmp_path: Path, topology: str, phases: int
) -> None:
    shell.ok(f'read "{resource("mux21.v")}"; ortho --topology {topology} --clock-phases {phases}')
    layout = shell.session.gate_layouts.current()
    assert layout.num_clocks() == phases
    expected = simulate_outputs(layout)
    assert expected == simulate_outputs(shell.session.networks.current())
    path = tmp_path / "hex.fgl"
    shell.ok(f'write_fgl "{path}"; read --topology {topology} "{path}"')
    restored = shell.session.gate_layouts.current()
    assert restored.num_clocks() == phases
    assert simulate_outputs(restored) == expected


@pytest.mark.skipif(not hasattr(physical_design, "exact_cartesian"), reason="pyfiction was built without Z3")
def test_failed_search_preserves_store_and_statistics(mux21_shell: Shell) -> None:
    before = len(mux21_shell.session.gate_layouts)
    output = mux21_shell.fails("exact --fixed-size 1 --timeout 0.0001")
    assert "bounds or timeout" in output
    assert "impossible" not in output
    assert len(mux21_shell.session.gate_layouts) == before
    assert isinstance(mux21_shell.session.log[-1]["stats"], dict)


@pytest.mark.parametrize("topology", [name for name in FGL_READERS if name not in {"shifted_cartesian", "hexagonal"}])
@pytest.mark.skipif(not hasattr(physical_design, "exact_cartesian"), reason="pyfiction was built without Z3")
def test_exact_topologies_preserve_function(shell: Shell, tmp_path: Path, topology: str) -> None:
    source = tmp_path / "wire.v"
    source.write_text("module top(a,f);\ninput a;\noutput f;\nassign f = a;\nendmodule\n", encoding="utf-8")
    shell.ok(f'read "{source}"; exact --topology {topology} --timeout 5')
    assert simulate_outputs(shell.session.gate_layouts.current()) == simulate_outputs(shell.session.networks.current())
