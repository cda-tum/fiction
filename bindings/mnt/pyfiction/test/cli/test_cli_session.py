# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of the dispatcher: tokenizing, running, failing, and describing statistics."""

from __future__ import annotations

import io
from typing import TYPE_CHECKING

import pytest
from rich.console import Console

from mnt.pyfiction import orthogonal, orthogonal_stats
from mnt.pyfiction.cli import Session
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import REGISTRY, STORE_FLAGS, Category
from mnt.pyfiction.cli.session import ignore_progress, stats_to_dict, tokenize
from mnt.pyfiction.cli.stores import Store

if TYPE_CHECKING:
    from collections.abc import Callable

    from mnt.pyfiction import technology_network

    from .conftest import Shell


@pytest.mark.parametrize(
    ("line", "expected"),
    [
        ("version", [["version"]]),
        ("read a.v; ortho ; cell", [["read", "a.v"], ["ortho"], ["cell"]]),
        ("read 'my file.v';;", [["read", "my file.v"]]),
        ('tt -e "[(ab)(!ac)]" # comment; version', [["tt", "-e", "[(ab)(!ac)]"]]),
        ("   ", []),
        ("write out.v;ortho", [["write", "out.v"], ["ortho"]]),
        ("read C:\\Users\\me\\mux21.v", [["read", "C:\\Users\\me\\mux21.v"]]),
        ('read "C:\\my dir\\a.v"', [["read", "C:\\my dir\\a.v"]]),
    ],
)
def test_tokenize(line: str, expected: list[list[str]]) -> None:
    assert tokenize(line) == expected


def test_unknown_command_fails(shell: Shell) -> None:
    output = shell.fails("frobnicate")
    assert "unknown command 'frobnicate'" in output


def test_unclosed_quote_fails(shell: Shell) -> None:
    assert not shell.run('read "a.v')


def test_usage_error_does_not_raise(shell: Shell) -> None:
    output = shell.fails("read")
    assert "usage: read" in output


def test_help_flag_prints_help(shell: Shell) -> None:
    output = shell.ok("ortho -h")
    assert "usage: ortho" in output
    assert "--verbose" in output


def test_line_stops_at_first_failure(shell: Shell) -> None:
    assert not shell.run("version; frobnicate; version")
    assert shell.output.count("compiled") == 1


def test_empty_store_is_an_error(shell: Shell) -> None:
    assert "no network in store" in shell.fails("ortho")


def test_command_error_keeps_the_shell_running(shell: Shell) -> None:
    shell.fails("ortho")
    assert shell.session.running
    shell.ok("version")


def test_every_command_has_a_category_and_summary() -> None:
    for name, cmd in REGISTRY.items():
        assert name == cmd.name or name in cmd.aliases
        assert isinstance(cmd.category, Category)
        assert cmd.summary
        assert "-h" in cmd.options


def test_store_flag_letters_are_reserved() -> None:
    """On a command that selects a store, -t, -n, -g, and -c mean the store and nothing else.

    A command that selects no store is free to spend the letters, as 'tt -t' and 'exact -c' do.
    """
    selects_a_store = [
        cmd
        for cmd in dict.fromkeys(REGISTRY.values())
        if any(long in cmd.options for _, long, _ in STORE_FLAGS.values())
    ]
    assert selects_a_store, "the registry lost every store-selecting command"
    for cmd in selects_a_store:
        for short, long, _ in STORE_FLAGS.values():
            if short in cmd.options:
                assert long in cmd.options, f"'{cmd.name}' uses {short} for something other than a store"


def test_store() -> None:
    store: Store[int] = Store("thing")
    with pytest.raises(CommandError, match="no thing in store"):
        store.current()
    store.add(1)
    store.add(2)
    assert store.current() == 2
    store.select(0)
    assert store.current() == 1
    with pytest.raises(CommandError, match="out of range"):
        store.select(5)
    assert store.summary() == "things: 2"
    store.clear()
    assert len(store) == 0
    assert store.summary() == "things: 0"


def test_status_line_describes_active_store_elements(mux21_shell: Shell) -> None:
    mux21_shell.ok("ortho; cell; tt -t 1000")
    status = mux21_shell.session.status_line()
    assert "net[0]/1 mux21" in status
    assert "gate[0]/1" in status
    assert "cell[0]/1" in status
    assert "tt[0]/1" in status


def test_stats_to_dict(mux21: technology_network) -> None:
    stats = orthogonal_stats()
    orthogonal(mux21, statistics=stats)
    result = stats_to_dict(stats)
    assert isinstance(result["time_total_s"], float)
    assert isinstance(result["num_gates"], int)
    assert result["num_gates"] > 0
    assert "report" not in result


def test_script_depth_limit(shell: Shell, tmp_path_factory: pytest.TempPathFactory) -> None:
    script = tmp_path_factory.mktemp("scripts") / "loop.fs"
    script.write_text(f"source {script}\n", encoding="utf-8")
    output = shell.fails(f"source {script}")
    assert "deeper than" in output


def test_progress_shows_on_terminal(resource: Callable[[str], str], monkeypatch: pytest.MonkeyPatch) -> None:
    """On a terminal, a running command shows its spinner and the tasks its algorithm reports."""
    monkeypatch.setenv("TERM", "xterm")
    buffer = io.StringIO()
    console = Console(file=buffer, width=100, force_terminal=True, color_system=None)
    session = Session(console=console)
    try:
        assert session.execute(f"read {resource('mux21.v')}; ortho")
    finally:
        session.close()
    assert "ortho" in buffer.getvalue()
    assert "placing gates" in buffer.getvalue()
    assert session.report_progress is ignore_progress


def test_progress_is_silent_without_terminal(mux21_shell: Shell) -> None:
    """Without a terminal, the progress display writes nothing."""
    assert not mux21_shell.ok("ortho")


def test_progress_resets_a_restarted_task(monkeypatch: pytest.MonkeyPatch) -> None:
    """A task whose count drops is shown from the start again instead of counting backwards."""
    monkeypatch.setenv("TERM", "xterm")
    buffer = io.StringIO()
    session = Session(console=Console(file=buffer, width=100, force_terminal=True, color_system=None))
    with session.progress("optimize") as report:
        report("gate relocations", 0, 4)
        report("gate relocations", 4, 4)
        report("gate relocations", 0, 3)
        report("gate relocations", 3, 3)
        report("wire paths", 7, 0)
    session.close()
    assert "gate relocations" in buffer.getvalue()
    assert "wire paths" in buffer.getvalue()


def test_progress_reports_are_dropped_without_terminal(shell: Shell) -> None:
    """Without a terminal, the callback handed to the algorithms discards the reports."""
    with shell.session.progress("optimize") as report:
        report("gate relocations", 0, 4)
        report("gate relocations", 4, 4)
    assert not shell.output
