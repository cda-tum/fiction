# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction.layouts import (
    CartesianGateLayout,
    HexagonalGateLayout,
    ShiftedCartesianGateLayout,
)
from mnt.pyfiction.layouts.coords import OffsetCoordinate
from mnt.pyfiction.physical_design.routing import yen_k_shortest_paths


@pytest.mark.parametrize(
    "make_lyt",
    [
        pytest.param(lambda: CartesianGateLayout((4, 4)), id="CartesianGateLayout"),
        pytest.param(lambda: ShiftedCartesianGateLayout((4, 4)), id="ShiftedCartesianGateLayout"),
        pytest.param(lambda: HexagonalGateLayout((4, 4)), id="HexagonalGateLayout"),
    ],
)
def test_non_clocked_yen_paths(make_lyt):
    lyt = make_lyt()
    assert yen_k_shortest_paths(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 0), 1) == [[(0, 0)]]


@pytest.mark.parametrize(
    "make_lyt",
    [
        pytest.param(lambda: CartesianGateLayout((4, 4), "2DDWave", "Layout"), id="CartesianGateLayout"),
        pytest.param(lambda: ShiftedCartesianGateLayout((4, 4), "2DDWave", "Layout"), id="ShiftedCartesianGateLayout"),
        pytest.param(lambda: HexagonalGateLayout((4, 4), "2DDWave", "Layout"), id="HexagonalGateLayout"),
    ],
)
def test_clocked_yen_paths(make_lyt):
    lyt = make_lyt()
    assert yen_k_shortest_paths(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 0), 1) == [[(0, 0)]]
    assert yen_k_shortest_paths(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(1, 0), 1) == [[(0, 0), (1, 0)]]
    assert yen_k_shortest_paths(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(0, 1), 1) == [[(0, 0), (0, 1)]]

    paths = yen_k_shortest_paths(lyt, OffsetCoordinate(0, 0), OffsetCoordinate(1, 1), 2)

    assert [(0, 0), (0, 1), (1, 1)] in paths
    assert [(0, 0), (1, 0), (1, 1)] in paths
