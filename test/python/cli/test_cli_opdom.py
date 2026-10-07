# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Operational domain orchestration through the dedicated CLI and shell."""

from __future__ import annotations

import builtins
import importlib
import json
import shutil
import subprocess  # ruff: ignore[suspicious-subprocess-import] -- installed CLI tests use argument lists
import sys
import sysconfig
from pathlib import Path
from typing import TYPE_CHECKING

import pytest

from mnt.fiction.cli.stores import CellEntry
from mnt.pyfiction.sidb import lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.model import sidb_defect, sidb_defect_type

if TYPE_CHECKING:
    from collections.abc import Callable
    from typing import Any

    from .conftest import Shell

SMALL_SWEEP = [
    "--x-min",
    "5.6",
    "--x-max",
    "5.7",
    "--x-step",
    "0.1",
    "--y-min",
    "5",
    "--y-max",
    "5.1",
    "--y-step",
    "0.1",
]
"""A four-point grid for bounded integration tests."""


@pytest.mark.parametrize("quiet", [False, True])
def test_opdom_entry_point_csv(
    resource: Callable[[str], str], tmp_path: Path, capsys: pytest.CaptureFixture[str], *, quiet: bool
) -> None:
    """The dedicated command writes the same four-point domain as the shell."""
    main = importlib.import_module("mnt.fiction.opdom").main
    csv = tmp_path / "domain.csv"
    log = tmp_path / "run.json"
    assert (
        main([
            resource("hex_21_inputsdbp_xor_v1.sqd"),
            "--gate",
            "xor",
            "--csv",
            str(csv),
            "--log",
            str(log),
            *(["--quiet"] if quiet else []),
            *SMALL_SWEEP,
        ])
        == 0
    )
    assert len(csv.read_text(encoding="utf-8").splitlines()) == 5
    result = json.loads(log.read_text(encoding="utf-8"))[0]["result"]
    assert result["num_evaluated_parameter_combinations"] == 4
    assert result["num_simulator_invocations"] > 0
    output = capsys.readouterr().out
    assert "Simulator calls" in output
    assert ("wrote" in output) is not quiet


def test_opdom_explicit_specification_overrides_active_table(xor_gate: Shell, tmp_path: Path) -> None:
    """Explicit specifications work inside the fiction shell too."""
    xor_gate.ok("tt -t 1000")
    path = tmp_path / "domain.csv"
    xor_gate.ok(f'opdom "{path}" --table 0110 ' + " ".join(SMALL_SWEEP))
    assert len(path.read_text(encoding="utf-8").splitlines()) == 5


@pytest.mark.parametrize(
    "options",
    [
        "--threads 0",
        "--timeout-ms -1",
        "--timeout-ms 18446744073709551616",
        "--x-min 0",
        "--expression c",
        "--show",
        "--bdl-pair-min 2 --bdl-pair-max 1",
        "--sketch --condition tolerate_kinks",
        "--sketch --strategy simulation_only",
        "--engine quicksim --base 3",
        "--table 01",
        "--table 0110 --table 0110",
    ],
)
def test_opdom_preflight_rejects_invalid_configuration(xor_gate: Shell, tmp_path: Path, options: str) -> None:
    """Invalid domain parameters and port mismatches create no output."""
    path = tmp_path / "domain.csv"
    xor_gate.fails(f'opdom "{path}" ' + " ".join(SMALL_SWEEP) + f" {options}")
    assert not path.exists()


@pytest.mark.parametrize("timeout", [10000, 2**32, 2**64 - 1])
def test_opdom_reports_native_parameters(xor_gate: Shell, tmp_path: Path, timeout: int) -> None:
    """Advanced options reach native parameters and are recorded in the log."""
    xor_gate.ok(
        f'opdom "{tmp_path / "domain.csv"}" --threads 1 --timeout-ms {timeout} '
        "--condition reject_kinks --strategy filter_then_simulation --input-encoding absence "
        "--bdl-pair-min 0.7 --bdl-pair-max 1.6 --bdl-wire-distance 2.1 " + " ".join(SMALL_SWEEP)
    )
    result = xor_gate.session.log[-1]["result"]
    assert result["threads"] == 1
    operational = result["operational_parameters"]
    assert operational["timeout"] == timeout
    assert operational["op_condition"] == "REJECT_KINKS"
    assert operational["strategy_to_analyze_operational_status"] == "FILTER_THEN_SIMULATION"
    inputs = operational["input_bdl_iterator_params"]
    assert inputs["input_bdl_config"] == "PERTURBER_ABSENCE_ENCODED"
    assert inputs["bdl_wire_params"]["threshold_bdl_interdistance"] == 2.1
    assert inputs["bdl_wire_params"]["bdl_pairs_params"]["minimum_distance"] == 0.7
    assert "Simulator calls" in xor_gate.output


