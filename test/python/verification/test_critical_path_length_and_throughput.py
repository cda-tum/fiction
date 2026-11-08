# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for timing of declared physical paths."""

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.layouts import cartesian_gate_layout, layout_input_port
from mnt.pyfiction.verification import critical_path_length_and_throughput

if TYPE_CHECKING:
    from collections.abc import Callable

    from .conftest import GateLayout


def test_timing_counts_wires_and_rejects_required_topology_defects() -> None:
    """Dangling cycles are ignored; required holes and cycles raise ValueError."""
    layout = cartesian_gate_layout((2, 3, 1), "2DDWave")
    pi = layout.create_pi("a", (0, 0))
    wire = layout.create_buf(pi, (0, 1))
    po = layout.create_po(wire, "result", (0, 2))
    dangling = layout.create_buf(pi, (1, 0))
    layout.connect(dangling, layout_input_port(dangling, 0))
    assert critical_path_length_and_throughput(layout) == (3, 1)
    layout.disconnect(layout_input_port(wire, 0))
    with pytest.raises(ValueError, match="disconnected input"):
        critical_path_length_and_throughput(layout)
    layout.connect(po, layout_input_port(wire, 0))
    with pytest.raises(ValueError, match="cycle"):
        critical_path_length_and_throughput(layout)


@pytest.mark.parametrize("length", [1, 4])
def test_declared_paths_ignore_placement(
    make_layout: Callable[[tuple[int, int, int], str], GateLayout], length: int
) -> None:
    """Declared paths count out-of-frame wires and full clock-cycle branch differences."""
    layout = make_layout((1, 1, 1), "OPEN")
    source = layout.create_pi("a", (10, 0))
    path = source
    for x in range(11, 11 + length):
        path = layout.create_buf(path, (x, 0))
    gate = layout.create_and(source, path, (11 + length, 0))
    layout.create_po(gate, "result", (12 + length, 0))
    assert critical_path_length_and_throughput(layout) == (length + 3, 1 + length // 4)


def test_reference_layout_timing(reference_layout: tuple[cartesian_gate_layout, int, int, int]) -> None:
    """Balanced gates, crossings, fanout, synchronization, and unbalanced paths retain their timing."""
    layout, length, throughput, _warnings = reference_layout
    assert critical_path_length_and_throughput(layout) == (length, throughput)


def test_long_route_with_inversion_and_branch() -> None:
    """A deep route with an inverter and a shorter branch retains the longest path."""
    length = 10_000
    layout = cartesian_gate_layout((length + 1, 2, 1), "2DDWave")
    signal = layout.create_pi("in", (0, 0))
    branch = signal
    for x in range(1, length):
        if x == length // 2:
            signal = layout.create_not(signal, (x, 0))
            branch = signal
        else:
            signal = layout.create_buf(signal, (x, 0))
    layout.create_po(signal, "out", (length, 0))
    layout.create_po(branch, "branch", (length // 2, 1))
    assert critical_path_length_and_throughput(layout) == (length + 1, 1)
