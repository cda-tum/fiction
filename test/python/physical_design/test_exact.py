# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING, cast

import pytest

from mnt.pyfiction.layouts import CartesianGateLayout, HexagonalGateLayout
from mnt.pyfiction.physical_design import ExactParams, exact
from mnt.pyfiction.verification import EquivalenceType, equivalence_checking

if TYPE_CHECKING:
    from mnt.pyfiction.networks import TechnologyNetwork


def test_exact_rejects_unsupported_layout_type(mux21: TechnologyNetwork) -> None:
    with pytest.raises(ValueError, match="exact does not support int"):
        exact(mux21, layout_type=cast("type[CartesianGateLayout]", int))


def test_exact_cartesian_default(mux21):
    layout = exact(mux21, layout_type=CartesianGateLayout).layout
    assert layout is not None
    assert equivalence_checking(mux21, layout).eq == EquivalenceType.STRONG


def test_exact_cartesian_with_parameters(mux21):
    params = ExactParams()
    params.border_io = True
    params.crossings = True
    params.scheme = "ESR"

    layout = exact(mux21, params=params, layout_type=CartesianGateLayout).layout
    assert layout is not None

    assert equivalence_checking(mux21, layout).eq == EquivalenceType.STRONG


def test_exact_cartesian_with_stats(mux21):

    result = exact(mux21, layout_type=CartesianGateLayout)
    layout = result.layout
    assert layout is not None

    assert equivalence_checking(mux21, layout).eq == EquivalenceType.STRONG


def test_exact_hexagonal_default(mux21):
    layout = exact(mux21, layout_type=HexagonalGateLayout).layout
    assert layout is not None
    assert equivalence_checking(mux21, layout).eq == EquivalenceType.STRONG


def test_exact_hexagonal_with_parameters(mux21):
    params = ExactParams()
    params.border_io = True
    params.crossings = True
    params.scheme = "ESR"

    layout = exact(mux21, params=params, layout_type=HexagonalGateLayout).layout
    assert layout is not None

    assert equivalence_checking(mux21, layout).eq == EquivalenceType.STRONG


def test_exact_hexagonal_with_stats(mux21):

    result = exact(mux21, layout_type=HexagonalGateLayout)
    layout = result.layout
    assert layout is not None

    assert equivalence_checking(mux21, layout).eq == EquivalenceType.STRONG
