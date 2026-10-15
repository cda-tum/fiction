# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction.layouts import (
    arrangement,
    cartesian_gate_layout,
    hexagonal_gate_layout,
    shifted_cartesian_gate_layout,
)
from mnt.pyfiction.networks import high_degree_fanin_exception
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.physical_design import num_clks, orthogonal, orthogonal_stats, place
from mnt.pyfiction.verification import eq_type, equivalence_checking

if TYPE_CHECKING:
    from pathlib import Path

    from mnt.pyfiction.networks import technology_network


def test_orthogonal_default(mux21):
    layout = orthogonal(mux21)
    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_orthogonal_with_parameters(mux21):
    layout = orthogonal(mux21, number_of_clock_phases=num_clks.FOUR)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_orthogonal_with_stats(mux21):
    stats = orthogonal_stats()

    layout = orthogonal(mux21, statistics=stats)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_orthogonal_reports_progress(mux21):
    options: dict[str, Any] = {}
    reports = []
    options["on_progress"] = lambda task, done, total: reports.append((task, done, total))

    layout = orthogonal(mux21, **options)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG

    placements = [(done, total) for task, done, total in reports if task == "placing gates"]
    assert placements[0][0] == 0
    assert placements == sorted(placements)
    assert placements[-1][0] == placements[-1][1] > 0

    options["on_progress"] = None
    before = list(reports)
    silent_layout = orthogonal(mux21, **options)
    assert reports == before
    assert equivalence_checking(mux21, silent_layout) == eq_type.STRONG


def test_orthogonal_rejects_high_degree_fanin(tmp_path):
    path = tmp_path / "maj.v"
    path.write_text(
        "module top(a, b, c, o);\ninput a, b, c;\noutput o;\nassign o = (a & b) | (a & c) | (b & c);\nendmodule\n",
        encoding="utf-8",
    )

    with pytest.raises(high_degree_fanin_exception):
        orthogonal(read_network(str(path)))


@pytest.mark.parametrize("a", list(arrangement))
def test_orthogonal_hexagonal_takes_the_arrangement_from_the_parameters(mux21, a):

    layout = orthogonal(mux21, layout_type=hexagonal_gate_layout, layout_arrangement=a)

    assert layout.get_arrangement() == a
    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_orthogonal_hexagonal_requires_an_arrangement(mux21):
    with pytest.raises(ValueError, match="arrangement"):
        orthogonal(mux21, layout_type=hexagonal_gate_layout)


@pytest.mark.parametrize("selector", [None, "hexagonal", int, shifted_cartesian_gate_layout])
def test_orthogonal_rejects_invalid_selectors(mux21: technology_network, selector: Any) -> None:
    """Orthogonal supports only Cartesian and hexagonal output classes."""
    with pytest.raises(TypeError):
        orthogonal(mux21, layout_type=selector)


def test_named_specification_and_place_overloads(tmp_path: Path) -> None:
    """Placement preserves the named technology-network contract."""
    path = tmp_path / "named.v"
    path.write_text(
        "module named_spec(left, right, result);\n"
        "input left, right;\noutput result;\nassign result = left & right;\nendmodule\n",
        encoding="utf-8",
    )
    network = read_network(str(path))
    layout = orthogonal(network)
    assert layout.get_layout_name() == "named_spec"
    assert [layout.get_name(layout.pi_at(i)) for i in range(2)] == ["left", "right"]
    assert layout.get_output_name(0) == "result"
    assert equivalence_checking(network, layout) == eq_type.STRONG

    placed = cartesian_gate_layout((2, 2), "2DDWave")
    left, right = network.pis()
    a = place(placed, (0, 0), network, left)
    b = place(placed, (1, 0), network, right)
    gate = place(placed, (1, 1), network, network.gates()[0], a, b, None)
    placed.create_po(gate, "result", (2, 1))
    assert placed.get_name(a) == "left"
    assert placed.get_name(b) == "right"
    assert equivalence_checking(network, placed) == eq_type.STRONG
