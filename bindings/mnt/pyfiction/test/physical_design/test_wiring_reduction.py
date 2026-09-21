# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction import (
    eq_type,
    equivalence_checking,
    orthogonal,
    technology_network,
    wiring_reduction,
    wiring_reduction_params,
    wiring_reduction_stats,
)


def test_wiring_reduction_default(mux21):
    layout = orthogonal(mux21)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG

    wiring_reduction(layout)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_wiring_reduction_with_parameters(mux21):
    layout = orthogonal(mux21)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG

    params = wiring_reduction_params()
    wiring_reduction(layout, params)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_wiring_reduction_with_stats(mux21: technology_network) -> None:
    """Statistics describe the actual input and result, which remain logically equivalent."""
    layout = orthogonal(mux21)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG

    before = layout.clone()
    stats = wiring_reduction_stats()
    wiring_reduction(layout, statistics=stats)

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


def test_wiring_reduction_with_stats_and_parameters(mux21: technology_network) -> None:
    """Statistics describe the actual input and result, which remain logically equivalent."""
    layout = orthogonal(mux21)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG

    params = wiring_reduction_params()
    params.timeout = 1000000

    before = layout.clone()
    stats = wiring_reduction_stats()
    wiring_reduction(layout, params, stats)

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
