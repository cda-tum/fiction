# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Adversarial CLI contracts: interfaces, persistence, and process safety."""

from __future__ import annotations

import io
import json
import os
import shutil
import stat
import subprocess  # ruff: ignore[suspicious-subprocess-import] -- bounded native crash regressions
import sys
import xml.etree.ElementTree as ET  # ruff: ignore[suspicious-xml-etree-import] -- parse files produced by tested writers
from pathlib import Path
from typing import TYPE_CHECKING

import pytest
from prompt_toolkit.completion import CompleteEvent
from prompt_toolkit.document import Document
from rich.console import Console

from mnt import pyfiction as fiction
from mnt.pyfiction.cli.app import CommandCompleter
from mnt.pyfiction.cli.commands.files import FGL_READERS
from mnt.pyfiction.cli.registry import REGISTRY
from mnt.pyfiction.cli.session import Session, json_value, tokenize
from mnt.pyfiction.cli.stores import Store

if TYPE_CHECKING:
    from collections.abc import Callable

    from .conftest import Shell


def test_pop_selects_predecessor() -> None:
    store: Store[int] = Store("number")
    for value in range(4):
        store.add(value)
    store.select(2)
    removed = store.pop()
    assert removed == 2
    assert store.current() == 1
    store.select(0)
    store.pop()
    assert store.current() == 1


@pytest.mark.parametrize("expression", ["p", "[ap]", "(a"])
def test_rejected_expression_preserves_contents(expression: str) -> None:
    table = fiction.dynamic_truth_table(1)
    table.create_from_binary_string("10")
    with pytest.raises(ValueError, match=r"expression|variable"):
        table.create_from_expression(expression)
    assert table.to_binary() == "10"


@pytest.mark.parametrize(
    "code",
    [
        "f.dynamic_truth_table(0).create_from_expression('p')",
        "f.dynamic_truth_table(38)",
        "f.dynamic_truth_table(64)",
        "f.dynamic_truth_table(4294967295)",
    ],
)
def test_invalid_truth_tables_fail_without_native_crash(code: str) -> None:
    result = subprocess.run(  # ruff: ignore[subprocess-without-shell-equals-true] -- fixed Python interpreter and test input
        [
            sys.executable,
            "-c",
            "from mnt import pyfiction as f\ntry:\n "
            + code
            + "\nexcept ValueError:\n pass\nelse:\n raise AssertionError('accepted unsafe input')",
        ],
        capture_output=True,
        text=True,
        timeout=20,
        check=False,
    )
    assert result.returncode == 0, result.stderr


def test_default_truth_table_is_zero() -> None:
    assert fiction.dynamic_truth_table().to_binary() == "0"


def test_quoted_boundaries_and_adjacent_completion() -> None:
    assert tokenize('source ";"; help # comment') == [["source", ";"], ["help"]]
    for text, expected in [
        ("help;he", "help"),
        ("help rea", "read"),
        ("cell --library bes", "bestagon"),
        ("ortho --topology odd_row", "odd_row_hex"),
    ]:
        completions = CommandCompleter().get_completions(Document(text), CompleteEvent(completion_requested=True))
        assert expected in [item.text for item in completions]


def test_json_conversion_is_recursive_and_finite() -> None:
    value = json_value({"nested": {"bad": float("nan"), "path": Path("layout.fgl")}})
    assert json.loads(json.dumps(value, allow_nan=False)) == {"nested": {"bad": None, "path": "layout.fgl"}}


def test_logging_disabled_retains_no_history() -> None:
    with_console = Console(file=io.StringIO())
    session = Session(console=with_console)
    try:
        assert session.execute("version; version")
        assert session.log == []
    finally:
        session.close()


def test_log_failure_still_cleans_temporary_files(tmp_path: Path) -> None:
    errors = io.StringIO()
    session = Session(log_path=tmp_path, errors=Console(file=errors))
    temporary = session.temp_file(".v")
    temporary.write_text("data", encoding="utf-8")
    session.close()
    assert not temporary.exists()
    assert session.close_failed
    assert "cannot write log" in errors.getvalue()


