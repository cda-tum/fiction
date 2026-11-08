# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Fresh gate layouts for verification across exposed geometries."""

from __future__ import annotations

from functools import partial
from itertools import starmap
from typing import TYPE_CHECKING, TypeAlias, cast

import pytest

from mnt.pyfiction.layouts import (
    arrangement,
    cartesian_gate_layout,
    hexagonal_gate_layout,
    shifted_cartesian_gate_layout,
)

if TYPE_CHECKING:
    from collections.abc import Callable, Iterable

GateLayout: TypeAlias = cartesian_gate_layout | shifted_cartesian_gate_layout | hexagonal_gate_layout
"""Gate-layout types accepted by the verification bindings."""


def _assign_clocks(layout: cartesian_gate_layout, phases: Iterable[tuple[tuple[int, int], int]]) -> None:
    """Assign the explicit clock phases of a physical blueprint.

    Args:
        layout: Blueprint whose clock zones are assigned.
        phases: Coordinates and expected clock phases.
    """
    for point, phase in phases:
        layout.assign_clock_number(point, phase)


@pytest.fixture(
    params=[
        pytest.param(cartesian_gate_layout, id="cartesian"),
        *[
            pytest.param(partial(layout_type, alignment), id=f"{name}-{alignment.name.lower()}")
            for name, layout_type in [("shifted", shifted_cartesian_gate_layout), ("hexagonal", hexagonal_gate_layout)]
            for alignment in [
                arrangement.EVEN_ROW,
                arrangement.ODD_ROW,
                arrangement.EVEN_COLUMN,
                arrangement.ODD_COLUMN,
            ]
        ],
    ]
)
def make_layout(request: pytest.FixtureRequest) -> Callable[[tuple[int, int, int], str], GateLayout]:
    """Construct an independent layout with the selected geometry.

    Args:
        request: Parametrized layout constructor.

    Returns:
        A factory accepting an extent and clocking scheme.
    """
    return cast("Callable[[tuple[int, int, int], str], GateLayout]", request.param)


@pytest.fixture(
    params=["and_or", "and_not", "xor_maj", "or_not", "fanout", "crossing", "synchronization", "unbalanced"]
)
def reference_layout(request: pytest.FixtureRequest) -> tuple[cartesian_gate_layout, int, int, int]:
    """Build the small physical blueprints used by Catch2 timing and design-rule tests.

    Args:
        request: Blueprint name.

    Returns:
        The layout, expected path length, throughput denominator, and warning count.
    """
    kind = request.param
    if kind in {"and_or", "and_not"}:
        layout = cartesian_gate_layout((4, 2, 1), "OPEN")
        _assign_clocks(layout, [((2, 0), 0), ((1, 0), 1), ((0, 0), 2), ((1, 1), 0), ((2, 1), 1), ((3, 1), 2)])
        a = layout.create_pi("x1", (2, 0))
        b = layout.create_pi("x2", (1, 1))
        first = layout.create_and(a, b, (1, 0))
        second = layout.create_or(b, a, (2, 1)) if kind == "and_or" else layout.create_not(b, (2, 1))
        layout.create_po(first, "f1", (0, 0))
        layout.create_po(second, "f2", (3, 1))
        return layout, 3, 1, 0
    if kind == "xor_maj":
        layout = cartesian_gate_layout((4, 3, 1), "OPEN")
        for phase, points in [(0, [(2, 0), (1, 1), (3, 1)]), (1, [(1, 0), (2, 1)]), (2, [(0, 0), (2, 2)])]:
            for point in points:
                layout.assign_clock_number(point, phase)
        a = layout.create_pi("a", (1, 1))
        b = layout.create_pi("b", (2, 0))
        c = layout.create_pi("c", (3, 1))
        majority = layout.create_maj(a, b, c, (2, 1))
        xor = layout.create_xor(a, b, (1, 0))
        layout.create_po(majority, "f1", (2, 2))
        layout.create_po(xor, "f2", (0, 0))
        return layout, 3, 1, 1
    if kind == "or_not":
        layout = cartesian_gate_layout((3, 3, 1), "2DDWave")
        a = layout.create_pi("x1", (1, 0))
        b = layout.create_pi("x2", (0, 1))
        gate = layout.create_or(a, b, (1, 1))
        inverter = layout.create_not(gate, (1, 2))
        layout.create_po(inverter, "f1", (2, 2))
        return layout, 4, 1, 0
    if kind == "fanout":
        layout = cartesian_gate_layout((3, 3, 2), "OPEN")
        _assign_clocks(
            layout,
            [
                ((0, 0), 3),
                ((0, 1), 0),
                ((0, 2), 1),
                ((1, 0), 2),
                ((1, 1), 1),
                ((1, 2), 0),
                ((2, 0), 3),
                ((2, 1), 2),
                ((2, 2), 3),
            ],
        )
        a = layout.create_pi("x1", (0, 1))
        first = layout.create_buf(a, (1, 1))
        second = layout.create_buf(first, (2, 1))
        wire = layout.create_buf(second, (2, 2))
        layout.create_po(first, "f1", (1, 0))
        layout.create_po(second, "f2", (2, 0))
        layout.create_po(wire, "f3", (1, 2))
        return layout, 5, 1, 0
    if kind == "crossing":
        layout = cartesian_gate_layout((4, 3, 2), "2DDWave")
        a, b, c, d = list(starmap(layout.create_pi, [("x1", (1, 0)), ("x2", (0, 1)), ("x3", (2, 0)), ("x4", (0, 2))]))
        first = layout.create_and(a, b, (1, 1))
        upper = layout.create_buf(c, (2, 1))
        lower = layout.create_buf(d, (1, 2))
        second = layout.create_and(upper, lower, (2, 2))
        crossing = layout.create_buf(first, (2, 1, 1))
        layout.create_po(crossing, "f1", (3, 1))
        layout.create_po(second, "f2", (3, 2))
        return layout, 4, 1, 0
    if kind == "synchronization":
        layout = cartesian_gate_layout((3, 2, 1), "2DDWave")
        layout.assign_synchronization_element((1, 0), 1)
        a = layout.create_pi("x1", (0, 0))
        b = layout.create_pi("x2", (2, 0))
        first = layout.create_buf(a, (0, 1))
        second = layout.create_buf(b, (1, 0))
        gate = layout.create_or(first, second, (1, 1))
        layout.create_po(gate, "f1", (2, 1))
        return layout, 4, 1, 0
    layout = cartesian_gate_layout((4, 3, 1), "OPEN")
    _assign_clocks(layout, [((1, 0), 0), ((2, 0), 1), ((3, 0), 2), ((0, 2), 1), ((1, 2), 2), ((2, 1), 0), ((2, 2), 3)])
    a = layout.create_pi("x1", (0, 2))
    b = layout.create_pi("x2", (1, 0))
    first = layout.create_buf(a, (1, 2))
    second = layout.create_buf(first, (2, 2))
    third = layout.create_buf(second, (2, 1))
    gate = layout.create_and(third, b, (2, 0))
    layout.create_po(gate, "f1", (3, 0))
    return layout, 6, 2, 0
