# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of the dispatcher: tokenizing, running, failing, and describing statistics."""

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest
from rich.cells import cell_len

from mnt.fiction.cli.errors import CommandError
from mnt.fiction.cli.parsing import tokenize
from mnt.fiction.cli.registry import REGISTRY, STORE_FLAGS, Category
from mnt.fiction.cli.session import Session
from mnt.fiction.cli.statistics import stats_to_dict
from mnt.fiction.cli.stores import Store
from mnt.pyfiction import orthogonal, orthogonal_stats, set_name

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path

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
