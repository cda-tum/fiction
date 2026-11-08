# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from functools import partial
from itertools import starmap
from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction.inml import inml_layout
from mnt.pyfiction.layouts import (
    arrangement,
    cartesian_gate_layout,
    hexagonal_gate_layout,
    shifted_cartesian_gate_layout,
)
from mnt.pyfiction.qca import qca_layout

if TYPE_CHECKING:
    from collections.abc import Callable

CLOCKED_LAYOUTS = [
    pytest.param(lambda: cartesian_gate_layout((4, 3, 2), "2DDWave"), id="cartesian_gate_layout"),
    pytest.param(
        lambda: shifted_cartesian_gate_layout(arrangement.ODD_COLUMN, (4, 3, 2), "2DDWave"),
        id="shifted_cartesian_gate_layout",
    ),
    pytest.param(lambda: hexagonal_gate_layout(arrangement.EVEN_ROW, (4, 3, 2), "2DDWave"), id="hexagonal_gate_layout"),
]


@pytest.mark.parametrize("make_layout", CLOCKED_LAYOUTS)
def test_layout_coordinate_access(make_layout: Callable[[], Any]) -> None:
    """Check gate-layout coordinate order and neighbors on both layers."""
    layout = make_layout()
    assert [(t.x, t.y, t.z) for t in layout.coordinates()] == [
        (x, y, z) for z in range(2) for y in range(3) for x in range(4)
    ]
    assert [(t.x, t.y, t.z) for t in layout.ground_coordinates()] == [(x, y, 0) for y in range(3) for x in range(4)]
    neighbors = {
        cartesian_gate_layout: [(0, 1, 0), (1, 0, 0), (1, 2, 0), (2, 1, 0)],
        shifted_cartesian_gate_layout: [(0, 1, 0), (0, 2, 0), (1, 0, 0), (1, 2, 0), (2, 1, 0), (2, 2, 0)],
        hexagonal_gate_layout: [(0, 0, 0), (0, 1, 0), (0, 2, 0), (1, 0, 0), (1, 2, 0), (2, 1, 0)],
    }
    assert sorted((t.x, t.y, t.z) for t in layout.adjacent_coordinates((1, 1))) == neighbors[type(layout)]
    assert sorted((t.x, t.y, t.z) for t in layout.adjacent_coordinates((1, 1, 1))) == [
        (x, y, 1) for x, y, _ in neighbors[type(layout)]
    ]
    assert sorted((t.x, t.y, t.z) for t in layout.adjacent_coordinates((3, 2))) == [(2, 2, 0), (3, 1, 0)]


@pytest.mark.parametrize(
    "layout",
    [
        pytest.param(cartesian_gate_layout, id="cartesian_gate_layout"),
        pytest.param(
            partial(shifted_cartesian_gate_layout, arrangement.ODD_COLUMN), id="shifted_cartesian_gate_layout"
        ),
        pytest.param(partial(hexagonal_gate_layout, arrangement.EVEN_ROW), id="hexagonal_gate_layout"),
    ],
)
def test_fetch_clocking_scheme(layout: Callable[..., Any]) -> None:
    """Construct known clocking schemes and reject unknown names with ValueError."""
    layout((1, 1), "USE")
    layout((2, 2), "2DDWave")
    layout((3, 3), "RES")
    layout((4, 4), "ESR")
    layout((5, 5), "BANCS")

    with pytest.raises(ValueError, match="clocking scheme"):
        layout((1, 2), "3DDWave")
    with pytest.raises(ValueError, match="clocking scheme"):
        layout((1, 2), "SUE")
    with pytest.raises(ValueError, match="clocking scheme"):
        layout((1, 2), "PES")


@pytest.mark.parametrize("layout_type", [cartesian_gate_layout, qca_layout, inml_layout])
@pytest.mark.parametrize("scheme", ["2DDWave", "USE", "BANCS"])
def test_clocking_scheme_name(layout_type: Callable[..., Any], scheme: str) -> None:
    layout = layout_type((2, 2), scheme)
    name = layout.get_clocking_scheme_name()
    assert name == scheme.upper()
    assert layout.is_clocking_scheme(name)
    layout.replace_clocking_scheme("RES")
    assert not layout.is_clocking_scheme(name)
    assert layout.get_clocking_scheme_name() == "RES"


