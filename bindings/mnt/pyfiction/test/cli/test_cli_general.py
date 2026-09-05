# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of the general commands: help, version, scripts, and the store commands."""

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path

    from .conftest import Shell


def test_help_lists_every_category(shell: Shell) -> None:
    output = shell.ok("help")
    for category in ("General", "Input and output", "Logic", "Physical design", "Technology", "Simulation"):
        assert category in output
    assert "ortho" in output


def test_help_of_a_command(shell: Shell) -> None:
    assert "usage: read" in shell.ok("help read")
    assert "unknown command" in shell.fails("help frobnicate")


def test_version(shell: Shell) -> None:
    assert "fiction" in shell.ok("version")
    assert shell.session.log[-1]["result"]["version"]  # type: ignore[index]


def test_quit_stops_the_session(shell: Shell) -> None:
    shell.ok("quit")
    assert not shell.session.running


def test_store_lists_elements_and_marks_the_active_one(mux21_shell: Shell, resource: Callable[[str], str]) -> None:
    mux21_shell.ok(f"read {resource('xor2.v')}")
    output = mux21_shell.ok("store -n")
    assert "  0: mux21 (TEC)" in output
    assert "* 1: xor2 (TEC)" in output
    assert "(empty)" in mux21_shell.ok("store -g")


def test_current_selects_an_element(mux21_shell: Shell, resource: Callable[[str], str]) -> None:
    mux21_shell.ok(f"read {resource('xor2.v')}")
    mux21_shell.ok("current -n 0")
    assert "* 0: mux21" in mux21_shell.ok("store -n")
    assert "out of range" in mux21_shell.fails("current -n 7")
    assert "exactly one store" in mux21_shell.fails("current 0")


def test_ps_prints_statistics(mux21_shell: Shell) -> None:
    output = mux21_shell.ok("ps -n")
    assert "gates" in output
    assert "depth" in output
    mux21_shell.ok("ortho")
    output = mux21_shell.ok("ps -g")
    assert "clocking" in output
    assert "throughput" in output


def test_print_layout(mux21_shell: Shell) -> None:
    mux21_shell.ok("ortho")
    assert len(mux21_shell.ok("print -g").splitlines()) > 1
    mux21_shell.ok("cell")
    assert len(mux21_shell.ok("print -c").splitlines()) > 1
    assert "show -n" in mux21_shell.fails("print -n")


def test_print_truth_table(shell: Shell) -> None:
    shell.ok("tt -t 1000")
    output = shell.ok("print -t")
    assert "hex: 8" in output
    assert "bin: 1000" in output


def test_show_writes_files(mux21_shell: Shell, tmp_path: Path) -> None:
    mux21_shell.ok(f"show -n --silent -o {tmp_path / 'net.dot'}")
    assert "digraph" in (tmp_path / "net.dot").read_text(encoding="utf-8")
    mux21_shell.ok("ortho")
    mux21_shell.ok(f"show -g --silent -o {tmp_path / 'lyt.dot'}")
    assert (tmp_path / "lyt.dot").stat().st_size > 0
    mux21_shell.ok("cell")
    mux21_shell.ok(f"show -c --silent --simple -o {tmp_path / 'lyt.svg'}")
    assert "<svg" in (tmp_path / "lyt.svg").read_text(encoding="utf-8")


def test_show_uses_a_temporary_file(mux21_shell: Shell) -> None:
    output = mux21_shell.ok("show -n --silent")
    assert "wrote " in output
    mux21_shell.session.close()


def test_clear(mux21_shell: Shell) -> None:
    mux21_shell.ok("ortho; clear -g")
    assert len(mux21_shell.session.gate_layouts) == 0
    assert len(mux21_shell.session.networks) == 1
    mux21_shell.ok("clear")
    assert len(mux21_shell.session.networks) == 0


def test_source_runs_nested_scripts(shell: Shell, tmp_path: Path, resource: Callable[[str], str]) -> None:
    inner = tmp_path / "inner.fs"
    inner.write_text(f"read {resource('mux21.v')}\n", encoding="utf-8")
    outer = tmp_path / "outer.fs"
    outer.write_text(f"source {inner}\northo\n", encoding="utf-8")
    shell.ok(f"source {outer}")
    assert len(shell.session.gate_layouts) == 1


def test_source_stops_at_a_failure(shell: Shell, tmp_path: Path) -> None:
    script = tmp_path / "bad.fs"
    script.write_text("version\nfrobnicate\nversion\n", encoding="utf-8")
    output = shell.fails(f"source {script}")
    assert "stopped at a failing command" in output
    assert "cannot read script" in shell.fails(f"source {tmp_path / 'missing.fs'}")


@pytest.mark.parametrize("flag", ["-t", "-g", "-c"])
def test_print_of_an_empty_store_fails(shell: Shell, flag: str) -> None:
    assert "in store" in shell.fails(f"print {flag}")