@pytest.mark.parametrize(
    "command",
    [
        "tt -r 64",
        "random -n -1 -g 2",
        "exact --timeout nan",
        "area --width -1",
        "quickexact --epsilon-r nan",
        "quickexact --epsilon-r inf",
    ],
)
def test_invalid_numeric_input_is_a_command_failure(shell: Shell, command: str) -> None:
    shell.fails(command)


@pytest.mark.parametrize(
    ("flags", "filename"),
    [("-n -g", "a.dot"), ("-n", "a.fgl"), ("-g", "a.v"), ("-c", "a.v"), ("--component-name", "a.fgl")],
)
def test_conflicting_writer_options_do_not_touch_files(shell: Shell, tmp_path: Path, flags: str, filename: str) -> None:
    path = tmp_path / filename
    path.write_text("original", encoding="utf-8")
    shell.fails(f'write {flags} "{path}"')
    assert path.read_text(encoding="utf-8") == "original"


@pytest.mark.parametrize("topology", list(FGL_READERS))
def test_all_topologies_round_trip_small_fixture(tmp_path: Path, topology: str) -> None:
    native = {
        "cartesian": "cartesian",
        "shifted_cartesian": "shifted_cartesian",
        "hexagonal": "hexagonal",
        "odd_column_cartesian": "shifted_cartesian",
        "even_row_hex": "hexagonal",
    }.get(topology, topology)
    layout = getattr(fiction, f"{native}_gate_layout")((2, 1), "2DDWave", topology)
    source = layout.create_pi("a", (0, 0))
    layout.create_po(source, "f", (1, 0))
    path = tmp_path / f"{topology}.fgl"
    fiction.write_fgl_layout(layout, str(path))
    restored = FGL_READERS[topology](str(path))
    assert restored.num_pis() == restored.num_pos() == 1
    assert fiction.simulate_outputs(restored) == fiction.simulate_outputs(layout)


def test_duplicate_output_names_preserve_order(tmp_path: Path) -> None:
    path = tmp_path / "outputs.v"
    path.write_text(
        "module top(a, f, g);\ninput a;\noutput f, g;\nassign f = a;\nassign g = ~a;\nendmodule\n", encoding="utf-8"
    )
    network = fiction.read_technology_network(str(path))
    network.set_output_name(0, "same")
    network.set_output_name(1, "same")
    outputs = fiction.simulate_outputs(network)
    assert [name for name, bits in outputs] == ["same", "same"]
    assert outputs[0][1] != outputs[1][1]


def test_piped_entry_point() -> None:
    result = subprocess.run(
        [sys.executable, "-m", "mnt.pyfiction.cli", "--quiet"],
        input="version\nquit\n",
        capture_output=True,
        text=True,
        timeout=20,
        check=False,
    )
    assert result.returncode == 0, result.stderr
    assert "compiled" in result.stdout
    assert "fiction>" not in result.stdout


def test_stacked_fqca_preserves_all_cells(shell: Shell, resource: Callable[[str], str], tmp_path: Path) -> None:
    filename = resource("stacked_crossing.fqca")
    shell.ok(f'read "{filename}"')
    layout = shell.session.cell_layouts.current().layout
    assert layout.num_cells() == 14
    assert layout.z() == 2
    assert layout.num_pis() == layout.num_pos() == 2

    def metadata(lyt: fiction.stacked_qca_layout) -> list[tuple[object, ...]]:
        return sorted(
            (
                c.x,
                c.y,
                c.z,
                str(lyt.get_cell_type(c)),
                str(lyt.get_cell_mode(c)),
                lyt.get_cell_name(c),
                lyt.get_clock_number(c),
            )
            for c in lyt.cells()
        )

    destination = tmp_path / "crossing.fqca"
    shell.ok(f'write -c "{destination}"')
    restored = fiction.read_stacked_fqca_layout(str(destination))
    assert metadata(restored) == metadata(layout)
    with pytest.raises((ValueError, IndexError, RuntimeError)):
        fiction.read_fqca_layout(filename)


