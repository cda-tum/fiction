# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of the general commands: help, version, scripts, and the store commands."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction import inml_layout, mol_qca_layout, mol_qca_technology
from mnt.pyfiction.cli.stores import CellEntry, element_name

if TYPE_CHECKING:
    from collections.abc import Callable

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


@pytest.mark.parametrize("simulate", [False, True])
def test_show_sidb_svg(shell: Shell, resource: Callable[[str], str], tmp_path: Path, *, simulate: bool) -> None:
    shell.ok(f"read {resource('siqad_or_gate.sqd')}")
    if simulate:
        shell.ok("quickexact")
    path = tmp_path / "sidb.svg"
    shell.ok(f"show -c --silent -o {path}")
    assert "<svg" in path.read_text(encoding="utf-8")


def test_show_molecular_qca_svg(shell: Shell, tmp_path: Path) -> None:
    layout = mol_qca_layout((2, 0), "OPEN", "wire")
    layout.assign_cell_type((0, 0), mol_qca_technology.cell_type.INPUT)
    layout.assign_cell_type((1, 0), mol_qca_technology.cell_type.NORMAL1)
    layout.assign_cell_type((2, 0), mol_qca_technology.cell_type.OUTPUT)
    shell.session.cell_layouts.add(CellEntry(layout))
    path = tmp_path / "molecular.svg"
    shell.ok(f"show -c --silent -o {path}")
    assert "<svg" in path.read_text(encoding="utf-8")


def test_show_rejects_inml_svg(shell: Shell, tmp_path: Path) -> None:
    shell.session.cell_layouts.add(CellEntry(inml_layout()))
    assert "no SVG drawer" in shell.fails(f"show -c --silent -o {tmp_path / 'inml.svg'}")


def test_show_opens_the_written_file(mux21_shell: Shell, tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    """The viewer is the platform's file opener, which takes the path itself, not a browser URL."""
    opened: list[list[str]] = []
    monkeypatch.setattr("mnt.pyfiction.cli.drawing.subprocess.Popen", lambda command, **_: opened.append(command))
    path = tmp_path / "network.dot"
    mux21_shell.ok(f"show -n -o {path}")
    assert opened == [[platform_opener(), str(path)]]
    assert "digraph" in path.read_text(encoding="utf-8")


def platform_opener() -> str:
    """Return the opener binary `show` reaches for on this platform.

    Returns:
        ``open`` on macOS, ``xdg-open`` elsewhere.
    """
    return "open" if sys.platform == "darwin" else "xdg-open"


def test_show_takes_an_explicit_program(mux21_shell: Shell, tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    """--program replaces the platform opener, substituting '{}' when the command carries one."""
    opened: list[list[str]] = []
    monkeypatch.setattr("mnt.pyfiction.cli.drawing.subprocess.Popen", lambda command, **_: opened.append(command))
    path = tmp_path / "network.dot"
    mux21_shell.ok(f"show -n -o {path} --program 'dot -Tpng'")
    mux21_shell.ok(f"show -n -o {path} --program 'viewer --file {{}} --wait'")
    assert opened == [["dot", "-Tpng", str(path)], ["viewer", "--file", str(path), "--wait"]]


def test_show_keeps_its_temporary_file_after_the_session_closes(
    mux21_shell: Shell, monkeypatch: pytest.MonkeyPatch
) -> None:
    """The viewer reads the file after `show` returns, so the file must outlive the session."""
    monkeypatch.setattr("mnt.pyfiction.cli.drawing.subprocess.Popen", lambda *_, **__: None)
    mux21_shell.ok("show -n")
    kept = Path(str(mux21_shell.session.log[-1]["result"]["file"]))  # type: ignore[index]
    mux21_shell.ok("show -n --delete")
    deleted = Path(str(mux21_shell.session.log[-1]["result"]["file"]))  # type: ignore[index]
    mux21_shell.session.close()
    assert kept.is_file()
    assert not deleted.exists()
    kept.unlink()


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


def test_print_of_a_simulated_sidb_layout_draws_one_picture(shell: Shell, resource: Callable[[str], str]) -> None:
    """The charge symbols replace the dots in a single picture, as the C++ shell printed it."""
    shell.ok(f"read {resource('siqad_or_gate.sqd')}")
    plain = shell.ok("print -c")
    shell.ok("quickexact")
    charged = shell.ok("print -c")
    assert "●" not in plain
    assert "●" in charged, charged
    assert "ground state energy" in charged
    # one lattice drawing, not the layout followed by a second one
    assert charged.count("⋅") <= plain.count("⋅")


def test_ps_all_describes_every_element(mux21_shell: Shell, resource: Callable[[str], str]) -> None:
    """--all walks the store instead of describing only the active element."""
    mux21_shell.ok(f"read {resource('xor2.v')}")
    output = mux21_shell.ok("ps -n --all")
    assert "mux21" in output
    assert "xor2" in output
    result = mux21_shell.session.log[-1]["result"]
    assert isinstance(result, dict)
    assert [entry["name"] for entry in result["network"]] == ["mux21", "xor2"]


def test_store_pop_removes_the_active_element(mux21_shell: Shell, resource: Callable[[str], str]) -> None:
    """--pop drops the active element and leaves the one before it active, as the C++ shell did."""
    mux21_shell.ok(f"read {resource('xor2.v')}")
    assert len(mux21_shell.session.networks) == 2
    mux21_shell.ok("store -n --pop")
    assert len(mux21_shell.session.networks) == 1
    assert element_name(mux21_shell.session.networks.current()) == "mux21"
    mux21_shell.ok("store -n --pop")
    assert len(mux21_shell.session.networks) == 0
    assert mux21_shell.session.networks.active is None
    assert "no network in store" in mux21_shell.fails("store -n --pop")


def test_store_pop_needs_a_store(mux21_shell: Shell) -> None:
    """Without a flag, --pop would empty stores the user did not name."""
    assert "select the stores to remove from" in mux21_shell.fails("store --pop")
    assert len(mux21_shell.session.networks) == 1


def test_store_pop_removes_nothing_when_one_store_is_empty(mux21_shell: Shell) -> None:
    """Every selected store is checked before any is touched, so a failure changes nothing."""
    assert "no gate-level layout in store" in mux21_shell.fails("store -n -g --pop")
    assert len(mux21_shell.session.networks) == 1


def test_exit_is_an_alias_of_quit(shell: Shell) -> None:
    """`exit` and `quit` are one command, and `help` lists it once."""
    listing = shell.ok("help")
    assert listing.count("quit, exit") == 1
    assert "\n  exit " not in listing
    shell.ok("exit")
    assert not shell.session.running
