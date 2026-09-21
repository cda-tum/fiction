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
    post_layout_optimization,
    post_layout_optimization_params,
    post_layout_optimization_stats,
    technology_network,
)


def test_post_layout_optimization_default(mux21):
    layout = orthogonal(mux21)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG

    post_layout_optimization(layout)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_post_layout_optimization_with_parameters(mux21):
    layout = orthogonal(mux21)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG

    params = post_layout_optimization_params()
    post_layout_optimization(layout, params)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_post_layout_optimization_with_stats(mux21: technology_network) -> None:
    """Statistics describe the actual input and result, which remain logically equivalent."""
    layout = orthogonal(mux21)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG

    before = layout.clone()
    stats = post_layout_optimization_stats()
    post_layout_optimization(layout, statistics=stats)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    assert stats.x_size_before == before.x() + 1
    assert stats.y_size_before == before.y() + 1
    assert stats.x_size_after == layout.x() + 1
    assert stats.y_size_after == layout.y() + 1
    assert layout.area() < before.area()
    assert stats.area_improvement == pytest.approx(100 * (1 - layout.area() / before.area()), abs=0.01)


def test_post_layout_optimization_with_stats_and_parameters(mux21: technology_network) -> None:
    """Statistics describe the actual input and result, which remain logically equivalent."""
    layout = orthogonal(mux21)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG

    params = post_layout_optimization_params()
    params.max_gate_relocations = 1
    params.timeout = 1000000

    before = layout.clone()
    stats = post_layout_optimization_stats()
    post_layout_optimization(layout, params, statistics=stats)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    assert stats.x_size_before == before.x() + 1
    assert stats.y_size_before == before.y() + 1
    assert stats.x_size_after == layout.x() + 1
    assert stats.y_size_after == layout.y() + 1
    assert layout.area() < before.area()
    assert stats.area_improvement == pytest.approx(100 * (1 - layout.area() / before.area()), abs=0.01)
