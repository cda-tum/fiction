# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction.layouts import coordinate, extent, normalize_layout_coordinates, random_coordinate
from mnt.pyfiction.qca import qca_cell_mode, qca_cell_type, qca_layout


def test_normalize_preserves_cells_metadata_and_padding() -> None:
    layout = qca_layout((7, 6, 2), "USE", "crossing", 2, 2)
    layout.assign_cell_type((3, 2), qca_cell_type.INPUT)
    layout.assign_cell_name((3, 2), "a")
    for position in [(4, 3), (4, 3, 1)]:
        layout.assign_cell_type(position, qca_cell_type.NORMAL)
    layout.assign_cell_mode((4, 3, 1), qca_cell_mode.VERTICAL)
    normalized = normalize_layout_coordinates(layout)
    assert normalized.get_extent() == extent(4, 4, 2)
    assert normalized.num_cells() == 3
    assert sorted(normalized.cells()) == sorted([coordinate(0, 0), coordinate(1, 1), coordinate(1, 1, 1)])
    assert normalized.get_cell_type((0, 0)) == qca_cell_type.INPUT
    assert normalized.get_cell_name((0, 0)) == "a"
    assert normalized.get_cell_mode((1, 1, 1)) == qca_cell_mode.VERTICAL
    assert normalized.get_layout_name() == "crossing"
    assert normalized.get_clocking_scheme_name() == "USE"
    assert (normalized.get_tile_size_x(), normalized.get_tile_size_y()) == (2, 2)
    assert layout.get_cell_type((3, 2)) == qca_cell_type.INPUT
    assert layout.get_extent() == extent(7, 6, 2)


@pytest.mark.parametrize("size", [(0, 0, 0), (3, 3, 1)])
def test_normalize_encloses_outside_occupancy(size: tuple[int, int, int]) -> None:
    layout = qca_layout(size)
    layout.assign_cell_type((5, 7), qca_cell_type.INPUT)
    layout.assign_cell_type((8, 9), qca_cell_type.NORMAL)
    normalized = normalize_layout_coordinates(layout)
    assert normalized.get_extent() == extent(4, 3, 1)
    assert sorted(normalized.cells()) == [coordinate(0, 0), coordinate(3, 2)]
    assert normalized.get_cell_type((0, 0)) == qca_cell_type.INPUT
    assert normalized.get_cell_type((3, 2)) == qca_cell_type.NORMAL


def test_normalize_negative_cells_retains_trailing_padding() -> None:
    layout = qca_layout((4, 5))
    layout.assign_cell_type((-2, -3), qca_cell_type.INPUT)
    normalized = normalize_layout_coordinates(layout)
    assert normalized.get_extent() == extent(6, 8, 1)
    assert normalized.cells() == [coordinate(0, 0)]
    assert normalized.get_cell_type((0, 0)) == qca_cell_type.INPUT


def test_normalize_rejects_unrepresentable_occupied_span() -> None:
    layout = qca_layout()
    layout.assign_cell_type((-(2**31), 0), qca_cell_type.INPUT)
    layout.assign_cell_type((2**31 - 1, 0), qca_cell_type.NORMAL)
    with pytest.raises(ValueError, match="coordinate domain"):
        normalize_layout_coordinates(layout)
    assert layout.num_cells() == 2


@pytest.mark.parametrize(
    ("first", "second"),
    [
        ((0, 0, 0), (0, 0, 0)),
        ((1, 0, 0), (1, 0, 0)),
        ((1, 1, 1), (5, 2, 3)),
        ((-10, -5, 0), (-10, -5, 0)),
        ((-10, -1, 3), (-10, -1, 6)),
        ((5, 1, 0), (1, 2, 0)),
        ((5, -1, 3), (1, 2, -4)),
        ((-(2**31), 0, 0), (2**31 - 1, 0, 0)),
    ],
)
def test_random_coordinate_inclusive_signed_bounds(first, second) -> None:
    bounds = [(min(a, b), max(a, b)) for a, b in zip(first, second, strict=True)]
    for start, stop in [(first, second), (second, first)]:
        for _ in range(32):
            result = random_coordinate(start, stop)
            assert all(
                low <= axis <= high for axis, (low, high) in zip((result.x, result.y, result.z), bounds, strict=True)
            )