@pytest.mark.parametrize("simple", [False, True])
def test_fqca_import_preserves_svg_drawing(mux21_shell: Shell, tmp_path: Path, *, simple: bool) -> None:
    """CLI FQCA imports render like native imports at both drawing detail levels."""
    mux21_shell.ok("ortho; cell --library qca-one")
    before = tmp_path / "before.svg"
    after = tmp_path / "after.svg"
    fqca = tmp_path / "layout.fqca"
    option = " --simple" if simple else ""
    mux21_shell.ok(f'write -c "{fqca}"; read "{fqca}"')
    params = fiction.write_qca_layout_svg_params()
    params.simple = simple
    fiction.write_qca_layout_svg(fiction.read_fqca_layout(str(fqca)), str(before), params)
    mux21_shell.ok(f'write -c "{after}"{option}')
    assert after.read_text(encoding="utf-8") == before.read_text(encoding="utf-8")
    mux21_shell.ok(f'show -c --silent -o "{after}"{option}')
    assert after.read_text(encoding="utf-8") == before.read_text(encoding="utf-8")


@pytest.mark.parametrize("options", ["--inv", "--xnor --inv"])
def test_incomplete_mapping_is_bounded_and_preserves_store(resource: Callable[[str], str], options: str) -> None:
    code = """
import sys
from mnt.pyfiction.cli import Session
session = Session()
assert session.execute('read "' + sys.argv[1] + '"')
original = session.networks.current()
assert not session.execute('map ' + sys.argv[2])
assert session.networks.current() is original
assert len(session.networks) == 1
session.close()
"""
    result = subprocess.run(  # ruff: ignore[subprocess-without-shell-equals-true] -- fixed Python interpreter and test input
        [sys.executable, "-c", code, resource("xor2.v"), options],
        capture_output=True,
        text=True,
        timeout=20,
        check=False,
    )
    assert result.returncode == 0, result.stdout + result.stderr


@pytest.mark.parametrize("variables", [2, 3])
@pytest.mark.parametrize("target", ["TEC", "AIG", "XAG", "MIG"])
def test_all_gate_functions_preserve_interfaces_and_formats(tmp_path: Path, variables: int, target: str) -> None:
    labels = ["apple", "banana", "cherry"][:variables]
    source = tmp_path / "source.blif"
    for function in range(1 << (1 << variables)):
        rows = [
            "".join(str((index >> bit) & 1) for bit in range(variables)) + " 1"
            for index in range(1 << variables)
            if function & (1 << index)
        ]
        source.write_text(
            ".model top\n.inputs "
            + " ".join(labels)
            + "\n.outputs fruit inverted one\n.names "
            + " ".join(labels)
            + " fruit\n"
            + "\n".join(rows or ["-" * variables + " 0"])
            + "\n.names fruit inverted\n0 1\n.names one\n1\n.end\n",
            encoding="utf-8",
        )
        original = fiction.read_technology_network(str(source))
        expected = fiction.simulate_outputs(original)
        network = fiction.convert_network(original, getattr(fiction.network_target, target))
        assert network.num_pis() == variables
        assert [network.get_name(pi) for pi in network.pis()] == labels
        assert fiction.simulate_outputs(network) == expected
        for suffix, writer, reader in [
            ("v", fiction.write_verilog, fiction.read_technology_network),
            ("blif", fiction.write_blif, fiction.read_technology_network),
            *([("aig", fiction.write_aiger, fiction.read_aig_network)] if target == "AIG" else []),
        ]:
            destination = tmp_path / f"roundtrip.{suffix}"
            writer(network, str(destination))
            restored = reader(str(destination))
            assert restored.num_pis() == variables, (target, suffix, function)
            assert [restored.get_name(pi) for pi in restored.pis()] == labels
            assert fiction.simulate_outputs(restored) == expected, (target, suffix, function)


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
    assert fiction.simulate_outputs(network) == [("fruit", [True, False, True, False])]


@pytest.mark.parametrize("topology", ["odd_row_hex", "even_row_hex", "odd_column_hex", "even_column_hex"])
@pytest.mark.parametrize("phases", [3, 4])
def test_direct_hexagonal_ortho_variants(
    shell: Shell, resource: Callable[[str], str], topology: str, phases: int
) -> None:
    shell.ok(f'read "{resource("mux21.v")}"; ortho --topology {topology} --clock-phases {phases}')
    layout = shell.session.gate_layouts.current()
    assert layout.num_clocks() == phases
    assert fiction.simulate_outputs(layout) == fiction.simulate_outputs(shell.session.networks.current())


