# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for timing of declared physical paths."""

from __future__ import annotations

import pytest

from mnt.pyfiction.layouts import LayoutInputPort, cartesian_gate_layout
from mnt.pyfiction.verification import critical_path_length_and_throughput


def test_timing_counts_wires_and_rejects_required_topology_defects() -> None:
    layout = cartesian_gate_layout((2, 3, 1), "2DDWave")
    pi = layout.create_pi("a", (0, 0))
    wire = layout.create_buf(pi, (0, 1))
    po = layout.create_po(wire, "result", (0, 2))
    dangling = layout.create_buf(pi, (1, 0))
    layout.connect(dangling, LayoutInputPort(dangling.object, 0))
    assert critical_path_length_and_throughput(layout) == (3, 1)
    layout.disconnect(LayoutInputPort(wire.object, 0))
    with pytest.raises(ValueError, match="disconnected input"):
        critical_path_length_and_throughput(layout)
    layout.connect(po, LayoutInputPort(wire.object, 0))
    with pytest.raises(ValueError, match="cycle"):
        critical_path_length_and_throughput(layout)
