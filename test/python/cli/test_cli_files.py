# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of reading and writing files and of creating truth tables."""

from __future__ import annotations

import xml.etree.ElementTree as ET  # ruff: ignore[suspicious-xml-etree-import] -- parse files produced by tested writers
from typing import TYPE_CHECKING

import pytest

from mnt.fiction.cli.stores import CellEntry, describe
from mnt.fiction.cli.topologies import FGL_READERS
from mnt.pyfiction import layouts, physical_design
from mnt.pyfiction.inml import inml_layout, inml_magnet_type
from mnt.pyfiction.layouts import shifted_cartesian_gate_layout
from mnt.pyfiction.layouts.io import write_fgl_layout
from mnt.pyfiction.networks import aig_network, mig_network, set_name, simulate_outputs, technology_network, xag_network
from mnt.pyfiction.qca import qca_layout
from mnt.pyfiction.sidb import sidb_layout
from mnt.pyfiction.sidb.io import read_sqd_layout

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path

    from .conftest import Shell


@pytest.mark.parametrize(
    ("network_type", "cls"),
    [("tec", technology_network), ("aig", aig_network), ("xag", xag_network), ("mig", mig_network)],
)
def test_read_verilog_as_every_type(shell: Shell, resource: Callable[[str], str], network_type: str, cls: type) -> None:
    shell.ok(f'read "{resource("mux21.v")}" --type {network_type}')
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
    shell.ok(f'read "{path}" --type {network_type}; simulate -n --store')
    assert shell.session.truth_tables.current().to_binary() == "1000"
    assert describe(shell.session.networks.current())["type"] == expected
    assert describe(shell.session.networks.current())["name"] == "and"


def test_read_blif_rejects_other_types(shell: Shell, tmp_path: Path, mux21_shell: Shell) -> None:
    blif = tmp_path / "mux21.blif"
    mux21_shell.ok(f'write_blif "{blif}"')
    shell.ok(f'read "{blif}"')
    assert "technology networks only" in shell.fails(f'read "{blif}" --type aig')


def test_read_rejects_a_directory(shell: Shell, tmp_path: Path) -> None:
    """Reading a folder is no longer supported; the message says to name one file."""
    assert "is a directory" in shell.fails(f'read "{tmp_path}"')
    assert len(shell.session.networks) == 0


def test_split_readers_take_their_own_format(shell: Shell, resource: Callable[[str], str]) -> None:
    """Each format-specific reader loads its format into the right store."""
    shell.ok(f'read_verilog "{resource("mux21.v")}"')
    assert len(shell.session.networks) == 1
    shell.ok(f'read_sqd "{resource("siqad_or_gate.sqd")}"')
    assert len(shell.session.cell_layouts) == 1


def test_split_readers_reject_another_format(shell: Shell, resource: Callable[[str], str]) -> None:
    """A reader named for one format refuses another, rather than guessing."""
    assert ".sqd" in shell.fails(f'read_sqd "{resource("mux21.v")}"')
    assert ".v" in shell.fails(f'read_verilog "{resource("siqad_or_gate.sqd")}"')
    assert len(shell.session.networks) == 0


def test_split_readers_reject_a_directory(shell: Shell, tmp_path: Path) -> None:
    assert "is a directory" in shell.fails(f'read_verilog "{tmp_path}"')


def test_read_aiger_accepts_both_aiger_suffixes(shell: Shell, tmp_path: Path, resource: Callable[[str], str]) -> None:
    """read_aiger reads binary .aig and ASCII .aag, dispatching on the file's own suffix."""
    shell.ok(f'read_verilog "{resource("mux21.v")}" --type aig')
    binary = tmp_path / "mux21.aig"
    shell.ok(f'write_aiger "{binary}"')
    shell.ok("clear -n")
    shell.ok(f'read_aiger "{binary}"')
    assert len(shell.session.networks) == 1


def test_read_missing_and_unknown_files(shell: Shell, tmp_path: Path) -> None:
    assert "no such file" in shell.fails(f'read "{tmp_path / "nope.v"}"')
    (tmp_path / "x.txt").write_text("", encoding="utf-8")
    assert "cannot read" in shell.fails(f'read "{tmp_path / "x.txt"}"')


