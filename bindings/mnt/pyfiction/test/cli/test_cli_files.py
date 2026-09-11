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

from mnt.pyfiction import (
    aig_network,
    inml_layout,
    inml_technology,
    mig_network,
    shifted_cartesian_gate_layout,
    technology_network,
    xag_network,
)
from mnt.pyfiction.cli.stores import CellEntry, describe

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


@pytest.mark.parametrize(("network_type", "expected"), [("aig", "AIG"), ("xag", "XAG"), ("mig", "MIG"), ("tec", "TEC")])
@pytest.mark.parametrize(
    ("suffix", "text"),
    [
        (".aag", "aag 3 2 0 1 1\n2\n4\n6\n6 2 4\ni0 a\ni1 b\no0 f\nc\nAND\n"),
        (".pla", ".i 2\n.o 1\n11 1\n.e\n"),
    ],
)
def test_read_aiger_and_pla(
    shell: Shell, tmp_path: Path, network_type: str, expected: str, suffix: str, text: str
) -> None:
    """aigverse reads .aag and .pla as AIGs, and --type converts the result into any network type."""
    path = tmp_path / f"and{suffix}"
    path.write_text(text, encoding="utf-8")
    shell.ok(f"read {path} --type {network_type}; simulate -n --store")
    assert shell.session.truth_tables.current().to_binary() == "1000"
    assert describe(shell.session.networks.current())["type"] == expected
    assert describe(shell.session.networks.current())["name"] == "and"


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
    empty = tmp_path / "empty"
    empty.mkdir()
    assert "no network files" in shell.fails(f"read {empty}")


def test_read_missing_and_unknown_files(shell: Shell, tmp_path: Path) -> None:
    assert "no such file" in shell.fails(f"read {tmp_path / 'nope.v'}")
    (tmp_path / "x.txt").write_text("", encoding="utf-8")
    assert "cannot read" in shell.fails(f"read {tmp_path / 'x.txt'}")


def test_read_unparsable_file_reports_the_parser(shell: Shell, tmp_path: Path) -> None:
    bad = tmp_path / "bad.v"
    bad.write_text("module broken(\n", encoding="utf-8")
    assert "could not parse" in shell.fails(f"read {bad}")


def test_read_sqd(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f"read {resource('siqad_or_gate.sqd')}")
    entry = shell.session.cell_layouts.current()
    assert entry.layout.num_dots() > 0
    assert shell.session.log[-1]["result"]["cell_layout"]["technology"] == "SiDB"  # type: ignore[index]


def test_sqd_shell_round_trip(shell: Shell, resource: Callable[[str], str], tmp_path: Path) -> None:
    shell.ok(f"read {resource('siqad_or_gate.sqd')}")
    before = shell.session.cell_layouts.current().layout
    path = tmp_path / "gate.sqd"
    shell.ok(f"write {path}; clear -c; read {path}")
    after = shell.session.cell_layouts.current().layout
    assert after.num_dots() == before.num_dots()
    assert after.num_pis() == before.num_pis()
    assert after.num_pos() == before.num_pos()
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
        layout = shifted_cartesian_gate_layout((1, 0), "2DDWave", "wire")
        source = layout.create_pi("a", (0, 0))
        layout.create_po(source, "f", (1, 0))
        shell.session.gate_layouts.add(layout)
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
    [
        ("-t 1000", "8"),
        ("-t 0xe8", "e8"),
        ("-e '<abc>'", "e8"),
        ("-e '[(ab)(!ac)]'", "d8"),
        ("-t 01", "1"),
        ("-t 0xe", "e"),
        ("-t 1110", "e"),
    ],
)
def test_tt(shell: Shell, arguments: str, hex_string: str) -> None:
    """Every source produces the table its argument names; one hex digit carries the four bits of two variables."""
    shell.ok(f"tt {arguments}")
    assert shell.session.truth_tables.current().to_hex() == hex_string


def test_tt_random_and_errors(shell: Shell) -> None:
    shell.ok("tt -r 4")
    assert shell.session.truth_tables.current().num_vars() == 4
    assert "power of two" in shell.fails("tt -t 101")
    assert "power of two" in shell.fails("tt -t 0xabc")
    assert "could not parse" in shell.fails("tt -e '(a'")
    assert "usage" in shell.fails("tt")


def test_read_directory_matches_mixed_case_suffixes(
    shell: Shell, tmp_path: Path, resource: Callable[[str], str]
) -> None:
    """Directory and single-file reads accept uppercase network extensions."""
    shutil.copy(resource("mux21.v"), tmp_path / "MUX21.V")
    shutil.copy(resource("xor2.v"), tmp_path / "xor2.v")
    shell.ok(f"read {tmp_path}")
    assert len(shell.session.networks) == 2
    assert not shell.stderr
    shell.ok(f"read {tmp_path / 'MUX21.V'}")


