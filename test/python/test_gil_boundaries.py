# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Native work permits Python progress without moving Python operations outside the GIL."""

from __future__ import annotations

import sys
import threading
from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.layouts import cartesian_gate_layout, cartesian_layout
from mnt.pyfiction.networks import random_network
from mnt.pyfiction.physical_design import color_routing
from mnt.pyfiction.sidb.io import read_surface_defects
from mnt.pyfiction.synthesis import dynamic_truth_table

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path
    from typing import Any

    from mnt.pyfiction.sidb import sidb_layout


@pytest.mark.parametrize("kind", ["coordinates", "depth", "routing", "defects", "truth_table"])
def test_native_work_releases_the_gil(kind: str, tmp_path: Path) -> None:
    """Python threads run during native traversal, routing, parsing, and truth-table export.

    Args:
        kind: Native operation to exercise.
        tmp_path: Directory for the defect matrix.
    """
    if kind == "coordinates":
        layout = cartesian_layout((1000, 200))
        assert len(run_with_python_progress(layout.coordinates)) == 200_000
    elif kind == "depth":
        network = random_network(100, 50_000, 7)
        assert run_with_python_progress(network.depth) > 0
    elif kind == "routing":
        gate_layout = cartesian_gate_layout((251, 50), "2DDWave")
        objectives = []
        for y in range(50):
            source = gate_layout.create_pi(f"in{y}", (0, y))
            gate_layout.create_po(source, f"out{y}", (250, y))
            objectives.append(((0, y), (250, y), 0))

        def route() -> bool:
            """Route independent rows using the native path-coloring algorithm.

            Returns:
                Whether every objective was routed.
            """
            return color_routing(gate_layout, objectives, path_limit=1)

        assert run_with_python_progress(route) is True

    elif kind == "defects":
        path = tmp_path / "defects.txt"
        path.write_text(("[" + "1 " * 80 + "]\n") * 200, encoding="utf-8")

        def read_defects() -> sidb_layout:
            """Read a complete defect matrix through the native file reader.

            Returns:
                The layout containing the parsed defects.
            """
            return read_surface_defects(str(path))

        assert run_with_python_progress(read_defects).num_defects() == 16_000

    else:
        table = dynamic_truth_table(24)
        assert run_with_python_progress(table.to_binary) == "0" * (1 << 24)


def run_with_python_progress(operation: Callable[[], Any]) -> Any:
    """Check that another Python thread runs before the native operation returns.

    Args:
        operation: Native call with its arguments prepared.

    Returns:
        The native operation's result.
    """

    ready = threading.Event()
    start = threading.Event()
    refreshed = threading.Event()

    def refresh() -> None:
        """Run one Python update after the native operation starts."""
        ready.set()
        start.wait()
        refreshed.set()

    worker = threading.Thread(target=refresh)
    interval = sys.getswitchinterval()
    worker.start()
    ready.wait()
    try:
        # Suppress interpreter handoffs so progress requires a native GIL release.
        sys.setswitchinterval(10)
        start.set()
        result = operation()
        refreshed_during_operation = refreshed.is_set()
    finally:
        sys.setswitchinterval(interval)
        start.set()
        worker.join()

    assert refreshed_during_operation
    return result
