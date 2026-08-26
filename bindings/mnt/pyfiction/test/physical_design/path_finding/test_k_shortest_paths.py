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
    cartesian_gate_layout,
    cartesian_layout,
    clocked_cartesian_layout,
    clocked_hexagonal_layout,
    clocked_shifted_cartesian_layout,
    hexagonal_gate_layout,
    hexagonal_layout,
    offset_coordinate,
    shifted_cartesian_gate_layout,
    shifted_cartesian_layout,
    yen_k_shortest_paths,
)


@pytest.mark.parametrize(
    "make_lyt",
    [
        pytest.param(lambda: cartesian_layout((4, 4)), id="cartesian_layout"),
        pytest.param(lambda: shifted_cartesian_layout((4, 4)), id="shifted_cartesian_layout"),
        pytest.param(lambda: hexagonal_layout((4, 4)), id="hexagonal_layout"),
    ],
)
def test_non_clocked_yen_paths(make_lyt):
    lyt = make_lyt()
    assert yen_k_shortest_paths(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0), 1) == [[(0, 0)]]


@pytest.mark.parametrize(
    "make_lyt",
    [
        pytest.param(lambda: clocked_cartesian_layout((4, 4), "2DDWave"), id="clocked_cartesian_layout"),
        pytest.param(lambda: cartesian_gate_layout((4, 4), "2DDWave", "Layout"), id="cartesian_gate_layout"),
        pytest.param(
            lambda: clocked_shifted_cartesian_layout((4, 4), "2DDWave"), id="clocked_shifted_cartesian_layout"
        ),
        pytest.param(
            lambda: shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"), id="shifted_cartesian_gate_layout"
        ),
        pytest.param(lambda: clocked_hexagonal_layout((4, 4), "2DDWave"), id="clocked_hexagonal_layout"),
        pytest.param(lambda: hexagonal_gate_layout((4, 4), "2DDWave", "Layout"), id="hexagonal_gate_layout"),
    ],
)
def test_clocked_yen_paths(make_lyt):
    lyt = make_lyt()
    assert yen_k_shortest_paths(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0), 1) == [[(0, 0)]]
    assert yen_k_shortest_paths(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0), 1) == [[(0, 0), (1, 0)]]
    assert yen_k_shortest_paths(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1), 1) == [[(0, 0), (0, 1)]]

    paths = yen_k_shortest_paths(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1), 2)

    assert [(0, 0), (0, 1), (1, 1)] in paths
    assert [(0, 0), (1, 0), (1, 1)] in paths
