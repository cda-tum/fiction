# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of the JSON statistics log."""

from __future__ import annotations

import json
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    from collections.abc import Callable

    from .conftest import Shell


def test_one_entry_per_command(make_shell: Callable[[], Shell], resource: Callable[[str], str]) -> None:
    shell = make_shell()
    shell.run(f"read {resource('mux21.v')}; ortho; frobnicate")
    shell.session.close()
    assert shell.session.log_path is not None
    entries = json.loads(shell.session.log_path.read_text(encoding="utf-8"))
    assert [entry["command"] for entry in entries] == ["read", "ortho", "frobnicate"]
    assert [entry["status"] for entry in entries] == ["ok", "ok", "error"]
    for entry in entries[:2]:
        assert set(entry) >= {"command", "args", "started", "runtime_s", "status", "result"}
        assert entry["runtime_s"] >= 0


def test_failed_command_is_logged_with_its_error(make_shell: Callable[[], Shell]) -> None:
    shell = make_shell()
    shell.fails("ortho")
    shell.fails("read")
    shell.session.close()
    assert shell.session.log_path is not None
    entries = json.loads(shell.session.log_path.read_text(encoding="utf-8"))
    assert [entry["status"] for entry in entries] == ["error", "error"]
    assert "no network in store" in entries[0]["error"]
    assert "result" not in entries[0]


def test_store_schemas(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f"read {resource('mux21.v')}; tt -t 1110; ortho; cell")
    log = shell.session.log
    network = log[0]["result"]["network"]  # type: ignore[index]
    assert set(network) == {"name", "type", "inputs", "outputs", "gates", "depth"}
    assert network["type"] == "TEC"
    truth_table = log[1]["result"]["truth_table"]  # type: ignore[index]
    assert truth_table == {"vars": 2, "hex": "e", "binary": "1110"}
    layout = log[2]["result"]["gate_layout"]  # type: ignore[index]
    assert set(layout) == {
        "name",
        "topology",
        "clocking",
        "size",
        "inputs",
        "outputs",
        "gates",
        "wires",
        "crossings",
        "critical_path",
        "throughput",
        "synchronization_elements",
    }
    assert layout["clocking"] == "2DDWAVE"
    assert isinstance(layout["throughput"], int)
    assert set(layout["size"]) == {"x", "y", "area"}
    stats = log[2]["result"]["stats"]  # type: ignore[index]
    assert isinstance(stats["time_total_s"], float)
    cell = log[3]["result"]["cell_layout"]  # type: ignore[index]
    assert cell["technology"] == "QCA"
    assert set(cell) == {"name", "technology", "size", "inputs", "outputs", "cells", "area_nm2"}


def test_help_is_logged(shell: Shell) -> None:
    shell.ok("ortho -h")
    assert shell.session.log[-1]["status"] == "help"


def test_log_survives_values_json_cannot_encode(make_shell: Callable[[], Shell]) -> None:
    """A value no encoder handles is written as its string form instead of failing the whole log."""
    shell = make_shell()
    shell.session.log.append({"command": "x", "result": {"value": object()}})
    shell.session.close()
    assert shell.session.log_path is not None
    entries = json.loads(shell.session.log_path.read_text(encoding="utf-8"))
    assert entries[-1]["result"]["value"].startswith("<object object")


def test_sidb_statistics_use_dots(shell: Shell, resource: Callable[[str], str]) -> None:
    """SiDB store output and JSON describe dots under the shared cell-layout store."""
    shell.ok(f"read {resource('siqad_or_gate.sqd')}")
    description = shell.session.log[-1]["result"]["cell_layout"]  # type: ignore[index]
    assert description["dots"] == shell.session.cell_layouts.current().layout.num_dots()
    assert "cells" not in description
    assert "dots" in shell.ok("store -c")
    assert "Dots" in shell.ok("ps -c")
