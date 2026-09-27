# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.layouts import (
    CartesianGateLayout,
    EvenColumnHexGateLayout,
    HexagonalGateLayout,
    OddColumnHexGateLayout,
    OddRowHexGateLayout,
    ShiftedCartesianGateLayout,
)

if TYPE_CHECKING:
    from mnt.pyfiction.layouts._types import GateLayout
    from mnt.pyfiction.networks import TechnologyNetwork

from mnt.pyfiction.networks import HighDegreeFaninError
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.physical_design import OrthogonalParams, orthogonal
from mnt.pyfiction.verification import EquivalenceType, equivalence_checking


def test_orthogonal_default(mux21):
    layout = orthogonal(mux21).layout
    assert equivalence_checking(mux21, layout).eq == EquivalenceType.STRONG


def test_orthogonal_with_parameters(mux21):
    params = OrthogonalParams()

    layout = orthogonal(mux21, params=params).layout

    assert equivalence_checking(mux21, layout).eq == EquivalenceType.STRONG


def test_orthogonal_with_stats(mux21):

    result = orthogonal(mux21)
    layout = result.layout

    assert equivalence_checking(mux21, layout).eq == EquivalenceType.STRONG


def test_orthogonal_reports_progress(mux21):
    assert OrthogonalParams().on_progress is None

    params = OrthogonalParams()
    reports = []
    params.on_progress = lambda task, done, total: reports.append((task, done, total))

    layout = orthogonal(mux21, params=params).layout

    assert equivalence_checking(mux21, layout).eq == EquivalenceType.STRONG
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

    with pytest.raises(HighDegreeFaninError):
        orthogonal(read_network(str(path)))


@pytest.mark.parametrize(
    "layout_type",
    [CartesianGateLayout, HexagonalGateLayout, OddRowHexGateLayout, OddColumnHexGateLayout, EvenColumnHexGateLayout],
)
def test_orthogonal_explicit_topology(mux21: TechnologyNetwork, layout_type: type[GateLayout]) -> None:
    result = orthogonal(mux21, layout_type=layout_type)
    assert isinstance(result.layout, layout_type)
    assert result.stats.num_gates == result.layout.num_gates()
    assert equivalence_checking(mux21, result.layout).eq == EquivalenceType.STRONG


def test_orthogonal_rejects_unsupported_topology(mux21: TechnologyNetwork) -> None:
    with pytest.raises(ValueError, match="does not support ShiftedCartesianGateLayout"):
        orthogonal(mux21, layout_type=ShiftedCartesianGateLayout)