@pytest.mark.parametrize("case", ["sketch_canvas", "charged_defect", "missing_ports"])
def test_layout_prerequisites(xor_gate: Shell, tmp_path: Path, case: str) -> None:
    """Unsupported layout features fail before producing an output."""
    layout = xor_gate.session.cell_layouts.current().layout
    assert isinstance(layout, sidb_layout)
    if case == "sketch_canvas":
        for site in layout.dots_with_tag(sidb_dot_tag.LOGIC):
            layout.assign_sidb(site, sidb_dot_tag.NORMAL)
        flags, message = "--sketch", "LOGIC dots"
    elif case == "charged_defect":
        layout.assign_defect(lattice_site(100, 100, 0), sidb_defect(sidb_defect_type.SI_VACANCY, -1))
        flags, message = "--engine quicksim", "charged defects"
    else:
        layout = sidb_layout()
        layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
        xor_gate.session.cell_layouts.add(CellEntry(layout))
        flags, message = "", "BDL input and output ports"
    path = tmp_path / "domain.csv"
    assert message in xor_gate.fails(f'opdom "{path}" {flags} ' + " ".join(SMALL_SWEEP))
    assert not path.exists()


def test_too_many_bdl_inputs(shell: Shell, wire_with_canvas: sidb_layout, tmp_path: Path) -> None:
    """Input counts beyond the native mask width fail before allocating truth tables."""
    layout = sidb_layout()
    for offset in range(0, 6400, 100):
        for site in wire_with_canvas.sidbs():
            layout.assign_sidb(lattice_site(site.x + offset, site.y, site.z), wire_with_canvas.get_dot_tag(site))
    shell.session.cell_layouts.add(CellEntry(layout))
    path = tmp_path / "domain.csv"
    assert "at most 63" in shell.fails(f'opdom "{path}" --gate id')
    assert not path.exists()


def test_without_plot_extra(resource: Callable[[str], str], tmp_path: Path) -> None:
    """Help and CSV computation do not import either optional plotting backend."""

    script = """
import importlib.abc
import sys
class NoPlots(importlib.abc.MetaPathFinder):
    def find_spec(self, fullname, path, target=None):
        if fullname.split(".")[0] in {"matplotlib", "plotly"}:
            raise ModuleNotFoundError(fullname)
sys.meta_path.insert(0, NoPlots())
from mnt.fiction.opdom import main
assert main(["--help"]) == 0
assert main(sys.argv[1:]) == 0
assert "matplotlib" not in sys.modules
assert "plotly" not in sys.modules
"""
    csv = tmp_path / "base.csv"
    result = subprocess.run(  # ruff: ignore[subprocess-without-shell-equals-true] -- test executables run without a shell
        [
            sys.executable,
            "-c",
            script,
            resource("hex_21_inputsdbp_xor_v1.sqd"),
            "--gate",
            "xor",
            "--csv",
            str(csv),
            *SMALL_SWEEP,
        ],
        capture_output=True,
        text=True,
        check=False,
    )
    assert result.returncode == 0, result.stderr
    assert len(csv.read_text(encoding="utf-8").splitlines()) == 5


@pytest.mark.parametrize("method", ["--grid-search", "--random-sampling 4", "--flood-fill 4", "--contour-tracing 4"])
@pytest.mark.parametrize("dimensions", [2, 3])
def test_methods_and_sketch(
    shell: Shell, wire_with_canvas: sidb_layout, tmp_path: Path, method: str, dimensions: int
) -> None:
    """Every method accepts 2D and 3D Sketch computations."""

    shell.session.cell_layouts.add(CellEntry(wire_with_canvas))
    path = tmp_path / "sketch.csv"
    z = "--z-sweep mu_minus --z-min -0.32 --z-max -0.32" if dimensions == 3 else ""
    shell.ok(f'opdom "{path}" --gate id --sketch {method} {z} ' + " ".join(SMALL_SWEEP))
    result = shell.session.log[-1]["result"]
    assert result["num_evaluated_parameter_combinations"] > 0
    assert result["num_simulator_invocations"] == 0
    assert len(path.read_text(encoding="utf-8").splitlines()[0].split(",")) == dimensions + 1


def test_multi_output_expressions_with_unused_inputs(
    shell: Shell, wire_with_canvas: sidb_layout, tmp_path: Path
) -> None:
    """Expressions for separate outputs share the detected input width."""

    layout = sidb_layout()
    for offset in (0, 100):
        for site in wire_with_canvas.sidbs():
            layout.assign_sidb(lattice_site(site.x + offset, site.y, site.z), wire_with_canvas.get_dot_tag(site))
    shell.session.cell_layouts.add(CellEntry(layout))
    path = tmp_path / "multi.csv"
    shell.ok(f'opdom "{path}" --expression a --expression b --x-min 5.6 --x-max 5.6 --y-min 5 --y-max 5 --threads 1')
    assert len(path.read_text(encoding="utf-8").splitlines()) == 2
    assert shell.session.log[-1]["result"]["num_simulator_invocations"] > 0


