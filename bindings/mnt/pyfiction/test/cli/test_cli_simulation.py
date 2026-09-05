# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of the SiDB simulation commands."""

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt import pyfiction

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path

    from .conftest import Shell


@pytest.fixture
def or_gate(shell: Shell, resource: Callable[[str], str]) -> Shell:
    """A shell holding SiQAD's OR gate and the OR truth table.

    Returns:
        The shell.
    """
    shell.ok(f"read {resource('siqad_or_gate.sqd')}; tt -t 1110")
    return shell


@pytest.fixture
def xor_gate(shell: Shell, resource: Callable[[str], str]) -> Shell:
    """A shell holding a Bestagon XOR gate and the XOR truth table.

    Returns:
        The shell.
    """
    shell.ok(f"read {resource('hex_21_inputsdbp_xor_v1.sqd')}; tt -t 0110")
    return shell


@pytest.mark.parametrize("engine", ["quickexact", "quicksim", "clustercomplete"])
def test_ground_state_engines(or_gate: Shell, engine: str) -> None:
    if engine == "clustercomplete" and not hasattr(pyfiction, "clustercomplete"):
        pytest.skip("pyfiction was built without ALGLIB")
    or_gate.ok(f"{engine} -e 5.6 -l 5 -m -0.28")
    entry = or_gate.session.cell_layouts.current()
    assert entry.result is not None
    assert entry.engine
    simulation = or_gate.session.log[-1]["result"]["cell_layout"]["simulation"]  # type: ignore[index]
    assert simulation["stable_states"] >= 1
    assert simulation["ground_state_energy_ev"] is not None
    assert "ground state" in or_gate.ok("print -c")
    assert "already simulated" in or_gate.fails(engine)


def test_simulation_needs_an_sidb_layout(mux21_shell: Shell) -> None:
    mux21_shell.ok("ortho; cell")
    assert "SiDB layout is needed" in mux21_shell.fails("quickexact")


def test_physical_parameter_validation(or_gate: Shell) -> None:
    assert "must be positive" in or_gate.fails("quickexact -e 0")
    assert "at least 1" in or_gate.fails("quicksim -i 0")
    assert "usage" in or_gate.fails("quickexact --base 4")


def test_temp(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f"read {resource('hex_21_inputsdbp_xor_v1.sqd')}; tt -t 0110; temp -g --engine quickexact")
    result = shell.session.log[-1]["result"]
    assert 0 < result["critical_temperature_k"] <= 400  # type: ignore[index]
    assert result["gate_based"] is True  # type: ignore[index]
    assert "critical temperature" in shell.output
    shell.ok("current -c 0; temp -t 50")
    assert "in (0, 1]" in shell.fails("temp -c 2")


def test_opdom_grid_search(xor_gate: Shell, tmp_path: Path) -> None:
    csv = tmp_path / "opdom.csv"
    xor_gate.ok(f"opdom {csv} --x-min 5.6 --x-max 5.8 --x-step 0.1 --y-min 5 --y-max 5.2 --y-step 0.1")
    result = xor_gate.session.log[-1]["result"]
    assert result["num_evaluated_parameter_combinations"] == 9  # type: ignore[index]
    assert "epsilon_r,lambda_tf" in csv.read_text(encoding="utf-8")


def test_opdom_sampling_and_errors(xor_gate: Shell, tmp_path: Path) -> None:
    csv = tmp_path / "opdom.csv"
    xor_gate.ok(f"opdom {csv} -r 4 -o --x-min 5.6 --x-max 5.8 --x-step 0.1 --y-min 5 --y-max 5.2 --y-step 0.1")
    assert "usage" in xor_gate.fails(f"opdom {csv} -r 2 -f 2")
    assert "at least 1" in xor_gate.fails(f"opdom {csv} -f 0")
    assert "positive step" in xor_gate.fails(f"opdom {csv} --x-step 0")
    assert "usage" in xor_gate.fails(f"opdom {csv} -x foo")
