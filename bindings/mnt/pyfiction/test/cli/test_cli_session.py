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
import threading
import time
from concurrent.futures import ThreadPoolExecutor
from typing import TYPE_CHECKING

import pytest
from rich.cells import cell_len
from rich.console import Console
from rich.progress import Progress

from mnt.fiction.cli.errors import CommandError
from mnt.fiction.cli.parsing import tokenize
from mnt.fiction.cli.registry import REGISTRY, STORE_FLAGS, Category
from mnt.fiction.cli.session import Session, ignore_progress
from mnt.fiction.cli.statistics import stats_to_dict
from mnt.fiction.cli.stores import Store
from mnt.pyfiction import orthogonal, orthogonal_stats, set_name

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path

    from rich.progress import TaskID

    from mnt.pyfiction import technology_network

    from .conftest import Shell


@pytest.mark.parametrize(
    ("line", "expected"),
    [
        ("version", [["version"]]),
        ('read ";"; help # comment', [["read", ";"], ["help"]]),
        ("read a.v; ortho ; cell", [["read", "a.v"], ["ortho"], ["cell"]]),
        ("read 'my file.v';;", [["read", "my file.v"]]),
        ('tt -e "[(ab)(!ac)]" # comment; version', [["tt", "-e", "[(ab)(!ac)]"]]),
        ("   ", []),
        ("write_verilog out.v;ortho", [["write_verilog", "out.v"], ["ortho"]]),
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
        assert name == cmd.name
        assert isinstance(cmd.category, Category)
        assert cmd.summary
        assert "-h" in cmd.options


def test_store_flag_letters_are_reserved() -> None:
    """On a command that selects a store, -t, -n, -g, and -c mean the store and nothing else.

    A command that selects no store is free to spend the letters, as 'tt -t' and 'exact -c' do.
    """
    selects_a_store = [
        cmd for cmd in REGISTRY.values() if any(long in cmd.options for _, long, _ in STORE_FLAGS.values())
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
    store.select(1)
    assert store.current() == 1
    with pytest.raises(CommandError, match="out of range"):
        store.select(5)
    with pytest.raises(CommandError, match="out of range"):
        store.select(0)
    store.clear()
    assert len(store) == 0


def test_status_line_describes_active_store_elements(mux21_shell: Shell) -> None:
    mux21_shell.ok("ortho; cell; tt -t 1000")
    status = mux21_shell.session.status_line()
    # positions count from 1, the way 'store' lists them and 'current' accepts them
    assert "networks 1 of 1 · mux21" in status
    assert "gate layouts 1 of 1" in status
    assert "cell layouts 1 of 1" in status
    assert "truth tables 1 of 1" in status


def test_status_line_is_empty_without_elements() -> None:
    assert Session().status_line() == "no elements in store"


@pytest.mark.parametrize("width", [1, 18, 20, 40, 80, 120])
def test_status_line_fits_terminal(shell: Shell, width: int) -> None:
    shell.session.console.width = width
    assert cell_len(shell.session.status_line()) <= width
    shell.ok("generate mux -b 1")
    set_name(shell.session.networks.current(), "长名称 e\u0301\n" * 10)
    status = shell.session.status_line()
    assert "\n" not in status
    assert cell_len(status) <= width
    shell.ok("ortho; cell; tt -t 1000")
    assert cell_len(shell.session.status_line()) <= width


def test_stats_to_dict(mux21: technology_network) -> None:
    stats = orthogonal_stats()
    orthogonal(mux21, statistics=stats)
    result = stats_to_dict(stats)
    assert isinstance(result["time_total_s"], float)
    assert isinstance(result["num_gates"], int)
    assert result["num_gates"] > 0
    assert "report" not in result


def test_pop_selects_predecessor() -> None:
    store: Store[int] = Store("number")
    for value in range(4):
        store.add(value)
    # positions count from 1, so position 3 holds the value 2
    store.select(3)
    removed = store.pop()
    assert removed == 2
    assert store.current() == 1
    store.select(1)
    store.pop()
    assert store.current() == 1


def test_decoding_failure_reports_file_and_line(shell: Shell, tmp_path: Path) -> None:
    path = tmp_path / "bad.fiction"
    path.write_bytes(b"version\n\xff")
    with pytest.raises(CommandError) as failure:
        shell.session.run_script(path)
    output = str(failure.value)
    assert f"{path}:2" in output
    assert "UTF-8" in output


def test_interrupted_command_is_logged_and_session_continues(shell: Shell, monkeypatch: pytest.MonkeyPatch) -> None:
    def interrupt(*_: object, **__: object) -> None:
        raise KeyboardInterrupt

    monkeypatch.setattr("mnt.fiction.cli.commands.logic.simulate.simulate_outputs", interrupt)
    shell.ok("generate rca -b 1")
    assert "interrupted" in shell.fails("simulate -n")
    assert shell.session.log[-1]["status"] == "interrupted"
    shell.ok("version")


@pytest.mark.parametrize(
    ("command", "message"),
    [
        ("tt -r 64", "fewer than 38"),
        ("random -n -1 -g 2", "at least 1"),
        ("random -n 0 -g 2", "at least 1"),
        ("random -n 2 -g 4294967296", "at most 4294967295"),
        ("random -n 2 -g 1 --seed 18446744073709551616", "at most 18446744073709551615"),
        ("exact --timeout nan", "finite number"),
        ("area --width -1", "cannot be negative"),
        ("quickexact --epsilon-r nan", "finite number"),
        ("quickexact --epsilon-r inf", "finite number"),
        ("quickexact --epsilon-r 0", "must be positive"),
        ("quicksim --alpha 2", "in (0, 1]"),
    ],
)
def test_invalid_numeric_input_is_a_command_failure(
    shell: Shell, resource: Callable[[str], str], command: str, message: str
) -> None:
    shell.ok(f'read "{resource("siqad_or_gate.sqd")}"; generate mux -b 1')
    assert message in shell.fails(command)
    assert len(shell.session.networks) == len(shell.session.cell_layouts) == 1


@pytest.mark.parametrize("topology", ["cartesian", "even_row_hex", "odd_row_hex", "odd_column_hex", "even_column_hex"])
@pytest.mark.parametrize("quiet", [False, True])
def test_progress_respects_quiet_on_terminal(
    resource: Callable[[str], str], monkeypatch: pytest.MonkeyPatch, topology: str, *, quiet: bool
) -> None:
    """Terminal progress respects quiet mode for every orthogonal topology."""
    monkeypatch.setenv("TERM", "xterm")
    buffer = io.StringIO()
    console = Console(file=buffer, width=100, force_terminal=True, color_system=None)
    session = Session(console=console)
    session.quiet = quiet
    try:
        assert session.execute(f'read "{resource("mux21.v")}"; ortho --topology {topology}')
    finally:
        session.close()
    if quiet:
        assert not buffer.getvalue()
    else:
        assert "ortho" in buffer.getvalue()
        assert "placing gates" in buffer.getvalue()
    assert session.report_progress is ignore_progress


def test_progress_is_silent_without_terminal(mux21_shell: Shell) -> None:
    """Without a terminal, the progress display writes nothing."""
    output = mux21_shell.ok("ortho")
    assert "placing gates" not in output
    assert "mux21" in output


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


def test_progress_serializes_shared_task_reports(monkeypatch: pytest.MonkeyPatch) -> None:
    """Concurrent reporters create one bar for a shared task name."""
    session = Session(console=Console(file=io.StringIO(), force_terminal=True))
    display = Progress(console=session.console, auto_refresh=False)
    monkeypatch.setattr("mnt.fiction.cli.session.Progress", lambda *_args, **_kwargs: display)
    add_task = display.add_task

    def delayed_add_task(description: str, *, total: float | None = 100, spinner: bool = False) -> TaskID:
        """Yield while Rich creates a task.

        Returns:
            The new task's identifier.
        """
        time.sleep(0.05)
        return add_task(description, total=total, spinner=spinner)

    monkeypatch.setattr(display, "add_task", delayed_add_task)
    ready = threading.Barrier(4)
    with session.progress("simulation") as report:

        def report_from_worker(_: int) -> None:
            """Start the reports together."""
            ready.wait(timeout=5)
            report("compositions", 0, 0)

        with ThreadPoolExecutor(max_workers=4) as workers:
            list(workers.map(report_from_worker, range(4)))

    assert sum(task.description == "compositions" for task in display.tasks) == 1


def test_progress_restarts_with_an_unknown_total(monkeypatch: pytest.MonkeyPatch) -> None:
    """A new pass clears the old total even if the old pass completed no items."""
    session = Session(console=Console(file=io.StringIO(), force_terminal=True))
    display = Progress(console=session.console, auto_refresh=False)
    monkeypatch.setattr("mnt.fiction.cli.session.Progress", lambda *_args, **_kwargs: display)
    with session.progress("simulation") as report:
        report("compositions", 0, 4)
        report("compositions", 0, 0)
        task = next(task for task in display.tasks if task.description == "compositions")
        assert task.total is None


def test_quiet_nested_progress_discards_reports() -> None:
    """A quiet nested command suspends the outer command's progress callback."""
    session = Session(console=Console(file=io.StringIO(), force_terminal=True))
    with session.progress("source") as outer:
        session.quiet = True
        with session.progress("ortho") as inner:
            assert inner is ignore_progress
            assert session.report_progress is ignore_progress
        assert session.report_progress is outer


def test_progress_reports_are_dropped_without_terminal(shell: Shell) -> None:
    """Without a terminal, the callback handed to the algorithms discards the reports."""
    with shell.session.progress("optimize") as report:
        report("gate relocations", 0, 4)
        report("gate relocations", 4, 4)
    assert not shell.output
