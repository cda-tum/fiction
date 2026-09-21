# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of the JSON statistics log."""

from __future__ import annotations

import io
import json
from pathlib import Path
from typing import TYPE_CHECKING

from rich.console import Console

from mnt.fiction.cli.session import Session
from mnt.fiction.cli.statistics import json_value
from mnt.pyfiction import (
    area,
)

if TYPE_CHECKING:
    from collections.abc import Callable

    from .conftest import Shell


def test_one_entry_per_command(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.run(f'read "{resource("mux21.v")}"; ortho; frobnicate')
    shell.session.close()
    assert shell.session.log_path is not None
    entries = json.loads(shell.session.log_path.read_text(encoding="utf-8"))
    assert [entry["command"] for entry in entries] == ["read", "ortho", "frobnicate"]
    assert [entry["status"] for entry in entries] == ["ok", "ok", "error"]
    for entry in entries[:2]:
        assert set(entry) >= {"command", "args", "started", "runtime_s", "status", "result"}
        assert entry["runtime_s"] >= 0


def test_failed_command_is_logged_with_its_error(shell: Shell) -> None:
    shell.fails("ortho")
    shell.fails("read")
    shell.session.close()
    assert shell.session.log_path is not None
    entries = json.loads(shell.session.log_path.read_text(encoding="utf-8"))
    assert [entry["status"] for entry in entries] == ["error", "error"]
    assert "no network in store" in entries[0]["error"]
    assert "result" not in entries[0]


def test_store_schemas(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f'read "{resource("mux21.v")}"; tt -t 1110; ortho; cell')
    log = shell.session.log
    network = log[0]["result"]["network"]  # type: ignore[index]
    assert set(network) >= {"name", "type", "inputs", "outputs", "gates", "depth"}
    assert network["type"] == "TEC"
    truth_table = log[1]["result"]["truth_table"]  # type: ignore[index]
    assert truth_table == {"vars": 2, "hex": "e", "binary": "1110"}
    layout = log[2]["result"]["gate_layout"]  # type: ignore[index]
    assert set(layout) >= {
        "name",
        "topology",
        "clocking",
        "size",
        "inputs",
        "outputs",
        "gates",
        "wires",
        "crossings",
        "synchronization_elements",
    }
    assert layout["clocking"] == "2DDWAVE"
    assert "throughput" not in layout
    shell.ok("ps -g")
    timing = log[-1]["result"]["gate_layout"]  # type: ignore[index]
    assert isinstance(timing["throughput"], int)
    assert isinstance(timing["critical_path"], int)
    assert set(layout["size"]) >= {"x", "y", "area"}
    stats = log[2]["result"]["stats"]  # type: ignore[index]
    assert isinstance(stats["time_total_s"], float)
    cell = log[3]["result"]["cell_layout"]  # type: ignore[index]
    assert cell["technology"] == "QCA"
    assert set(cell) >= {"name", "technology", "size", "inputs", "outputs", "cells", "area_nm2"}


def test_help_is_logged(shell: Shell) -> None:
    shell.ok("ortho -h")
    assert shell.session.log[-1]["status"] == "help"


def test_sidb_statistics_use_dots(shell: Shell, resource: Callable[[str], str]) -> None:
    """SiDB store output and JSON describe dots under the shared cell-layout store."""
    shell.ok(f'read "{resource("siqad_or_gate.sqd")}"')
    description = shell.session.log[-1]["result"]["cell_layout"]  # type: ignore[index]
    assert description["dots"] == shell.session.cell_layouts.current().layout.num_dots()
    assert "cells" not in description
    # the store listing shows the layout extent; the dot count is in the ps block
    assert "17 x 18" in shell.ok("store -c")
    assert "dots" in shell.ok("ps -c")


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


def test_sidb_statistics_use_lattice_area(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f'read "{resource("siqad_or_gate.sqd")}"; ps -c')
    description = shell.session.log[-1]["result"]
    assert isinstance(description, dict)
    assert isinstance(description["cell_layout"], dict)
    assert description["cell_layout"]["area_nm2"] == area(shell.session.cell_layouts.current().layout)
    assert "Area (nm²)" in shell.output
