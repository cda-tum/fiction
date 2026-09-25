# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of the logic commands and of the aigverse bridge."""

from __future__ import annotations

import os
import subprocess  # ruff: ignore[suspicious-subprocess-import] -- bounded native crash regressions
import sys
from typing import TYPE_CHECKING

import pytest
from aigverse import abc
from aigverse.algorithms import equivalence_checking as aig_equivalent

from mnt.fiction.cli.aigverse_bridge import from_aigverse, to_aigverse
from mnt.fiction.cli.stores import size_and_depth
from mnt.pyfiction import networks
from mnt.pyfiction.networks import aig_network, simulate_outputs, technology_network

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path

    from .conftest import Shell


def test_map_on_a_technology_network(mux21_shell: Shell) -> None:
    mux21_shell.ok("map --and --or --inv")
    mapped = mux21_shell.session.networks.current()
    assert isinstance(mapped, technology_network)
    assert "stats" in mux21_shell.session.log[-1]["result"]  # type: ignore[operator]


def test_map_on_an_aig(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f'read "{resource("mux21.v")}" --type aig; map --all2')
    assert isinstance(shell.session.networks.current(), technology_network)
    assert len(shell.session.networks) == 2


def test_map_needs_gates(mux21_shell: Shell) -> None:
    assert "at least one gate type" in mux21_shell.fails("map")


@pytest.mark.parametrize("gates", ["--all", "--all3"])
def test_map_complete_gate_sets_preserves_outputs(mux21_shell: Shell, gates: str) -> None:
    mux21_shell.ok("simulate -n --store")
    expected = mux21_shell.session.truth_tables.current().to_binary()
    mux21_shell.ok(f"map {gates}; simulate -n --store")
    assert mux21_shell.session.truth_tables.current().to_binary() == expected


def test_fanouts_and_balance(mux21_shell: Shell) -> None:
    mux21_shell.ok("fanouts -d 3 -s depth; balance -u")
    assert len(mux21_shell.session.networks) == 3
    assert "usage" in mux21_shell.fails("fanouts -d 4")


def test_seeded_fanout_substitution_preserves_outputs(mux21_shell: Shell) -> None:
    mux21_shell.ok("simulate -n --store")
    expected = mux21_shell.session.truth_tables.current().to_binary()
    mux21_shell.ok("fanouts -s random --seed 7; simulate -n --store")
    assert mux21_shell.session.truth_tables.current().to_binary() == expected


def test_gates(mux21_shell: Shell) -> None:
    output = mux21_shell.ok("gates -n --detailed")
    assert "AND2" in output
    assert "LT2" in output
    assert mux21_shell.session.log[-1]["result"]["num_and2"] >= 1  # type: ignore[index]
    mux21_shell.ok("ortho")
    assert "FANOUT" in mux21_shell.ok("gates -g")


def test_simulate(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f'read "{resource("xor2.v")}"; simulate -n --store')
    assert "0110" in shell.output
    assert shell.session.truth_tables.current().to_binary() == "0110"
    shell.ok("ortho")
    shell.ok("simulate -g --silent")
    assert not shell.output
    assert shell.session.log[-1]["result"]["tables"][0]["binary"] == "0110"  # type: ignore[index]


def test_random_and_generate(shell: Shell) -> None:
    shell.ok("random -n 4 -g 10 --seed 7")
    network = shell.session.networks.current()
    assert isinstance(network, aig_network)
    assert network.num_pis() == 4
    assert shell.session.log[-1]["result"]["network"]["name"] == "7"  # type: ignore[index]
    shell.ok("generate rca -b 4")
    assert shell.session.networks.current().num_pis() == 8
    shell.ok("random -n 2 -g 3")


def test_aig_passes_preserve_the_function(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f'read "{resource("mux21.v")}" --type aig')
    original = shell.session.networks.current()
    shell.ok("aig rewrite resub refactor balance cleanup")
    optimized = shell.session.networks.current()
    assert isinstance(optimized, aig_network)
    assert aig_equivalent(to_aigverse(shell.session, original), to_aigverse(shell.session, optimized))
    assert shell.session.log[-1]["result"]["passes"] == ["rewrite", "resub", "refactor", "balance", "cleanup"]  # type: ignore[index]


