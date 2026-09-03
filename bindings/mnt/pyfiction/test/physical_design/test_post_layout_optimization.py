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


def test_post_layout_optimization_with_stats(mux21):
    layout = orthogonal(mux21)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG

    stats = post_layout_optimization_stats()
    post_layout_optimization(layout, statistics=stats)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    assert stats.x_size_before == 6
    assert stats.y_size_before == 8
    assert stats.x_size_after == 6
    assert stats.y_size_after == 4
    assert stats.area_improvement == pytest.approx(50.0, abs=1e-9)


def test_post_layout_optimization_with_stats_and_parameters(mux21):
    layout = orthogonal(mux21)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG

    params = post_layout_optimization_params()
    params.max_gate_relocations = 1
    params.timeout = 1000000

    stats = post_layout_optimization_stats()
    post_layout_optimization(layout, params, statistics=stats)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    assert stats.x_size_before == 6
    assert stats.y_size_before == 8
    assert stats.x_size_after == 6
    assert stats.y_size_after == 4
    assert stats.area_improvement == pytest.approx(50.0, abs=1e-9)