@pytest.mark.parametrize("make_layout", CLOCKED_LAYOUTS)
def test_clock_number_applies_to_every_layer(make_layout: Callable[[], Any]) -> None:
    layout = make_layout()
    layout.assign_clock_number((1, 1, 0), 3)
    assert layout.get_clock_number((1, 1, 1)) == 3

    layout.assign_clock_number((0, 1, 1), 2)
    assert layout.get_clock_number((0, 1, 0)) == 2


@pytest.mark.parametrize("make_layout", CLOCKED_LAYOUTS)
def test_clock_zone_iteration(make_layout: Callable[[], Any]) -> None:
    """Check clocked neighbors and boundary clipping."""
    layout = make_layout()
    assert layout.incoming_clocked_zones((0, 0)) == []
    assert layout.outgoing_clocked_zones((3, 2)) == []

    assert sorted((t.x, t.y, t.z) for t in layout.incoming_clocked_zones((1, 1))) == [(0, 1, 0), (1, 0, 0)]
    assert sorted((t.x, t.y, t.z) for t in layout.outgoing_clocked_zones((1, 1))) == [(1, 2, 0), (2, 1, 0)]


@pytest.mark.parametrize("make_layout", CLOCKED_LAYOUTS)
def test_replace_clocking_scheme_keeps_synchronization_elements(make_layout: Callable[[], Any]) -> None:
    """Lookup ignores case; replacement clears overrides and retains synchronization delays."""
    layout = make_layout()
    layout.assign_clock_number((1, 1), 3)
    layout.assign_synchronization_element((1, 1), 2)
    assert layout.is_synchronization_element((1, 1))

    original_name = layout.get_clocking_scheme_name()
    with pytest.raises(ValueError, match="Unknown clocking scheme"):
        layout.replace_clocking_scheme("3DDWave")
    assert layout.get_clocking_scheme_name() == original_name
    assert layout.get_clock_number((1, 1)) == 3
    assert layout.get_synchronization_element((1, 1)) == 2

    layout.replace_clocking_scheme("rEs")
    assert layout.is_clocking_scheme("RES")
    assert layout.is_regularly_clocked()
    assert layout.get_clock_number((1, 1)) == 1
    assert layout.get_synchronization_element((1, 1)) == 2
    assert layout.num_se() == 1

    layout.replace_clocking_scheme("2dDwAvEhEx")
    is_hexagonal = isinstance(layout, hexagonal_gate_layout)
    assert layout.get_clocking_scheme_name() == ("2DDWAVEHEX" if is_hexagonal else "2DDWAVE")
    assert layout.get_clock_number((1, 1)) == (1 if is_hexagonal else 2)
    assert layout.get_synchronization_element((1, 1)) == 2


@pytest.mark.parametrize(
    ("name", "period", "phases"),
    [
        ("COLUMNAR3", [[0, 0, 0], [1, 1, 1], [2, 2, 2]], 3),
        ("COLUMNAR4", [[0] * 4, [1] * 4, [2] * 4, [3] * 4], 4),
        ("ROW3", [[0, 1, 2]] * 3, 3),
        ("ROW4", [[0, 1, 2, 3]] * 4, 4),
        ("2DDWAVE3", [[0, 1, 2], [1, 2, 0], [2, 0, 1]], 3),
        ("2DDWAVE4", [[0, 1, 2, 3], [1, 2, 3, 0], [2, 3, 0, 1], [3, 0, 1, 2]], 4),
        ("USE", [[0, 3, 2, 1], [1, 2, 3, 0], [2, 1, 0, 3], [3, 0, 1, 2]], 4),
        ("RES", [[3, 0, 1, 0], [0, 1, 2, 3], [1, 0, 3, 2], [2, 3, 0, 1]], 4),
        ("ESR", [[3, 0, 1, 0], [0, 1, 2, 3], [1, 2, 3, 2], [2, 3, 0, 1]], 4),
        ("CFE", [[0, 3, 0, 3], [1, 2, 1, 2], [0, 3, 0, 3], [1, 2, 1, 2]], 4),
        ("RIPPLE", [[0, 3, 0, 3], [1, 2, 1, 2], [2, 1, 2, 1], [3, 0, 3, 0]], 4),
        ("SRS", [[1, 2, 3, 0], [0, 3, 2, 1], [3, 0, 3, 2], [2, 1, 0, 1]], 4),
        ("BANCS", [[0, 2, 2, 1, 1, 0], [1, 1, 0, 0, 2, 2], [2, 0, 1, 2, 0, 1]], 3),
    ],
)
def test_clock_catalog_periods(name: str, period: list[list[int]], phases: int) -> None:
    """Check literal clock cutouts and translations, including negative positions."""
    layout = cartesian_gate_layout((1, 1), name.swapcase())
    assert layout.num_clocks() == phases
    assert layout.is_regularly_clocked()
    width, height = len(period), len(period[0])
    for x, column in enumerate(period):
        for y, expected in enumerate(column):
            for dx, dy in [(0, 0), (width, 0), (0, height), (-width, -height)]:
                assert layout.get_clock_number((x + dx, y + dy)) == expected, (name, x, y, dx, dy)


