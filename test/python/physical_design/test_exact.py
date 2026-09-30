# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction.layouts import arrangement
from mnt.pyfiction.physical_design import (
    exact_cartesian,
    exact_hexagonal,
    exact_params,
    exact_shifted_cartesian,
    exact_stats,
)
from mnt.pyfiction.verification import eq_type, equivalence_checking


def test_exact_cartesian_default(mux21):
    layout = exact_cartesian(mux21)
    assert layout is not None
    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_exact_cartesian_with_parameters(mux21):
    params = exact_params()
    params.border_io = True
    params.crossings = True
    params.scheme = "ESR"

    layout = exact_cartesian(mux21, params)
    assert layout is not None

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_exact_cartesian_with_stats(mux21):
    stats = exact_stats()

    layout = exact_cartesian(mux21, statistics=stats)
    assert layout is not None

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_exact_hexagonal_default(mux21):
    params = exact_params()
    params.layout_arrangement = arrangement.EVEN_ROW

    layout = exact_hexagonal(mux21, params)
    assert layout is not None
    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_exact_hexagonal_with_parameters(mux21):
    params = exact_params()
    params.border_io = True
    params.crossings = True
    params.scheme = "ESR"
    params.layout_arrangement = arrangement.EVEN_ROW

    layout = exact_hexagonal(mux21, params)
    assert layout is not None

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_exact_hexagonal_with_stats(mux21):
    stats = exact_stats()
    params = exact_params()
    params.layout_arrangement = arrangement.EVEN_ROW

    layout = exact_hexagonal(mux21, params, stats)
    assert layout is not None

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_exact_layout_arrangement_is_optional_and_writable():
    params = exact_params()
    assert params.layout_arrangement is None

    params.layout_arrangement = arrangement.ODD_COLUMN
    assert params.layout_arrangement == arrangement.ODD_COLUMN


@pytest.mark.parametrize("design", [exact_shifted_cartesian, exact_hexagonal])
def test_exact_requires_an_arrangement_for_shifted_and_hexagonal_layouts(mux21, design):
    with pytest.raises(ValueError, match="arrangement"):
        design(mux21)