def test_read_directory_reports_a_broken_file_and_continues(
    shell: Shell, tmp_path: Path, resource: Callable[[str], str]
) -> None:
    """One unparsable file is reported and skipped; the rest of the directory still lands in the store."""
    shutil.copy(resource("mux21.v"), tmp_path / "mux21.v")
    (tmp_path / "broken.v").write_text("this is not Verilog", encoding="utf-8")
    shell.ok(f"read {tmp_path}")
    assert len(shell.session.networks) == 1
    assert "broken.v" in shell.stderr
    result = shell.session.log[-1]["result"]
    assert isinstance(result, dict)
    assert [failure["file"] for failure in result["failed"]] == [str(tmp_path / "broken.v")]


def test_read_directory_of_only_broken_files_fails(shell: Shell, tmp_path: Path) -> None:
    (tmp_path / "broken.v").write_text("this is not Verilog", encoding="utf-8")
    assert "none of the 1 network files" in shell.fails(f"read {tmp_path}")


def test_write_defaults_to_the_element_name(
    mux21_shell: Shell, tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    """Without a file, the active element's name and --format make one in the current directory."""
    monkeypatch.chdir(tmp_path)
    mux21_shell.ok("write -F v")
    assert (tmp_path / "mux21.v").is_file()
    mux21_shell.ok("ortho; cell; write -F qca")
    assert (tmp_path / "mux21.qca").is_file()


def test_write_refuses_a_directory(mux21_shell: Shell, tmp_path: Path) -> None:
    assert "is a directory" in mux21_shell.fails(f"write {tmp_path} -F v")


def test_write_needs_a_known_format(mux21_shell: Shell, tmp_path: Path) -> None:
    assert "cannot write" in mux21_shell.fails(f"write {tmp_path / 'mux21.xyz'}")
    assert "give a file with a known suffix" in mux21_shell.fails("write")


def test_write_dot_selects_the_store(mux21_shell: Shell, tmp_path: Path) -> None:
    """-n and -g read as ordinary store selection on a .dot file; a layout is the default."""
    mux21_shell.ok("ortho")
    for flag, expected in (("-n", "digraph"), ("-g", "digraph"), ("", "digraph")):
        path = tmp_path / f"out{flag or 'default'}.dot"
        mux21_shell.ok(f"write {path} {flag}")
        assert expected in path.read_text(encoding="utf-8")


def test_write_dot_indexes_and_clock_colors(mux21_shell: Shell, tmp_path: Path) -> None:
    """The drawer flags the C++ shell exposed reach the DOT writers again."""
    mux21_shell.ok("ortho")
    plain = tmp_path / "plain.dot"
    colored = tmp_path / "colored.dot"
    mux21_shell.ok(f"write {plain} -g")
    mux21_shell.ok(f"write {colored} -g --clock-colors --indexes")
    assert plain.read_text(encoding="utf-8") != colored.read_text(encoding="utf-8")


@pytest.mark.parametrize("suffix", [".qll", ".sqd"])
def test_write_more_cell_formats(shell: Shell, resource: Callable[[str], str], tmp_path: Path, suffix: str) -> None:
    """The happy paths of the cell-level writers, not only their type errors."""
    if suffix == ".sqd":
        shell.ok(f"read {resource('siqad_or_gate.sqd')}")
    else:
        shell.ok(f"read {resource('mux21.v')}; ortho; cell")
    path = tmp_path / f"out{suffix}"
    shell.ok(f"write {path}")
    assert path.stat().st_size > 0


def test_write_qcc_component_name(shell: Shell, tmp_path: Path) -> None:
    """--component-name names the QCC component after the file, as the C++ `qcc -c` did."""
    layout = inml_layout((3, 0))
    layout.set_layout_name("mygate")
    cell = inml_technology.cell_type
    layout.assign_cell_type((0, 0), cell.INPUT)
    layout.assign_cell_type((1, 0), cell.NORMAL)
    layout.assign_cell_type((2, 0), cell.NORMAL)
    layout.assign_cell_type((3, 0), cell.OUTPUT)
    shell.session.cell_layouts.add(CellEntry(layout))

    named_after_the_layout = tmp_path / "wire.qcc"
    named_after_the_file = tmp_path / "component.qcc"
    shell.ok(f"write {named_after_the_layout}")
    shell.ok(f"write {named_after_the_file} --component-name")
    assert 'name="mygate"' in named_after_the_layout.read_text(encoding="utf-8")
    assert 'name="component"' in named_after_the_file.read_text(encoding="utf-8")