@pytest.mark.parametrize("options", [["--timeout-ms", "0"], ["--engine", "clustercomplete", "--timeout-ms", "10"]])
def test_timeout_failures(resource: Callable[[str], str], tmp_path: Path, options: list[str]) -> None:
    """Expired or unsupported deadlines report failure without writing a partial domain."""
    main = importlib.import_module("mnt.fiction.opdom").main
    csv = tmp_path / "domain.csv"
    assert (
        main([resource("hex_21_inputsdbp_xor_v1.sqd"), "--gate", "xor", "--csv", str(csv), *options, *SMALL_SWEEP]) != 0
    )
    assert not csv.exists()


def test_missing_plot_dependency_is_reported_before_computation(
    xor_gate: Shell, tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    """Unavailable plotting packages do not leave an expensive computation's CSV behind."""

    original_import = builtins.__import__

    def without_plotly(name: str, *args: Any, **kwargs: Any) -> object:
        if name.startswith("plotly"):
            raise ModuleNotFoundError(name)
        return original_import(name, *args, **kwargs)

    monkeypatch.setattr(builtins, "__import__", without_plotly)
    csv = tmp_path / "domain.csv"
    assert "mnt-pyfiction[plot]" in xor_gate.fails(f'opdom "{csv}" --plot "{tmp_path / "plot.html"}"')
    assert not csv.exists()


def test_installed_console_command(resource: Callable[[str], str], tmp_path: Path) -> None:
    """The wheel's installed console command runs a real computation."""

    executable = shutil.which("fiction-opdom", path=sysconfig.get_path("scripts"))
    assert executable is not None
    for command in ([executable], [sys.executable, "-m", "mnt.fiction.opdom"]):
        help_result = subprocess.run(  # ruff: ignore[subprocess-without-shell-equals-true] -- test executables run without a shell
            [*command, "--help"], capture_output=True, text=True, check=False
        )
        assert help_result.returncode == 0, help_result.stderr
        assert "--plot" in help_result.stdout
    csv = tmp_path / "installed.csv"
    result = subprocess.run(  # ruff: ignore[subprocess-without-shell-equals-true] -- test executables run without a shell
        [executable, resource("hex_21_inputsdbp_xor_v1.sqd"), "--gate", "xor", "--csv", str(csv), *SMALL_SWEEP],
        capture_output=True,
        text=True,
        check=False,
    )
    assert result.returncode == 0, result.stderr
    assert "Simulator calls" in result.stdout
    assert len(csv.read_text(encoding="utf-8").splitlines()) == 5


@pytest.mark.parametrize("output_flag", ["--csv", "--plot", "--log"])
def test_input_is_not_an_output(resource: Callable[[str], str], tmp_path: Path, output_flag: str) -> None:
    """A colliding output or log path cannot overwrite the input SQD file."""
    main = importlib.import_module("mnt.fiction.opdom").main
    path = tmp_path / "input.sqd"
    source = Path(resource("hex_21_inputsdbp_xor_v1.sqd")).read_bytes()
    path.write_bytes(source)
    assert main([str(path), "--gate", "xor", output_flag, str(path), *SMALL_SWEEP]) != 0
    assert path.read_bytes() == source


def test_shell_log_is_not_a_plot(xor_gate: Shell, tmp_path: Path) -> None:
    """The shell rejects plots that would be replaced by its statistics log."""
    plot = tmp_path / "domain.html"
    xor_gate.session.log_path = plot
    csv = tmp_path / "domain.csv"
    assert "distinct" in xor_gate.fails(f'opdom "{csv}" --plot "{plot}" ' + " ".join(SMALL_SWEEP))
    assert not csv.exists()
    assert not plot.exists()


def test_input_hard_link_is_not_a_log(resource: Callable[[str], str], tmp_path: Path) -> None:
    """A log hard link cannot truncate the input SQD file."""
    main = importlib.import_module("mnt.fiction.opdom").main
    path = tmp_path / "input.sqd"
    source = Path(resource("hex_21_inputsdbp_xor_v1.sqd")).read_bytes()
    path.write_bytes(source)
    log = tmp_path / "alias.json"
    log.hardlink_to(path)
    csv = tmp_path / "domain.csv"
    assert main([str(path), "--gate", "xor", "--log", str(log), "--csv", str(csv), *SMALL_SWEEP]) != 0
    assert path.read_bytes() == source
    assert not csv.exists()