def test_read_unparsable_file_reports_the_parser(shell: Shell, tmp_path: Path) -> None:
    bad = tmp_path / "bad.v"
    bad.write_text("module broken(\n", encoding="utf-8")
    assert "could not parse" in shell.fails(f'read "{bad}"')


def test_read_sqd(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f'read "{resource("siqad_or_gate.sqd")}"')
    entry = shell.session.cell_layouts.current()
    assert isinstance(entry.layout, sidb_layout)
    assert entry.layout.num_dots() > 0
    assert shell.session.log[-1]["result"]["cell_layout"]["technology"] == "SiDB"  # type: ignore[index]


def test_sqd_shell_round_trip(shell: Shell, resource: Callable[[str], str], tmp_path: Path) -> None:
    shell.ok(f'read "{resource("siqad_or_gate.sqd")}"')
    before = shell.session.cell_layouts.current().layout
    path = tmp_path / "gate.sqd"
    shell.ok(f'write_sqd "{path}"; clear -c; read "{path}"')
    after = shell.session.cell_layouts.current().layout
    assert isinstance(before, sidb_layout)
    assert isinstance(after, sidb_layout)
    assert after.num_dots() == before.num_dots()
    assert after.num_pis() == before.num_pis()
    assert after.num_pos() == before.num_pos()


@pytest.mark.parametrize("topology", ["cartesian", "shifted_cartesian", "hexagonal"])
def test_fgl_round_trip(shell: Shell, resource: Callable[[str], str], tmp_path: Path, topology: str) -> None:
    fgl = tmp_path / "layout.fgl"
    shell.ok(f'read "{resource("mux21.v")}"; ortho')
    if topology == "hexagonal":
        shell.ok("hex")
    if topology == "shifted_cartesian":
        layout = shifted_cartesian_gate_layout((1, 0), "2DDWave", "wire")
        source = layout.create_pi("a", (0, 0))
        layout.create_po(source, "f", (1, 0))
        shell.session.gate_layouts.add(layout)
    gates = shell.session.gate_layouts.current().num_gates()
    shell.ok(f'write_fgl "{fgl}"; clear -g; read "{fgl}" --topology {topology}')
    assert shell.session.gate_layouts.current().num_gates() == gates


@pytest.mark.parametrize(
    ("writer", "suffix"), [("write_verilog", ".v"), ("write_blif", ".blif"), ("write_dot -n", ".dot")]
)
def test_write_networks(mux21_shell: Shell, tmp_path: Path, writer: str, suffix: str) -> None:
    path = tmp_path / f"mux21{suffix}"
    mux21_shell.ok(f'{writer} "{path}"')
    assert path.stat().st_size > 0


def test_write_aiger_needs_an_aig(mux21_shell: Shell, tmp_path: Path, resource: Callable[[str], str]) -> None:
    assert "--type aig" in mux21_shell.fails(f'write_aiger "{tmp_path / "x.aig"}"')
    mux21_shell.ok(f'read "{resource("mux21.v")}" --type aig; write_aiger "{tmp_path / "x.aig"}"')
    assert (tmp_path / "x.aig").stat().st_size > 0


@pytest.mark.parametrize("suffix", [".qca", ".qll", ".svg", ".dot", ".fgl"])
def test_write_layouts(mux21_shell: Shell, tmp_path: Path, suffix: str) -> None:
    mux21_shell.ok("ortho; cell")
    path = tmp_path / f"mux21{suffix}"
    mux21_shell.ok(f'write_{suffix[1:]} "{path}"')
    assert path.stat().st_size > 0


def test_write_via_layer_flags(mux21_shell: Shell, tmp_path: Path) -> None:
    mux21_shell.ok("ortho; cell")
    mux21_shell.ok(f'write_qca --no-via-layers "{tmp_path / "a.qca"}"')


def test_write_technology_mismatch(mux21_shell: Shell, tmp_path: Path) -> None:
    mux21_shell.ok("ortho; cell")
    assert "not a qca_layout" in mux21_shell.fails(f'write_sqd "{tmp_path / "x.sqd"}"')
    assert "not a qca_layout" in mux21_shell.fails(f'write_qcc "{tmp_path / "x.qcc"}"')


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


