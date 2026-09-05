# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of the logic commands and of the aigverse bridge."""

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest
from aigverse import abc
from aigverse.algorithms import equivalence_checking as aig_equivalent

from mnt.pyfiction import aig_network, technology_network
from mnt.pyfiction.cli.commands.logic import from_aigverse, to_aigverse

if TYPE_CHECKING:
    from collections.abc import Callable

    from .conftest import Shell


def test_map_on_a_technology_network(mux21_shell: Shell) -> None:
    mux21_shell.ok("map --and --or --inv")
    mapped = mux21_shell.session.networks.current()
    assert isinstance(mapped, technology_network)
    assert "stats" in mux21_shell.session.log[-1]["result"]  # type: ignore[operator]


def test_map_on_an_aig(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f"read {resource('mux21.v')} --type aig; map --all2")
    assert isinstance(shell.session.networks.current(), technology_network)
    assert len(shell.session.networks) == 2


def test_map_needs_gates(mux21_shell: Shell) -> None:
    assert "at least one gate type" in mux21_shell.fails("map")


def test_fanouts_and_balance(mux21_shell: Shell) -> None:
    mux21_shell.ok("fanouts -d 3 -s depth; balance -u")
    assert len(mux21_shell.session.networks) == 3
    assert "usage" in mux21_shell.fails("fanouts -d 4")


def test_gates(mux21_shell: Shell) -> None:
    output = mux21_shell.ok("gates -n --detailed")
    assert "AND2" in output
    assert "LT2" in output
    assert mux21_shell.session.log[-1]["result"]["num_and2"] >= 1  # type: ignore[index]
    mux21_shell.ok("ortho")
    assert "FANOUT" in mux21_shell.ok("gates -g")


def test_simulate(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f"read {resource('xor2.v')}; simulate -n --store")
    assert "0110" in shell.output
    assert shell.session.truth_tables.current().to_binary() == "0110"
    shell.ok("ortho; simulate -g --silent")
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
    shell.ok(f"read {resource('mux21.v')} --type aig")
    original = shell.session.networks.current()
    shell.ok("aig rewrite resub refactor balance cleanup")
    optimized = shell.session.networks.current()
    assert isinstance(optimized, aig_network)
    assert aig_equivalent(to_aigverse(shell.session, original), to_aigverse(shell.session, optimized))
    assert shell.session.log[-1]["result"]["passes"] == ["rewrite", "resub", "refactor", "balance", "cleanup"]  # type: ignore[index]


def test_aig_needs_an_aig(mux21_shell: Shell) -> None:
    assert "--type aig" in mux21_shell.fails("aig rewrite")


def test_bridge_round_trip(mux21_shell: Shell, resource: Callable[[str], str]) -> None:
    mux21_shell.ok(f"read {resource('mux21.v')} --type aig")
    aig = mux21_shell.session.networks.current()
    back = from_aigverse(mux21_shell.session, to_aigverse(mux21_shell.session, aig), "back", like=aig)
    assert back.num_gates() == aig.num_gates()
    assert back.num_pis() == aig.num_pis()
    assert [back.get_name(pi) for pi in back.pis()] == [aig.get_name(pi) for pi in aig.pis()]
    assert back.get_output_name(0) == aig.get_output_name(0)


def test_abc(shell: Shell, resource: Callable[[str], str]) -> None:
    shell.ok(f"read {resource('mux21.v')} --type aig")
    if not abc.is_available():
        assert "AIGVERSE_ABC" in shell.fails("abc -s resyn2")
        pytest.skip("ABC is not installed")
    shell.ok("abc -s resyn2; abc -c 'balance; rewrite'")
    assert isinstance(shell.session.networks.current(), aig_network)