def test_decoding_failure_reports_file_and_line(shell: Shell, tmp_path: Path) -> None:
    path = tmp_path / "bad.fiction"
    path.write_bytes(b"version\n\xff")
    output = shell.fails(f'source "{path}"')
    assert f"{path}:2" in output
    assert "UTF-8" in output
    assert "Traceback" not in output


def test_drv_blocked_equivalence_is_a_report(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f'read "{resource("mux21.v")}"')
    layout = fiction.cartesian_gate_layout((1, 0), "2DDWave", "invalid")
    source = layout.create_pi("a", (0, 0))
    layout.create_po(source, "f", (0, 1))
    shell.session.gate_layouts.add(layout)
    shell.ok("equiv -n -g; version")
    result = shell.session.log[-2]["result"]
    assert isinstance(result, dict)
    assert result["eq"] == "NOT_CHECKED"
    assert "implementation" in str(result["reason"])


@pytest.mark.parametrize("value", ["2", "-1", "18446744073709551616", "1garbage"])
def test_fgl_rejects_unrepresentable_layers(tmp_path: Path, value: str) -> None:
    layout = fiction.cartesian_gate_layout((1, 0), "2DDWave", "wire")
    source = layout.create_pi("a", (0, 0))
    layout.create_po(source, "f", (1, 0))
    path = tmp_path / "invalid.fgl"
    fiction.write_fgl_layout(layout, str(path))
    path.write_text(path.read_text(encoding="utf-8").replace("<z>0</z>", f"<z>{value}</z>", 1), encoding="utf-8")
    with pytest.raises(fiction.fgl_parsing_error, match=r"range|integer"):
        fiction.read_cartesian_fgl_layout(str(path))


@pytest.mark.parametrize(
    ("command", "expected"),
    [
        ("version", "compiled"),
        ("print -t", "hex:"),
        ("store -n", "Index"),
        ("simulate -n", "(0x"),
        ("equiv -n -g", "equivalent"),
        ("area", "nm²"),
    ],
)
def test_quiet_retains_requested_results(mux21_shell: Shell, command: str, expected: str) -> None:
    mux21_shell.ok("ortho; cell; tt -t 1000")
    mux21_shell.session.quiet = True
    assert expected in mux21_shell.ok(command)


def test_failed_search_preserves_store_and_statistics(mux21_shell: Shell) -> None:
    before = len(mux21_shell.session.gate_layouts)
    output = mux21_shell.fails("exact --fixed-size 1 --timeout 0.0001")
    assert "bounds or timeout" in output
    assert "impossible" not in output
    assert len(mux21_shell.session.gate_layouts) == before
    assert isinstance(mux21_shell.session.log[-1]["stats"], dict)


def test_mapping_with_statistics_preserves_function(mux21_shell: Shell) -> None:
    expected = fiction.simulate_outputs(mux21_shell.session.networks.current())
    mux21_shell.ok("map --and --inv --verbose")
    assert fiction.simulate_outputs(mux21_shell.session.networks.current()) == expected
    result = mux21_shell.session.log[-1]["result"]
    assert isinstance(result, dict)
    assert isinstance(result["stats"], dict)
    assert isinstance(result["stats"]["mapper_stats"], dict)
    assert result["stats"]["mapper_stats"]["mapping_error"] is False


def test_all_help_has_inputs_defaults_and_example(shell: Shell) -> None:
    for name in REGISTRY:
        output = shell.ok(f"help {name}")
        for section in ("Inputs:", "Defaults:", "Restrictions:", "Example:"):
            assert section in output, name


def test_fgl_preserves_labels_and_synchronization(tmp_path: Path) -> None:
    layout = fiction.cartesian_gate_layout((2, 0), "2DDWave", "A & B < C")
    source = layout.create_pi("a&b", (0, 0))
    wire = layout.create_buf(source, (1, 0))
    layout.create_po(wire, "f<g", (2, 0))
    layout.assign_synchronization_element((1, 0), 2)
    path = tmp_path / "sync.fgl"
    fiction.write_fgl_layout(layout, str(path))
    restored = fiction.read_cartesian_fgl_layout(str(path))
    assert fiction.get_name(restored) == "A & B < C"
    assert restored.num_se() == 1
    assert restored.get_synchronization_element((1, 0)) == 2
    assert fiction.simulate_outputs(restored) == fiction.simulate_outputs(layout)