def test_aig_reports_gates_and_depth(shell: Shell) -> None:
    shell.ok("generate rca -b 4")
    before = shell.session.networks.current()
    shell.ok("aig balance")
    after = shell.session.networks.current()
    # balancing trades gates for depth, so the line has to carry both numbers to be readable
    assert after.depth() < before.depth()
    assert after.num_gates() > before.num_gates()
    assert f"{before.num_gates()} -> {after.num_gates()} gates" in shell.stdout
    assert f"depth {before.depth()} -> {after.depth()}" in shell.stdout
    result = shell.session.log[-1]["result"]
    assert result["gates_before"] == before.num_gates()  # type: ignore[index]
    assert result["depth_before"] == before.depth()  # type: ignore[index]


def test_size_and_depth_without_an_input_network(shell: Shell) -> None:
    # 'abc --no-read' lets the flow provide its own input, so there is nothing to compare against
    shell.ok("generate rca -b 2")
    network = shell.session.networks.current()
    assert size_and_depth(None, network) == f"{network.num_gates()} gates, depth {network.depth()}"


def test_aig_needs_an_aig(mux21_shell: Shell) -> None:
    assert "--type aig" in mux21_shell.fails("aig rewrite")


def test_bridge_round_trip(mux21_shell: Shell, resource: Callable[[str], str]) -> None:
    mux21_shell.ok(f'read "{resource("mux21.v")}" --type aig')
    aig = mux21_shell.session.networks.current()
    back = from_aigverse(mux21_shell.session, to_aigverse(mux21_shell.session, aig), "back", like=aig)
    assert back.num_gates() == aig.num_gates()
    assert back.num_pis() == aig.num_pis()
    assert [back.get_name(pi) for pi in back.pis()] == [aig.get_name(pi) for pi in aig.pis()]
    assert back.get_output_name(0) == aig.get_output_name(0)


def test_abc(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f'read "{resource("mux21.v")}" --type aig')
    if not abc.is_available():
        assert not os.environ.get("FICTION_REQUIRE_ABC"), "the integration job requires external ABC"
        assert "AIGVERSE_ABC" in shell.fails("abc -s resyn2")
        pytest.skip("ABC is not installed")
    shell.ok("abc -s resyn2; abc -c 'balance; rewrite'")
    assert isinstance(shell.session.networks.current(), aig_network)


def test_abc_xag_and_custom_flow(
    shell: Shell, resource: Callable[[str], str], tmp_path: Path, monkeypatch: pytest.MonkeyPatch
) -> None:
    if not abc.is_available():
        assert not os.environ.get("FICTION_REQUIRE_ABC"), "the integration job requires external ABC"
        pytest.skip("ABC is not installed")
    shell.ok(f'read "{resource("xor2.v")}" --type xag')
    shell.ok("simulate -n")
    expected = shell.session.log[-1]["result"]
    shell.ok("abc -c balance --no-strash; simulate -n")
    assert shell.session.log[-1]["result"] == expected
    original = shell.session.networks.current()
    shell.ok("abc -c print_stats --no-write")
    assert shell.session.networks.current() is original
    path = tmp_path / "custom input.aig"
    shell.ok(f'write_aiger "{path}"')
    monkeypatch.chdir(tmp_path)
    shell.ok(f"abc --no-read --no-strash -c 'read_aiger \"{path.name}\"; strash; balance'")
    shell.ok("simulate -n")
    assert shell.session.log[-1]["result"] == expected


@pytest.mark.parametrize("options", ["--inv", "--xnor --inv"])
def test_incomplete_mapping_is_bounded_and_preserves_store(resource: Callable[[str], str], options: str) -> None:
    code = """
import sys
from mnt.fiction.cli import Session
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


def test_mapping_with_statistics_preserves_function(mux21_shell: Shell) -> None:
    expected = simulate_outputs(mux21_shell.session.networks.current())
    mux21_shell.ok("map --and --inv --verbose")
    assert simulate_outputs(mux21_shell.session.networks.current()) == expected
    result = mux21_shell.session.log[-1]["result"]
    assert isinstance(result, dict)
    assert isinstance(result["stats"], dict)
    assert isinstance(result["stats"]["mapper_stats"], dict)
    assert result["stats"]["mapper_stats"]["mapping_error"] is False


@pytest.mark.parametrize("kind", ["aig", "xag", "mig", "tec"])
def test_seeded_random_networks(shell: Shell, kind: str) -> None:
    shell.ok(f"random --type {kind} -n 4 -g 30 --seed 17")
    network = shell.session.networks.current()
    assert network.num_pis() == 4
    assert isinstance(network, getattr(networks, "technology_network" if kind == "tec" else f"{kind}_network"))
    shell.ok(f"random --type {kind} -n 4 -g 30 --seed 17")
    assert simulate_outputs(network) == simulate_outputs(shell.session.networks.current())
