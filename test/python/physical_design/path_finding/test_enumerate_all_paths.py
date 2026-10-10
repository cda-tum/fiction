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
    arrangement,
    cartesian_gate_layout,
    cartesian_layout,
    coordinate,
    hexagonal_gate_layout,
    hexagonal_layout,
    shifted_cartesian_gate_layout,
    shifted_cartesian_layout,
)
from mnt.pyfiction.physical_design.path_finding import enumerate_all_paths

if TYPE_CHECKING:
    from collections.abc import Callable
    from typing import TypeAlias

CoordinateLayout: TypeAlias = cartesian_layout | shifted_cartesian_layout | hexagonal_layout
GateLayout: TypeAlias = cartesian_gate_layout | shifted_cartesian_gate_layout | hexagonal_gate_layout


@pytest.mark.parametrize(
    "make_lyt",
    [
        pytest.param(lambda: cartesian_layout((5, 5)), id="cartesian_layout"),
        pytest.param(lambda: shifted_cartesian_layout(arrangement.ODD_COLUMN, (5, 5)), id="shifted_cartesian_layout"),
        pytest.param(lambda: hexagonal_layout(arrangement.EVEN_ROW, (5, 5)), id="hexagonal_layout"),
    ],
)
def test_non_clocked_paths(make_lyt: Callable[[], CoordinateLayout]) -> None:
    """Enumerate the zero-length path on unclocked geometry."""
    lyt = make_lyt()
    assert enumerate_all_paths(lyt, coordinate(0, 0), coordinate(0, 0)) == [[(0, 0)]]


@pytest.mark.parametrize(
    "make_lyt",
    [
        pytest.param(lambda: cartesian_gate_layout((5, 5), "2DDWave", "Layout"), id="cartesian_gate_layout"),
        pytest.param(
            lambda: shifted_cartesian_gate_layout(arrangement.ODD_COLUMN, (5, 5), "2DDWave", "Layout"),
            id="shifted_cartesian_gate_layout",
        ),
        pytest.param(
            lambda: hexagonal_gate_layout(arrangement.EVEN_ROW, (5, 5), "2DDWave", "Layout"), id="hexagonal_gate_layout"
        ),
    ],
)
def test_clocking_paths(make_lyt: Callable[[], GateLayout]) -> None:
    """Enumerate all short paths that follow clock flow."""
    lyt = make_lyt()
    assert enumerate_all_paths(lyt, coordinate(0, 0), coordinate(0, 0)) == [[(0, 0)]]
    assert enumerate_all_paths(lyt, coordinate(0, 0), coordinate(1, 0)) == [[(0, 0), (1, 0)]]
    assert enumerate_all_paths(lyt, coordinate(0, 0), coordinate(0, 1)) == [[(0, 0), (0, 1)]]

    paths = enumerate_all_paths(lyt, coordinate(0, 0), coordinate(1, 1))

    assert [(0, 0), (0, 1), (1, 1)] in paths
    assert [(0, 0), (1, 0), (1, 1)] in paths