@pytest.mark.parametrize("phases", [3, 4])
def test_open_clock_overrides_wrap_and_preserve_other_zones(phases: int) -> None:
    layout = cartesian_gate_layout((4, 7), f"oPeN{phases}")
    assert layout.num_clocks() == phases
    assert not layout.is_regularly_clocked()
    assert [layout.get_clock_number((0, y)) for y in range(7)] == [0] * 7
    for y in range(7):
        layout.assign_clock_number((0, y), y)
    assert [layout.get_clock_number((0, y)) for y in range(7)] == [y % phases for y in range(7)]
    assert [layout.get_clock_number((1, y)) for y in range(7)] == [0] * 7


@pytest.mark.parametrize("name", ["OPEN", "COLUMNAR", "ROW", "2DDWAVE", "2DDWAVEHEX"])
def test_clock_phase_suffixes(name: str) -> None:
    assert cartesian_gate_layout((1, 1), name).num_clocks() == 4
    assert cartesian_gate_layout((1, 1), name + "3").num_clocks() == 3
    assert cartesian_gate_layout((1, 1), name + "4").num_clocks() == 4


@pytest.mark.parametrize(
    "name",
    [
        "",
        "Column",
        "Rows",
        "TwoDDWave",
        "2DDWave6",
        "SUE",
        "SER",
        "ERS",
        "CEF",
        "RPIPLE",
        "SSR",
        "BNCS",
        "USE3",
        "RES3",
        "ESR3",
        "CFE3",
        "RIPPLE3",
        "SRS3",
        "BANCS4",
        "3",
    ],
)
def test_invalid_clock_aliases(name: str) -> None:
    with pytest.raises(ValueError, match="clocking scheme"):
        cartesian_gate_layout((1, 1), name)


@pytest.mark.parametrize("phases", [3, 4])
@pytest.mark.parametrize("a", list(arrangement))
def test_hexagonal_clock_periods(a: arrangement, phases: int) -> None:
    layout = hexagonal_gate_layout(a, (1, 1), f"2dDwAvEhEx{phases}")
    assert layout.num_clocks() == phases
    assert layout.get_clocking_scheme_name() == "2DDWAVEHEX"
    odd = a in (arrangement.ODD_ROW, arrangement.ODD_COLUMN)
    row = a in (arrangement.ODD_ROW, arrangement.EVEN_ROW)
    # Each shifted pair shares a phase; odd arrangements advance on the first odd index.
    offsets = [0, 1, 1, 2, 2, 3, 3, 4] if odd else [0, 0, 1, 1, 2, 2, 3, 3]
    for fixed in range(phases):
        for shifted in range(2 * phases):
            expected = (fixed + offsets[shifted]) % phases
            for translation in (-2 * phases, 0, 2 * phases):
                position = (fixed, shifted + translation) if row else (shifted + translation, fixed)
                assert layout.get_clock_number(position) == expected, (a, phases, position)


