# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of the Graphviz DOT drawer for every gate-level layout topology."""

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction import (
    cartesian_gate_layout,
    hexagonal_gate_layout,
    shifted_cartesian_gate_layout,
    write_dot_layout,
)

if TYPE_CHECKING:
    from pathlib import Path


@pytest.mark.parametrize(
    "layout_type",
    [cartesian_gate_layout, shifted_cartesian_gate_layout, hexagonal_gate_layout],
)
def test_write_dot_layout_draws_every_topology(tmp_path: Path, layout_type: type) -> None:
    layout = layout_type((2, 2, 1), "2DDWave", "drawn")
    x1 = layout.create_pi("a", (0, 0))
    x2 = layout.create_pi("b", (1, 0))
    layout.create_po(layout.create_and(x1, x2, (1, 1)), "f", (2, 1))
    file = tmp_path / "layout.dot"
    write_dot_layout(layout, str(file))
    text = file.read_text(encoding="utf-8")
    assert "graph" in text
    assert "a" in text


def test_clone_is_independent(mux21_layout: cartesian_gate_layout) -> None:
    copy = mux21_layout.clone()
    assert copy.num_gates() == mux21_layout.num_gates()
    copy.set_layout_name("copy")
    assert mux21_layout.get_layout_name() != "copy"
