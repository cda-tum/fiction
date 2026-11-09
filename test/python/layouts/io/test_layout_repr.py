# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import re
from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.layouts import (
    arrangement,
    cartesian_gate_layout,
    hexagonal_gate_layout,
    shifted_cartesian_gate_layout,
)
from mnt.pyfiction.qca import qca_cell_mode, qca_cell_type, qca_layout

if TYPE_CHECKING:
    from collections.abc import Callable


def test_empty_gate_layout_representation(gate_factory: Callable) -> None:
    assert repr(gate_factory((3, 3))) == "[i] empty layout\n"


@pytest.mark.parametrize(
    ("make_layout", "message"),
    [
        (lambda: hexagonal_gate_layout(arrangement.EVEN_ROW, (1, 1)), "hexagonal"),
        (lambda: shifted_cartesian_gate_layout(arrangement.EVEN_ROW, (1, 1)), "shifted cartesian"),
    ],
)
def test_noncartesian_gate_representation_reports_unsupported(make_layout: Callable, message: str) -> None:
    layout = make_layout()
    layout.create_pi("a", (0, 0))
    assert repr(layout) == f"[e] {message} layout printing is not supported\n"


def test_gate_representation_reports_symbols_and_clocked_connections() -> None:
    layout = cartesian_gate_layout((4, 2), "OPEN")
    a = layout.create_pi("a", (2, 0))
    b = layout.create_pi("b", (1, 1))
    gate = layout.create_and(a, b, (1, 0))
    inverter = layout.create_not(b, (2, 1))
    layout.create_po(gate, "f", (0, 0))
    layout.create_po(inverter, "g", (3, 1))
    assert "\x1b[" in repr(layout)
    assert re.sub(r"\x1b\[[0-9;]*m", "", repr(layout)) == "        \nO & I ▢ \n        \n▢ I ¬ O \n\n"
    for position, phase in [((2, 0), 0), ((1, 0), 1), ((0, 0), 2), ((1, 1), 0), ((2, 1), 1), ((3, 1), 2)]:
        layout.assign_clock_number(position, phase)
    assert re.sub(r"\x1b\[[0-9;]*m", "", repr(layout)) == "        \nO←&←I ▢ \n  ↑     \n▢ I→¬→O \n\n"


def test_qca_representation_contains_cells_and_crossings() -> None:
    layout = qca_layout((5, 5, 2))
    assert repr(layout) == "[i] empty layout\n"
    for position, kind in [
        ((0, 2), qca_cell_type.INPUT),
        ((2, 0), qca_cell_type.INPUT),
        ((1, 2), qca_cell_type.NORMAL),
        ((2, 2), qca_cell_type.NORMAL),
        ((3, 2), qca_cell_type.NORMAL),
        ((2, 4), qca_cell_type.OUTPUT),
        ((4, 2), qca_cell_type.OUTPUT),
    ]:
        layout.assign_cell_type(position, kind)
    for y in range(1, 4):
        layout.assign_cell_type((2, y, 1), qca_cell_type.NORMAL)
        layout.assign_cell_mode((2, y, 1), qca_cell_mode.CROSSOVER)
    assert re.sub(r"\x1b\[[0-9;]*m", "", repr(layout)) == "  i  \n  x  \ni▢x▢o\n  x  \n  o  \n\n"


def test_terminal_only_representation() -> None:
    layout = cartesian_gate_layout((2, 1))
    source = layout.create_pi("a", (0, 0))
    layout.create_po(source, "f", (1, 0))
    assert "I O" in re.sub(r"\x1b\[[0-9;]*m", "", repr(layout))


def test_gate_representation_keeps_symbols_and_row_order() -> None:
    layout = cartesian_gate_layout((4, 3), "OPEN")
    a, b, c = [layout.create_pi(name, (0, y)) for y, name in enumerate(("a", "b", "c"))]
    or_gate = layout.create_or(a, b, (1, 0))
    majority = layout.create_maj(a, b, c, (1, 1))
    xor_gate = layout.create_xor(or_gate, majority, (1, 2))
    fanout = layout.create_buf(xor_gate, (2, 1))
    layout.create_po(fanout, "f", (3, 0))
    layout.create_po(fanout, "g", (3, 1))
    plain = re.sub(r"\x1b\[[0-9;]*m", "", repr(layout))
    for symbol in ("|", "M", "X", "F"):
        assert symbol in plain
    assert plain.index("|") < plain.index("M") < plain.index("X")


def test_gate_crossing_representation() -> None:
    layout = cartesian_gate_layout((2, 1, 2), "OPEN")
    layout.create_buf((0, 0))
    layout.create_buf((0, 0, 1))
    layout.create_buf((1, 0))
    assert "+ =" in re.sub(r"\x1b\[[0-9;]*m", "", repr(layout))


def test_qca_and_representation() -> None:
    layout = qca_layout((5, 5))
    for position, kind in [
        ((0, 2), qca_cell_type.INPUT),
        ((2, 4), qca_cell_type.INPUT),
        ((2, 0), qca_cell_type.CONST_0),
        ((4, 2), qca_cell_type.OUTPUT),
        ((2, 1), qca_cell_type.NORMAL),
        ((2, 2), qca_cell_type.NORMAL),
        ((2, 3), qca_cell_type.NORMAL),
        ((1, 2), qca_cell_type.NORMAL),
        ((3, 2), qca_cell_type.NORMAL),
    ]:
        layout.assign_cell_type(position, kind)
    assert re.sub(r"\x1b\[[0-9;]*m", "", repr(layout)) == "  0  \n  ▢  \ni▢▢▢o\n  ▢  \n  i  \n\n"
