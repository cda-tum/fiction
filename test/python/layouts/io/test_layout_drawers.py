# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of the Graphviz DOT drawer for every gate-level layout topology."""

from __future__ import annotations

import re
from functools import partial
from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction.layouts import (
    arrangement,
    cartesian_gate_layout,
    hexagonal_gate_layout,
    shifted_cartesian_gate_layout,
)
from mnt.pyfiction.layouts.io import write_dot_layout

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path


@pytest.mark.parametrize(
    ("layout_type", "attributes"),
    [
        pytest.param(cartesian_gate_layout, ("shape=square", "rankdir=TB"), id="cartesian"),
        pytest.param(
            partial(shifted_cartesian_gate_layout, arrangement.ODD_COLUMN),
            ("shape=square", "rankdir=LR"),
            id="shifted",
        ),
        pytest.param(
            partial(hexagonal_gate_layout, arrangement.EVEN_ROW),
            ("shape=hexagon", "orientation=30", "rankdir=TB"),
            id="hexagonal",
        ),
    ],
)
def test_write_dot_layout_draws_every_topology(
    tmp_path: Path, layout_type: Callable[..., Any], attributes: tuple[str, ...]
) -> None:
    """DOT preserves gate labels, all connections, and each topology's drawing attributes."""
    layout = layout_type((3, 3, 2), "2DDWave", "drawn")
    x1 = layout.create_pi("a", (0, 0))
    x2 = layout.create_pi("b", (1, 0))
    layout.create_po(layout.create_and(x1, x2, (1, 1)), "f", (2, 1))
    file = tmp_path / "layout.dot"
    write_dot_layout(layout, str(file))
    text = file.read_text(encoding="utf-8")
    labels = re.findall(r'(\w+) \[label="([^"]+)"', text)
    assert sorted(label for _, label in labels) == ["AND", "a", "b", "f"]
    node_labels = dict(labels)
    assert len(node_labels) == 4
    edges = re.findall(r"(\w+) -> (\w+) \[style=solid\];", text)
    assert sorted((node_labels[source], node_labels[target]) for source, target in edges) == [
        ("AND", "f"),
        ("a", "AND"),
        ("b", "AND"),
    ]
    for attribute in attributes:
        assert attribute in text