@pytest.mark.parametrize("scheme", ["2DDWave", "USE"])
def test_clocked_neighbor_degrees(scheme: str) -> None:
    layout = cartesian_gate_layout((3, 3), scheme)
    for zone, wave, use, degree in [
        ((0, 0), (0, 2), (1, 1), 2),
        ((1, 0), (1, 2), (1, 2), 3),
        ((2, 0), (1, 1), (2, 0), 2),
        ((1, 1), (2, 2), (2, 2), 4),
        ((0, 2), (1, 1), (0, 2), 2),
        ((1, 2), (2, 1), (2, 1), 3),
        ((2, 2), (2, 0), (1, 1), 2),
    ]:
        assert (layout.in_degree(zone), layout.out_degree(zone)) == (wave if scheme == "2DDWave" else use)
        assert layout.degree(zone) == degree


def test_synchronization_delays_update_clocked_neighbors() -> None:
    layout = cartesian_gate_layout((3, 3), "2DDWave")
    layout.assign_synchronization_element((0, 0), 0)
    assert layout.num_se() == 0
    for zone, delay in [((0, 1), 1), ((1, 0), 1), ((1, 2), 2), ((2, 1), 2)]:
        layout.assign_synchronization_element(zone, delay)
        assert layout.get_synchronization_element(zone) == delay
        assert layout.is_synchronization_element(zone)
    assert layout.num_se() == 4
    expected = sorted(starmap(layout.coord, [(1, 0), (0, 1), (1, 2), (2, 1)]))
    assert sorted(layout.incoming_clocked_zones((1, 1))) == expected
    assert sorted(layout.outgoing_clocked_zones((1, 1))) == expected
    for zone, degree in [((0, 0), 2), ((1, 0), 3), ((1, 1), 4), ((2, 2), 2)]:
        assert (layout.in_degree(zone), layout.out_degree(zone), layout.degree(zone)) == (degree,) * 3
    clone = layout.clone()
    clone.assign_synchronization_element((0, 1), 0)
    assert clone.num_se() == 3
    assert not clone.is_synchronization_element((0, 1))
    assert layout.num_se() == 4


def test_tile_clock_zones_floor_negative_cells() -> None:
    layout = inml_layout((4, 4), "2DDWave", "zones", 2, 2)
    assert layout.get_clock_zone((3, 2)) == layout.coord(1, 1)
    assert layout.get_clock_zone((3, 2, 1)) == layout.coord(1, 1)
    assert [layout.get_clock_number(c) for c in [(0, 0), (1, 1), (2, 0), (0, 3), (3, 3), (4, 1), (4, 3), (4, 4)]] == [
        0,
        0,
        1,
        1,
        2,
        2,
        3,
        0,
    ]
    layout.set_tile_size_x(3)
    layout.set_tile_size_y(3)
    for cell, zone in [
        ((-1, -2), (-1, -1)),
        ((-3, -3), (-1, -1)),
        ((-4, 0), (-2, 0)),
        ((0, 0), (0, 0)),
        ((4, 7), (1, 2)),
    ]:
        assert layout.get_clock_zone(cell) == layout.coord(*zone)
    layout.assign_clock_number((-1, -1), 2)
    assert layout.get_clock_number((-1, -2)) == 2
    for setter in (layout.set_tile_size_x, layout.set_tile_size_y):
        with pytest.raises(ValueError, match="positive"):
            setter(0)
    assert (layout.get_tile_size_x(), layout.get_tile_size_y()) == (3, 3)


def test_clocked_predicates_follow_overrides_and_synchronization() -> None:
    layout = cartesian_gate_layout((3, 3), "2DDWave")
    assert layout.is_incoming_clocked((1, 1), (1, 0))
    assert layout.is_outgoing_clocked((1, 0), (1, 1))
    assert not layout.is_incoming_clocked((1, 1), (0, 0))
    assert not layout.is_outgoing_clocked((1, 1), (1, 1))
    layout.assign_clock_number((1, 0), 2)
    assert not layout.is_outgoing_clocked((0, 0), (1, 0))
    assert not layout.is_incoming_clocked((1, 1), (1, 0))
    assert not layout.is_regularly_clocked()
    layout.replace_clocking_scheme("2DDWave")
    layout.assign_synchronization_element((1, 1), 1)
    assert layout.is_regularly_clocked()
    assert layout.get_clock_number((1, 1)) == 2
    for neighbor in [(1, 0), (0, 1), (2, 1), (1, 2)]:
        assert layout.is_incoming_clocked((1, 1), neighbor)
        assert layout.is_outgoing_clocked(neighbor, (1, 1))