def test_write_defaults_to_the_element_name(
    mux21_shell: Shell, tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    """Without a file, the active element's name and the format suffix make one in the current directory."""
    monkeypatch.chdir(tmp_path)
    mux21_shell.ok("write_verilog")
    assert (tmp_path / "mux21.v").is_file()
    mux21_shell.ok("ortho; cell; write_qca")
    assert (tmp_path / "mux21.qca").is_file()


def test_write_refuses_a_directory(mux21_shell: Shell, tmp_path: Path) -> None:
    assert "is a directory" in mux21_shell.fails(f'write_verilog "{tmp_path}"')


def test_writer_selects_format_independently_of_filename(mux21_shell: Shell, tmp_path: Path) -> None:
    """The writer command determines serialization, including for extensionless output."""
    for name in ("output", "output.xyz", "output.blif"):
        path = tmp_path / name
        mux21_shell.ok(f'write_verilog "{path}"')
        assert "module top" in path.read_text(encoding="utf-8")
    assert "unknown command" in mux21_shell.fails("write output.v")


def test_write_dot_selects_the_store(mux21_shell: Shell, tmp_path: Path) -> None:
    """-n and -g read as ordinary store selection on a .dot file; a layout is the default."""
    mux21_shell.ok("ortho")
    for flag, expected in (("-n", "digraph"), ("-g", "digraph"), ("", "digraph")):
        path = tmp_path / f"out{flag or 'default'}.dot"
        mux21_shell.ok(f'write_dot "{path}" {flag}')
        assert expected in path.read_text(encoding="utf-8")


def test_write_dot_indexes_and_clock_colors(mux21_shell: Shell, tmp_path: Path) -> None:
    """The drawer flags the C++ shell exposed reach the DOT writers again."""
    mux21_shell.ok("ortho")
    plain = tmp_path / "plain.dot"
    colored = tmp_path / "colored.dot"
    mux21_shell.ok(f'write_dot "{plain}" -g')
    mux21_shell.ok(f'write_dot "{colored}" -g --clock-colors --indexes')
    assert plain.read_text(encoding="utf-8") != colored.read_text(encoding="utf-8")


@pytest.mark.parametrize("suffix", [".qll", ".sqd"])
def test_write_more_cell_formats(shell: Shell, resource: Callable[[str], str], tmp_path: Path, suffix: str) -> None:
    """The happy paths of the cell-level writers, not only their type errors."""
    if suffix == ".sqd":
        shell.ok(f'read "{resource("siqad_or_gate.sqd")}"')
    else:
        shell.ok(f'read "{resource("mux21.v")}"; ortho; cell')
    path = tmp_path / f"out{suffix}"
    shell.ok(f'write_{suffix[1:]} "{path}"')
    assert path.stat().st_size > 0


def test_write_qcc_component_name(shell: Shell, tmp_path: Path) -> None:
    """--component-name names the QCC component after the file, as the C++ `qcc -c` did."""
    layout = inml_layout((3, 0))
    layout.set_layout_name("mygate")
    cell = inml_magnet_type
    layout.assign_cell_type((0, 0), cell.INPUT)
    layout.assign_cell_type((1, 0), cell.NORMAL)
    layout.assign_cell_type((2, 0), cell.NORMAL)
    layout.assign_cell_type((3, 0), cell.OUTPUT)
    shell.session.cell_layouts.add(CellEntry(layout))

    named_after_the_layout = tmp_path / "wire.qcc"
    named_after_the_file = tmp_path / "component.qcc"
    shell.ok(f'write_qcc "{named_after_the_layout}"')
    shell.ok(f'write_qcc "{named_after_the_file}" --component-name')
    assert 'name="mygate"' in named_after_the_layout.read_text(encoding="utf-8")
    assert 'name="component"' in named_after_the_file.read_text(encoding="utf-8")


@pytest.mark.parametrize(
    ("writer", "options", "message"),
    [
        ("write_verilog", "--via-layers", "unrecognized arguments"),
        ("write_verilog", "--simple", "unrecognized arguments"),
        ("write_verilog", "--indexes", "unrecognized arguments"),
        ("write_dot", "--clock-colors -n", "--clock-colors requires a gate-level DOT drawing"),
    ],
)
def test_rejected_writer_options_preserve_destination(
    mux21_shell: Shell, tmp_path: Path, writer: str, options: str, message: str
) -> None:
    """Unsupported options fail before replacing an existing file."""
    path = tmp_path / "output"
    path.write_text("keep this file", encoding="utf-8")
    assert message in mux21_shell.fails(f'{writer} "{path}" {options}')
    assert path.read_text(encoding="utf-8") == "keep this file"


@pytest.mark.parametrize("name", ["", "../escape", "folder/name", r"folder\name", "C:escape"])
def test_implicit_output_requires_a_simple_name(
    shell: Shell, tmp_path: Path, monkeypatch: pytest.MonkeyPatch, name: str
) -> None:
    """An embedded name cannot choose an output directory."""
    monkeypatch.chdir(tmp_path)
    network = technology_network()
    set_name(network, name)
    shell.session.networks.add(network)
    message = "explicit output path" if name else "no name"
    assert message in shell.fails("write_verilog")
    assert not list(tmp_path.glob("*.v"))


def test_implicit_gate_layout_filename(mux21_shell: Shell, tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    """FGL uses the active gate layout name when no filename is supplied."""
    monkeypatch.chdir(tmp_path)
    mux21_shell.ok("ortho; write_fgl")
    assert (tmp_path / "mux21.fgl").is_file()


def test_read_fgl_round_trips_a_gate_layout(shell: Shell, tmp_path: Path, resource: Callable[[str], str]) -> None:
    """read_fgl loads a layout written by `write_fgl`, under the topology the flag names."""
    shell.ok(f'read_verilog "{resource("mux21.v")}"')
    shell.ok("ortho")
    path = tmp_path / "mux21.fgl"
    shell.ok(f'write_fgl "{path}"')
    shell.ok("clear -g")
    shell.ok(f'read_fgl "{path}" --topology cartesian')
    assert len(shell.session.gate_layouts) == 1


def test_split_readers_report_a_missing_file(shell: Shell, tmp_path: Path) -> None:
    assert "no such file" in shell.fails(f'read_verilog "{tmp_path / "absent.v"}"')
    assert "no such file" in shell.fails(f'read_fgl "{tmp_path / "absent.fgl"}"')


@pytest.mark.parametrize(
    ("writer", "flags"),
    [
        ("write_dot", "-n -g"),
        ("write_fgl", "-n"),
        ("write_verilog", "-g"),
        ("write_verilog", "-c"),
        ("write_fgl", "--component-name"),
    ],
)
def test_conflicting_writer_options_do_not_touch_files(shell: Shell, tmp_path: Path, writer: str, flags: str) -> None:
    path = tmp_path / "output"
    path.write_text("original", encoding="utf-8")
    shell.fails(f'{writer} {flags} "{path}"')
    assert path.read_text(encoding="utf-8") == "original"


@pytest.mark.parametrize("topology", list(FGL_READERS))
@pytest.mark.parametrize("phases", [3, 4])
def test_all_topologies_round_trip_small_fixture(shell: Shell, tmp_path: Path, topology: str, phases: int) -> None:
    native = {
        "cartesian": "cartesian",
        "shifted_cartesian": "shifted_cartesian",
        "hexagonal": "hexagonal",
        "odd_column_cartesian": "shifted_cartesian",
        "even_row_hex": "hexagonal",
    }.get(topology, topology)
    layout = getattr(layouts, f"{native}_gate_layout")((2, 1), f"2DDWave{phases}", topology)
    source = layout.create_pi("a", (0, 0))
    layout.create_po(source, "f", (1, 0))
    path = tmp_path / f"{topology}.fgl"
    write_fgl_layout(layout, str(path))
    shell.ok(f'read_fgl "{path}" --topology {topology}')
    restored = shell.session.gate_layouts.current()
    assert restored.num_clocks() == phases
    assert restored.num_pis() == restored.num_pos() == 1
    assert simulate_outputs(restored) == simulate_outputs(layout)


@pytest.mark.parametrize(
    ("format_name", "content"),
    [
        ("aag", "aag 2 2 0 1 0\n2\n4\n2\ni0 apple\ni1 banana\no0 fruit\nc\n"),
        ("pla", ".i 2\n.o 1\n.ilb apple banana\n.ob fruit\n1- 1\n.e\n"),
    ],
)
def test_named_extensionless_bridges(shell: Shell, tmp_path: Path, format_name: str, content: str) -> None:
    source = tmp_path / "input without extension"
    source.write_text(content, encoding="utf-8")
    shell.ok(f'read --format {format_name} "{source}"')
    network = shell.session.networks.current()
    assert [network.get_name(pi) for pi in network.pis()] == ["apple", "banana"]
    assert simulate_outputs(network) == [("fruit", [True, False, True, False])]


@pytest.mark.parametrize("library", ["qca-one", "sim7-mol", "bestagon", "topolinano"])
def test_complete_design_and_export_workflows(shell: Shell, tmp_path: Path, library: str) -> None:
    if library == "topolinano" and not hasattr(physical_design, "exact_cartesian"):
        pytest.skip("pyfiction was built without Z3")
    source = tmp_path / "inverter.v"
    source.write_text("module top(a, f);\ninput a;\noutput f;\nassign f = ~a;\nendmodule\n", encoding="utf-8")
    shell.ok(f'read "{source}"')
    design = "exact --topolinano -s columnar --border-io --timeout 10" if library == "topolinano" else "ortho"
    shell.ok(design + "; check; equiv -n -g")
    assert "strongly equivalent" in shell.output
    if library == "bestagon":
        shell.ok("hex; equiv -n -g")
        assert "equivalent" in shell.output
    shell.ok(f"cell --library {library}; area")
    entry = shell.session.cell_layouts.current()
    formats = {
        "qca-one": ("qca", "qll", "svg"),
        "sim7-mol": ("qll", "svg"),
        "bestagon": ("sqd", "svg"),
        "topolinano": ("qcc", "qll"),
    }[library]
    for suffix in formats:
        destination = tmp_path / f"exported.{suffix}"
        options = " --component-name" if suffix == "qcc" else ""
        shell.ok(f'write_{suffix} "{destination}"{options}')
        text = destination.read_text(encoding="utf-8")
        if suffix == "sqd":
            assert isinstance(entry.layout, sidb_layout)
            restored = read_sqd_layout(str(destination))
            assert restored.num_dots() == entry.layout.num_dots()
            assert restored.num_pis() == entry.layout.num_pis()
            assert restored.num_pos() == entry.layout.num_pos()
        elif suffix == "qca":
            assert isinstance(entry.layout, qca_layout)
            assert text.count("[TYPE:QCADCell]") >= entry.layout.num_cells()
        else:
            root = ET.fromstring(text)  # ruff: ignore[suspicious-xml-element-tree-usage] -- writer output
            assert (
                root.tag.endswith("svg")
                if suffix == "svg"
                else root.tag == {"qll": "qcalayout", "qcc": "qcacomponent"}[suffix]
            )
            if suffix == "qll":
                assert not isinstance(entry.layout, sidb_layout)
                items = root.findall("./layout/item")
                pins = root.findall("./layout/pin")
                if library == "topolinano":
                    lengths = [item.find("./property[@name='length']") for item in items]
                    assert (
                        sum(int(length.get("value", "1")) if length is not None else 1 for length in lengths)
                        == entry.layout.num_cells()
                    )
                else:
                    assert len(items) + len(pins) == entry.layout.num_cells()
                    assert all("layer" in item.attrib for item in items)
                assert len(pins) == entry.layout.num_pis() + entry.layout.num_pos()
            elif suffix == "qcc":
                assert "exported" in text
    drawing = tmp_path / "layout.dot"
    shell.ok(f'show -g --silent -o "{drawing}"')
    assert "digraph" in drawing.read_text(encoding="utf-8")
