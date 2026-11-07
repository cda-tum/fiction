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
    obstructions,
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


def test_clock_cycle_paths() -> None:
    """Search a clock cycle without repeating coordinates."""
    layout = cartesian_gate_layout((3, 2), "OPEN")
    for site, phase in [((0, 0), 0), ((1, 0), 1), ((1, 1), 2), ((0, 1), 3)]:
        layout.assign_clock_number(site, phase)
    layout.assign_clock_number((2, 0), 0)
    layout.assign_clock_number((2, 1), 0)
    path = [[(0, 0), (1, 0), (1, 1), (0, 1)]]
    assert enumerate_all_paths(layout, (0, 0), (0, 1)) == path
    assert yen_k_shortest_paths(layout, (0, 0), (0, 1), 2) == path
    assert enumerate_all_paths(layout, (0, 0), (0, 0)) == [[(0, 0)]]
    assert yen_k_shortest_paths(layout, (0, 0), (0, 0), 2) == [[(0, 0)]]
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


@pytest.mark.parametrize(
    ("scheme", "counts"),
    [(None, (184, 108, 108)), ("2DDWave", (20, 19, 19)), ("USE", (4, 1, 1))],
    ids=["unclocked", "twoddwave", "use"],
)
def test_enumeration_and_yen_cover_complete_four_by_four_routes(
    scheme: str | None, counts: tuple[int, int, int]
) -> None:
    """Check path multiplicity, validity, and ranking under coordinate and directed-edge constraints."""
    layout = cartesian_layout((4, 4)) if scheme is None else cartesian_gate_layout((4, 4), scheme)
    for obstruction, expected_count in zip(["none", "coordinate", "connection"], counts, strict=False):
        blocked = obstructions()
        if obstruction == "coordinate":
            blocked.obstruct_coordinate((3, 0))
        if obstruction == "connection":
            blocked.obstruct_connection((2, 0), (3, 0))
        paths = enumerate_all_paths(layout, (0, 0), (3, 3), obstructions=blocked)
        assert len(paths) == len({tuple(path) for path in paths}) == expected_count
        for path in paths:
            assert path[0] == (0, 0)
            assert path[-1] == (3, 3)
            assert len(path) == len(set(path))
            assert all(0 <= site.x < 4 and 0 <= site.y < 4 and site.z == 0 for site in path)
            assert all(abs(a.x - b.x) + abs(a.y - b.y) == 1 for a, b in itertools.pairwise(path))
            if isinstance(layout, cartesian_gate_layout):
                assert all(itertools.starmap(layout.is_outgoing_clocked, itertools.pairwise(path)))
            assert all(not blocked.is_obstructed_coordinate(site) for site in path)
            assert all(not blocked.is_obstructed_connection(a, b) for a, b in itertools.pairwise(path))
        shortest = yen_k_shortest_paths(layout, (0, 0), (3, 3), 7, obstructions=blocked)
        assert len(shortest) == len({tuple(path) for path in shortest}) == min(7, expected_count)
        assert all(path in paths for path in shortest)
        assert [len(path) for path in shortest] == sorted(len(path) for path in paths)[:7]


def test_clock_cycle_with_a_chord() -> None:
    """A chord adds a second simple route without revisiting the clock cycle."""
    layout = cartesian_gate_layout((3, 3), "OPEN")
    for site, clock in [
        ((0, 0), 0),
        ((1, 0), 1),
        ((2, 0), 2),
        ((2, 1), 3),
        ((2, 2), 0),
        ((1, 2), 1),
        ((0, 2), 2),
        ((0, 1), 3),
        ((1, 1), 0),
    ]:
        layout.assign_clock_number(site, clock)
    expected = [[(0, 1), (0, 0), (1, 0), (2, 0), (2, 1)], [(0, 1), (1, 1), (1, 0), (2, 0), (2, 1)]]
    for _ in range(2):
        assert sorted(enumerate_all_paths(layout, (0, 1), (2, 1))) == sorted(expected)
        assert sorted(yen_k_shortest_paths(layout, (0, 1), (2, 1), 3)) == sorted(expected)