@pytest.mark.parametrize("flags", ["-n", "-g"])
@pytest.mark.parametrize("symbolic", [False, True])
def test_real_graphviz_renders_svg(mux21_shell: Shell, tmp_path: Path, flags: str, *, symbolic: bool) -> None:
    if shutil.which("dot") is None:
        pytest.skip("Graphviz is not installed")
    mux21_shell.ok("ortho")
    destination = tmp_path / "drawing with spaces.svg"
    target = tmp_path / "target.svg"
    if symbolic:
        target.write_text("original", encoding="utf-8")
        try:
            destination.symlink_to(target.name)
        except OSError as error:
            if sys.platform == "win32" and error.winerror == 1314:
                pytest.skip("Windows did not grant symbolic-link creation privileges")
            raise
    else:
        destination.write_text("original", encoding="utf-8")
    if os.name == "posix":
        destination.chmod(0o600)
    mux21_shell.ok(f'show {flags} --silent -o "{destination}"')
    assert "<svg" in destination.read_text(encoding="utf-8")
    assert "digraph" in destination.with_suffix(".dot").read_text(encoding="utf-8")
    if os.name == "posix":
        assert stat.S_IMODE(destination.stat().st_mode) == 0o600
    if symbolic:
        assert destination.is_symlink()
        assert "<svg" in target.read_text(encoding="utf-8")
        target.unlink()
        mux21_shell.fails(f'show {flags} --silent -o "{destination}"')
        assert destination.is_symlink()
        assert not target.exists()


def test_graphviz_failure_preserves_destination(
    mux21_shell: Shell, tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    monkeypatch.setattr("mnt.pyfiction.cli.drawing.shutil.which", lambda _: None)
    destination = tmp_path / "drawing.svg"
    destination.write_text("original", encoding="utf-8")
    output = mux21_shell.fails(f'show -n --silent -o "{destination}"')
    assert "Graphviz" in output
    assert "DOT retained" in output
    assert destination.read_text(encoding="utf-8") == "original"
    assert "digraph" in destination.with_suffix(".dot").read_text(encoding="utf-8")


def test_installed_console_script() -> None:
    binary = Path(sys.executable).parent / ("fiction.exe" if os.name == "nt" else "fiction")
    assert binary.is_file(), "install the wheel before running the console-script contract"
    result = subprocess.run(  # ruff: ignore[subprocess-without-shell-equals-true] -- installed console script
        [str(binary), "--quiet", "-c", "version"], capture_output=True, text=True, timeout=20, check=False
    )
    assert result.returncode == 0, result.stderr
    assert "compiled" in result.stdout


@pytest.mark.parametrize("flags", ["-g", "-c"])
def test_redirected_drawings_have_no_ansi(mux21_shell: Shell, flags: str) -> None:
    mux21_shell.ok("ortho; cell")
    output = mux21_shell.ok(f"print {flags}")
    assert "\x1b" not in output


def test_completion_respects_path_context(tmp_path: Path) -> None:
    directory = tmp_path / "with spaces"
    directory.mkdir()
    (directory / "circuit.v").write_text("", encoding="utf-8")
    event = CompleteEvent(completion_requested=True)
    candidates = list(CommandCompleter().get_completions(Document(f'read "{directory}/circ'), event))
    assert any("uit.v" in candidate.text for candidate in candidates)
    assert not list(CommandCompleter().get_completions(Document(f"tt {directory}/circ"), event))


@pytest.mark.parametrize("library", ["qca-one", "sim7-mol", "bestagon", "topolinano"])
def test_complete_design_and_export_workflows(shell: Shell, tmp_path: Path, library: str) -> None:
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
        "qca-one": ("qca", "fqca", "qll", "svg"),
        "sim7-mol": ("qll", "svg"),
        "bestagon": ("sqd", "svg"),
        "topolinano": ("qcc", "qll"),
    }[library]
    for suffix in formats:
        destination = tmp_path / f"exported.{suffix}"
        options = " --component-name" if suffix == "qcc" else ""
        shell.ok(f'write -c "{destination}"{options}')
        text = destination.read_text(encoding="utf-8")
        if suffix == "fqca":
            restored = fiction.read_stacked_fqca_layout(str(destination))
            assert restored.num_cells() == entry.layout.num_cells()
            assert restored.num_pis() == entry.layout.num_pis()
            assert restored.num_pos() == entry.layout.num_pos()
        elif suffix == "sqd":
            restored = fiction.read_sqd_layout(str(destination))
            assert restored.num_dots() == entry.layout.num_dots()
            assert restored.num_pis() == entry.layout.num_pis()
            assert restored.num_pos() == entry.layout.num_pos()
        elif suffix == "qca":
            assert text.count("[TYPE:QCADCell]") >= entry.layout.num_cells()
        else:
            root = ET.fromstring(text)  # ruff: ignore[suspicious-xml-element-tree-usage] -- writer output
            assert (
                root.tag.endswith("svg")
                if suffix == "svg"
                else root.tag == {"qll": "qcalayout", "qcc": "qcacomponent"}[suffix]
            )
            if suffix == "qll":
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


