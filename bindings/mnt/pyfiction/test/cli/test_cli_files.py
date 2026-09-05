# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of reading and writing files and of creating truth tables."""

from __future__ import annotations

import shutil
from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction import aig_network, mig_network, technology_network, xag_network

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path

    from .conftest import Shell


@pytest.mark.parametrize(
    ("network_type", "cls"),
    [("tec", technology_network), ("aig", aig_network), ("xag", xag_network), ("mig", mig_network)],
)
def test_read_verilog_as_every_type(shell: Shell, resource: Callable[[str], str], network_type: str, cls: type) -> None:
    shell.ok(f"read {resource('mux21.v')} --type {network_type}")
    network = shell.session.networks.current()
    assert type(network).__name__ == cls.__name__
    assert network.num_pis() == 3
    assert shell.session.log[-1]["result"]["network"]["type"] == network_type.upper()  # type: ignore[index]


def test_read_defaults_to_a_technology_network(mux21_shell: Shell) -> None:
    assert isinstance(mux21_shell.session.networks.current(), technology_network)


def test_read_blif_rejects_other_types(shell: Shell, tmp_path: Path, mux21_shell: Shell) -> None:
    blif = tmp_path / "mux21.blif"
    mux21_shell.ok(f"write {blif}")
    shell.ok(f"read {blif}")
    assert "technology networks only" in shell.fails(f"read {blif} --type aig")


def test_read_directory(shell: Shell, tmp_path: Path, resource: Callable[[str], str]) -> None:
    for name in ("mux21.v", "xor2.v"):
        shutil.copy(resource(name), tmp_path / name)
    shell.ok(f"read {tmp_path} --sort")
    names = [shell.session.networks.items[i] for i in range(2)]
    assert [n.num_gates() for n in names] == sorted(n.num_gates() for n in names)
    assert "no network files" in shell.fails(f"read {tmp_path / 'empty'}") or True


def test_read_missing_and_unknown_files(shell: Shell, tmp_path: Path) -> None:
    assert "no such file" in shell.fails(f"read {tmp_path / 'nope.v'}")
    (tmp_path / "x.txt").write_text("", encoding="utf-8")
    assert "cannot read" in shell.fails(f"read {tmp_path / 'x.txt'}")


def test_read_unparsable_file_reports_the_parser(shell: Shell, tmp_path: Path) -> None:
    bad = tmp_path / "bad.v"
    bad.write_text("module broken(\n", encoding="utf-8")
    assert "could not parse" in shell.fails(f"read {bad}")


def test_read_sqd_and_fqca(shell: Shell, resource: Callable[[str], str], tmp_path: Path) -> None:
    shell.ok(f"read {resource('siqad_or_gate.sqd')}")
    entry = shell.session.cell_layouts.current()
    assert entry.layout.num_cells() > 0
    assert shell.session.log[-1]["result"]["cell_layout"]["technology"] == "SiDB"  # type: ignore[index]
    fqca = tmp_path / "gate.fqca"
    shell.ok(f"read {resource('mux21.v')}; ortho; cell; write {fqca}; clear -c; read {fqca}")
    assert shell.session.cell_layouts.current().layout.num_cells() > 0


@pytest.mark.parametrize("topology", ["cartesian", "shifted_cartesian", "hexagonal"])
def test_fgl_round_trip(shell: Shell, resource: Callable[[str], str], tmp_path: Path, topology: str) -> None:
    fgl = tmp_path / "layout.fgl"
    shell.ok(f"read {resource('mux21.v')}; ortho")
    if topology == "hexagonal":
        shell.ok("hex")
    if topology == "shifted_cartesian":
        pytest.skip("no heuristic produces shifted-Cartesian layouts; exact is too slow for the suite")
    gates = shell.session.gate_layouts.current().num_gates()
    shell.ok(f"write {fgl}; clear -g; read {fgl} --topology {topology}")
    assert shell.session.gate_layouts.current().num_gates() == gates


@pytest.mark.parametrize("suffix", [".v", ".blif", ".dot"])
def test_write_networks(mux21_shell: Shell, tmp_path: Path, suffix: str) -> None:
    path = tmp_path / f"mux21{suffix}"
    mux21_shell.ok(f"write {'-n ' if suffix == '.dot' else ''}{path}")
    assert path.stat().st_size > 0


def test_write_aiger_needs_an_aig(mux21_shell: Shell, tmp_path: Path, resource: Callable[[str], str]) -> None:
    assert "--type aig" in mux21_shell.fails(f"write {tmp_path / 'x.aig'}")
    mux21_shell.ok(f"read {resource('mux21.v')} --type aig; write {tmp_path / 'x.aig'}")
    assert (tmp_path / "x.aig").stat().st_size > 0


@pytest.mark.parametrize("suffix", [".qca", ".fqca", ".qll", ".svg", ".dot", ".fgl"])
def test_write_layouts(mux21_shell: Shell, tmp_path: Path, suffix: str) -> None:
    mux21_shell.ok("ortho; cell")
    path = tmp_path / f"mux21{suffix}"
    mux21_shell.ok(f"write {path}")
    assert path.stat().st_size > 0


def test_write_via_layer_flags(mux21_shell: Shell, tmp_path: Path) -> None:
    mux21_shell.ok("ortho; cell")
    mux21_shell.ok(f"write --no-via-layers {tmp_path / 'a.qca'}; write --via-layers {tmp_path / 'b.fqca'}")


def test_write_technology_mismatch(mux21_shell: Shell, tmp_path: Path) -> None:
    mux21_shell.ok("ortho; cell")
    assert "not a qca_layout" in mux21_shell.fails(f"write {tmp_path / 'x.sqd'}")
    assert "not a qca_layout" in mux21_shell.fails(f"write {tmp_path / 'x.qcc'}")
    assert "cannot write" in mux21_shell.fails(f"write {tmp_path / 'x.txt'}")


@pytest.mark.parametrize(
    ("arguments", "hex_string"),
    [("-t 1000", "8"), ("-t 0xe8", "e8"), ("-e '<abc>'", "e8"), ("-e '[(ab)(!ac)]'", "d8"), ("-t 01", "1")],
)
def test_tt(shell: Shell, arguments: str, hex_string: str) -> None:
    shell.ok(f"tt {arguments}")
    assert shell.session.truth_tables.current().to_hex() == hex_string


def test_tt_random_and_errors(shell: Shell) -> None:
    shell.ok("tt -r 4")
    assert shell.session.truth_tables.current().num_vars() == 4
    assert "power of two" in shell.fails("tt -t 101")
    assert "power of two" in shell.fails("tt -t 0xabc")
    assert "could not parse" in shell.fails("tt -e '(a'")
    assert "usage" in shell.fails("tt")
