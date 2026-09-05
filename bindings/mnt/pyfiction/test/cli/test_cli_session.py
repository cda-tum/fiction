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

from mnt.pyfiction import orthogonal, orthogonal_stats
from mnt.pyfiction.cli.registry import REGISTRY, Category, CommandError
from mnt.pyfiction.cli.session import stats_to_dict, tokenize
from mnt.pyfiction.cli.stores import Store

if TYPE_CHECKING:
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
        assert cmd.name == name
        assert isinstance(cmd.category, Category)
        assert cmd.summary
        assert "-h" in cmd.options


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
