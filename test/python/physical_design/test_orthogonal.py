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

from mnt.pyfiction.layouts import arrangement, hexagonal_gate_layout, shifted_cartesian_gate_layout
from mnt.pyfiction.networks import high_degree_fanin_exception
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.physical_design import orthogonal, orthogonal_params, orthogonal_stats
from mnt.pyfiction.verification import eq_type, equivalence_checking

if TYPE_CHECKING:
    from mnt.pyfiction.networks import technology_network


def test_orthogonal_default(mux21):
    layout = orthogonal(mux21)
    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_orthogonal_with_parameters(mux21):
    params = orthogonal_params()

    layout = orthogonal(mux21, params)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_orthogonal_with_stats(mux21):
    stats = orthogonal_stats()

    layout = orthogonal(mux21, statistics=stats)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_orthogonal_reports_progress(mux21):
    assert orthogonal_params().on_progress is None

    params = orthogonal_params()
    reports = []
    params.on_progress = lambda task, done, total: reports.append((task, done, total))

    layout = orthogonal(mux21, params)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG
    assert params.on_progress is not None

    placements = [(done, total) for task, done, total in reports if task == "placing gates"]
    assert placements[0][0] == 0
    assert placements == sorted(placements)
    assert placements[-1][0] == placements[-1][1] > 0

    params.on_progress = None
    assert params.on_progress is None


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
    params = orthogonal_params()
    params.layout_arrangement = a

    layout = orthogonal(mux21, params, layout_type=hexagonal_gate_layout)

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
