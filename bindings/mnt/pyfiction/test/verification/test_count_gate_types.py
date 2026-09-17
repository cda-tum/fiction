# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of counting gate types in networks and layouts."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction import count_gate_types, orthogonal, read_aig_network

if TYPE_CHECKING:
    from pathlib import Path

    from mnt.pyfiction import technology_network


def _total(stats: object) -> int:
    return sum(getattr(stats, name) for name in dir(stats) if name.startswith("num_"))


def test_count_gate_types_of_a_network(mux21: technology_network) -> None:
    stats = count_gate_types(mux21)
    assert stats.num_and2 == 2
    assert stats.num_or2 == 1
    assert stats.num_inv == 1
    assert _total(stats) == mux21.num_gates()
    assert "AND2" in stats.report()
    assert "LT2" not in stats.report()
    assert "LT2" in stats.report(detailed=True)


def test_count_gate_types_of_an_aig(resources_dir: Path) -> None:
    aig = read_aig_network(str(resources_dir / "mux21.v"))
    assert count_gate_types(aig).num_and2 == aig.num_gates()


def test_count_gate_types_of_a_layout(mux21: technology_network) -> None:
    layout = orthogonal(mux21)
    stats = count_gate_types(layout)
    assert stats.num_and2 == 2
    assert stats.num_or2 == 1
    assert stats.num_inv == 1
    assert stats.num_fanout >= 1
    assert stats.num_buf >= 1  # every wire tile is a buffer