def test_gate_library_error_preserves_store(shell: Shell) -> None:
    layout = fiction.shifted_cartesian_gate_layout((1, 1), "2DDWave", "unsupported routing")
    source = layout.create_pi("a", (0, 0))
    layout.create_po(source, "f", (0, 1))
    shell.session.gate_layouts.add(layout)
    output = shell.fails("cell -l topolinano")
    assert "unsupported gate orientation at tile" in output
    assert len(shell.session.cell_layouts) == 0


def test_directory_import_ignores_directories_with_network_suffix(shell: Shell, tmp_path: Path) -> None:
    (tmp_path / "folder.v").mkdir()
    (tmp_path / "wire.V").write_text(
        "module top(a,f);\ninput a;\noutput f;\nassign f = a;\nendmodule\n", encoding="utf-8"
    )
    shell.ok(f'read "{tmp_path}"')
    assert len(shell.session.networks) == 1
    assert "failed" not in shell.stderr


def test_interrupted_command_is_logged_and_session_continues(shell: Shell, monkeypatch: pytest.MonkeyPatch) -> None:
    def interrupt(*_: object, **__: object) -> None:
        raise KeyboardInterrupt

    monkeypatch.setattr("mnt.pyfiction.cli.commands.logic.simulate_outputs", interrupt)
    shell.ok("generate rca -b 1")
    assert "interrupted" in shell.fails("simulate -n")
    assert shell.session.log[-1]["status"] == "interrupted"
    shell.ok("version")


@pytest.mark.parametrize("topology", [name for name in FGL_READERS if name not in {"shifted_cartesian", "hexagonal"}])
def test_exact_topologies_preserve_function(shell: Shell, tmp_path: Path, topology: str) -> None:
    source = tmp_path / "wire.v"
    source.write_text("module top(a,f);\ninput a;\noutput f;\nassign f = a;\nendmodule\n", encoding="utf-8")
    shell.ok(f'read "{source}"; exact --topology {topology} --timeout 5')
    assert fiction.simulate_outputs(shell.session.gate_layouts.current()) == fiction.simulate_outputs(
        shell.session.networks.current()
    )


@pytest.mark.parametrize(("kind", "gate"), [("xag", "num_xor2"), ("mig", "num_maj3")])
def test_random_uses_native_gate_sets(shell: Shell, kind: str, gate: str) -> None:
    shell.ok(f"random --type {kind} -n 4 -g 30 --seed 17")
    network = shell.session.networks.current()
    assert network.num_pis() == 4
    assert getattr(fiction.count_gate_types(network), gate) > 0


def test_sidb_statistics_use_lattice_area(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f'read "{resource("siqad_or_gate.sqd")}"; ps -c')
    description = shell.session.log[-1]["result"]
    assert isinstance(description, dict)
    assert isinstance(description["cell_layout"], dict)
    assert description["cell_layout"]["area_nm2"] == fiction.area(shell.session.cell_layouts.current().layout)
    assert "Area (nm²)" in shell.output
