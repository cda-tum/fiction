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

from mnt.pyfiction.physical_design import WiringReductionParams, orthogonal, wiring_reduction
from mnt.pyfiction.verification import eq_type, equivalence_checking

if TYPE_CHECKING:
    from mnt.pyfiction.networks import TechnologyNetwork


def test_wiring_reduction_default(mux21):
    layout = orthogonal(mux21).layout

    assert equivalence_checking(mux21, layout) == eq_type.STRONG

    layout = wiring_reduction(layout).layout

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_wiring_reduction_with_parameters(mux21):
    layout = orthogonal(mux21).layout

    assert equivalence_checking(mux21, layout) == eq_type.STRONG

    params = WiringReductionParams()
    layout = wiring_reduction(layout, params=params).layout

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_wiring_reduction_with_stats(mux21: TechnologyNetwork) -> None:
    """Statistics describe the actual input and result, which remain logically equivalent."""
    layout = orthogonal(mux21).layout

    assert equivalence_checking(mux21, layout) == eq_type.STRONG

    original = layout
    before = layout.clone()
    result = wiring_reduction(layout)
    stats = result.stats
    layout = result.layout
    assert original.area() == before.area()

    assert equivalence_checking(mux21, layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    assert stats.x_size_before == before.x() + 1
    assert stats.y_size_before == before.y() + 1
    assert stats.x_size_after == layout.x() + 1
    assert stats.y_size_after == layout.y() + 1
    wires_before = before.num_wires() - before.num_pis() - before.num_pos()
    wires_after = layout.num_wires() - layout.num_pis() - layout.num_pos()
    assert stats.num_wires_before == wires_before
    assert stats.num_wires_after == wires_after
    assert layout.num_wires() < before.num_wires()
    assert stats.wiring_improvement == pytest.approx(100 * (1 - wires_after / wires_before), abs=0.01)
    assert layout.area() < before.area()
    assert stats.area_improvement == pytest.approx(100 * (1 - layout.area() / before.area()), abs=0.01)


def test_wiring_reduction_with_stats_and_parameters(mux21: TechnologyNetwork) -> None:
    """Statistics describe the actual input and result, which remain logically equivalent."""
    layout = orthogonal(mux21).layout

    assert equivalence_checking(mux21, layout) == eq_type.STRONG

    params = WiringReductionParams()
    params.timeout = 1000000

    original = layout
    before = layout.clone()
    result = wiring_reduction(layout, params=params)
    stats = result.stats
    layout = result.layout
    assert original.area() == before.area()

    assert equivalence_checking(mux21, layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    assert stats.x_size_before == before.x() + 1
    assert stats.y_size_before == before.y() + 1
    assert stats.x_size_after == layout.x() + 1
    assert stats.y_size_after == layout.y() + 1
    wires_before = before.num_wires() - before.num_pis() - before.num_pos()
    wires_after = layout.num_wires() - layout.num_pis() - layout.num_pos()
    assert stats.num_wires_before == wires_before
    assert stats.num_wires_after == wires_after
    assert layout.num_wires() < before.num_wires()
    assert stats.wiring_improvement == pytest.approx(100 * (1 - wires_after / wires_before), abs=0.01)
    assert layout.area() < before.area()
    assert stats.area_improvement == pytest.approx(100 * (1 - layout.area() / before.area()), abs=0.01)
