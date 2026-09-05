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

if TYPE_CHECKING:
    from collections.abc import Callable

    from .conftest import Shell


def test_equiv_network_against_layout(mux21_shell: Shell) -> None:
    mux21_shell.ok("ortho; equiv -n -g")
    assert "strongly equivalent" in mux21_shell.output
    assert mux21_shell.session.log[-1]["result"]["eq"] == "STRONG"  # type: ignore[index]


def test_equiv_two_networks(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f"read {resource('xor2.v')}; read {resource('xnor2.v')}; equiv -n")
    assert "not equivalent" in shell.output
    assert "needs two" in shell.fails("clear; read " + resource("xor2.v") + "; equiv -n")


def test_equiv_two_layouts(mux21_shell: Shell) -> None:
    mux21_shell.ok("ortho; optimize; equiv -g")
    assert "equivalent" in mux21_shell.output
    assert "select the stores" in mux21_shell.fails("equiv")


def test_check(mux21_shell: Shell) -> None:
    mux21_shell.ok("ortho; check")
    result = mux21_shell.session.log[-1]["result"]
    assert result["violations"] == 0  # type: ignore[index]
    assert "DRV" in mux21_shell.output or "violation" in mux21_shell.output.lower()
