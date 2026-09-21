# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of equivalence checking and design rule checking."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction import (
    cartesian_gate_layout,
)

if TYPE_CHECKING:
    from collections.abc import Callable

    from .conftest import Shell


def test_equiv_network_against_layout(mux21_shell: Shell) -> None:
    mux21_shell.ok("ortho; equiv -n -g")
    assert "strongly equivalent" in mux21_shell.output
    assert mux21_shell.session.log[-1]["result"]["eq"] == "STRONG"  # type: ignore[index]


def test_equiv_two_networks(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f'read "{resource("xor2.v")}"; read "{resource("xnor2.v")}"; equiv -n')
    assert "not equivalent" in shell.output
    assert "needs two" in shell.fails("clear; read " + resource("xor2.v") + "; equiv -n")


def test_equiv_two_layouts(mux21_shell: Shell) -> None:
    mux21_shell.ok("ortho; optimize; equiv -g")
    assert "equivalent" in mux21_shell.output
    assert "select the stores" in mux21_shell.fails("equiv")


def test_check(mux21_shell: Shell) -> None:
    """`check` logs the whole nested design rule report, as the C++ shell did, and prints it."""
    mux21_shell.ok("ortho; check")
    result = mux21_shell.session.log[-1]["result"]
    assert isinstance(result, dict)
    assert result["DRVs"] == 0
    assert result["Warnings"] == 0
    for section in ("Unplaced nodes", "Dead placed nodes", "Missing connections", "I/O counts"):
        assert section in result, f"the report lost '{section}'"
    assert "violations" in mux21_shell.output


def test_drv_blocked_equivalence_is_a_report(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f'read "{resource("mux21.v")}"')
    layout = cartesian_gate_layout((1, 0), "2DDWave", "invalid")
    source = layout.create_pi("a", (0, 0))
    layout.create_po(source, "f", (0, 1))
    shell.session.gate_layouts.add(layout)
    shell.ok("equiv -n -g; version")
    result = shell.session.log[-2]["result"]
    assert isinstance(result, dict)
    assert result["eq"] == "NOT_CHECKED"
    assert "implementation" in str(result["reason"])
