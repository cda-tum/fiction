# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import itertools
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
from mnt.pyfiction.physical_design.path_finding import enumerate_all_paths, yen_k_shortest_paths

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

    assert len(paths) == 2
    assert [(0, 0), (0, 1), (1, 1)] in paths
    assert [(0, 0), (1, 0), (1, 1)] in paths


def test_unclocked_cartesian_paths() -> None:
    """Enumerate both simple routes across a two-by-two Cartesian layout."""
    paths = enumerate_all_paths(cartesian_layout((2, 2)), (0, 0), (1, 1))
    assert len(paths) == 2
    assert [(0, 0), (0, 1), (1, 1)] in paths
    assert [(0, 0), (1, 0), (1, 1)] in paths


def test_hexagonal_clocking_path() -> None:
    """Hexagonal clocking permits the direct diagonal connection."""
    layout = hexagonal_gate_layout(arrangement.EVEN_ROW, (3, 3), "2DDWAVEHEX", "Layout")
    assert enumerate_all_paths(layout, (0, 0), (1, 1)) == [[(0, 0), (1, 1)]]


def test_unclocked_cartesian_detours() -> None:
    """Enumerate every simple route across a three-by-three Cartesian layout."""
    layout = cartesian_layout((3, 3))
    all_paths = enumerate_all_paths(layout, (0, 0), (2, 2))
    shortest_paths = yen_k_shortest_paths(layout, (0, 0), (2, 2), 13)
    assert len(all_paths) == len({tuple(path) for path in all_paths}) == 12
    for path in all_paths:
        assert path[0] == (0, 0)
        assert path[-1] == (2, 2)
        assert len(path) == len(set(path))
        assert all(0 <= site.x < 3 and 0 <= site.y < 3 and site.z == 0 for site in path)
        assert all(abs(a.x - b.x) + abs(a.y - b.y) == 1 for a, b in itertools.pairwise(path))
    assert len(shortest_paths) == 12
    assert {tuple(path) for path in shortest_paths} == {tuple(path) for path in all_paths}
    assert [len(path) for path in shortest_paths] == sorted(len(path) for path in shortest_paths)


def test_unreachable_target_with_clock_cycle() -> None:
    """A clock cycle cannot cause repeated traversal while seeking an unreachable target."""
    layout = cartesian_gate_layout((3, 2), "OPEN")
    for site, phase in [((0, 0), 0), ((1, 0), 1), ((1, 1), 2), ((0, 1), 3)]:
        layout.assign_clock_number(site, phase)
    assert enumerate_all_paths(layout, (0, 0), (2, 0)) == []
    assert yen_k_shortest_paths(layout, (0, 0), (2, 0), 2) == []


@pytest.mark.parametrize(
    "make_lyt",
    [
        pytest.param(lambda: cartesian_layout((2, 2)), id="cartesian_layout"),
        pytest.param(lambda: cartesian_gate_layout((2, 2), "2DDWave", "Layout"), id="cartesian_gate_layout"),
    ],
)
@pytest.mark.parametrize(("source", "target"), [((2, 0), (0, 0)), ((0, 0), (-1, 0))])
def test_enumeration_endpoint_bounds(
    make_lyt: Callable[[], cartesian_layout | cartesian_gate_layout],
    source: tuple[int, int],
    target: tuple[int, int],
) -> None:
    """Check endpoint bounds independently of the other valid endpoint."""
    lyt = make_lyt()
    with pytest.raises(ValueError):  # ruff: ignore[pytest-raises-too-broad] - Only the exception type is specified.
        enumerate_all_paths(lyt, source, target)
