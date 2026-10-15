# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""SiDB circuit design through the public Python interface."""

from __future__ import annotations

from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction.layouts import arrangement, cartesian_gate_layout, hexagonal_gate_layout
from mnt.pyfiction.sidb import sidb_layout, site_at_row
from mnt.pyfiction.sidb.generators import (
    design_sidb_gates_mode,
    on_the_fly_sidb_circuit_design,
)
from mnt.pyfiction.sidb.io import read_sqd_layout, write_sidb_layout_svg_to_string, write_sqd_layout

if TYPE_CHECKING:
    from pathlib import Path


@pytest.fixture
def and_circuit() -> hexagonal_gate_layout:
    """Return a placed AND circuit with two inputs and one output."""
    layout = hexagonal_gate_layout(arrangement.EVEN_ROW, (3, 3, 1), "ROW", "AND")
    first = layout.create_pi("a", (0, 0, 0))
    second = layout.create_pi("b", (1, 0, 0))
    gate = layout.create_and(first, second, (1, 1, 0))
    layout.create_po(gate, "f", (0, 2, 0))
    return layout


@pytest.mark.parametrize("per_gate", [False, True])
def test_circuit_timeout(and_circuit: hexagonal_gate_layout, *, per_gate: bool) -> None:
    """Circuit and nested gate budgets raise TimeoutError instead of returning a partial circuit."""
    options: dict[str, Any] = {}

    options["design_mode"] = design_sidb_gates_mode.QUICKCELL
    options["number_of_canvas_sidbs"] = 3
    if per_gate:
        options["design_gate_timeout"] = 0
    else:
        options["timeout"] = 0

    with pytest.raises(TimeoutError):
        on_the_fly_sidb_circuit_design(and_circuit, **options)

    assert and_circuit.num_pis() == 2
    assert and_circuit.num_pos() == 1
    gate = and_circuit.find_object((1, 1, 0))
    assert gate is not None
    assert and_circuit.is_and(gate)
    assert (options["design_gate_timeout"] if per_gate else options["timeout"]) == 0


@pytest.mark.parametrize("timeout", [-1, 2**64, 1.5])
def test_invalid_timeout(and_circuit: hexagonal_gate_layout, timeout: float) -> None:
    """The Python API accepts only unsigned 64-bit millisecond budgets."""
    with pytest.raises(TypeError):
        on_the_fly_sidb_circuit_design(and_circuit, timeout=timeout)  # ty: ignore[invalid-argument-type]
    with pytest.raises(TypeError):
        on_the_fly_sidb_circuit_design(and_circuit, design_gate_timeout=timeout)  # ty: ignore[invalid-argument-type]


@pytest.mark.parametrize("shift", [arrangement.ODD_COLUMN, arrangement.EVEN_COLUMN])
def test_column_arrangements(shift: arrangement) -> None:
    """Circuit design rejects flat-top layouts instead of dropping their north/south wires."""
    layout = hexagonal_gate_layout(shift, (1, 3, 1), "2DDWave")
    first = layout.create_pi("x", (0, 0, 0))
    wire = layout.create_buf(first, (0, 1, 0))
    layout.create_po(wire, "y", (0, 2, 0))

    with pytest.raises(ValueError, match="pointy-top"):
        on_the_fly_sidb_circuit_design(layout, timeout=1_000)


@pytest.mark.slow
def test_design_and_export(and_circuit: hexagonal_gate_layout, tmp_path: Path) -> None:
    """A real circuit produces SiDBs without modifying its gate-level input."""
    options: dict[str, Any] = {}

    options["design_mode"] = design_sidb_gates_mode.QUICKCELL
    options["number_of_canvas_sidbs"] = 3
    result = on_the_fly_sidb_circuit_design(and_circuit, **options)
    assert isinstance(result, sidb_layout)
    assert result.num_dots() > 0
    assert result.num_pis() > 0
    assert result.num_pos() > 0
    assert "<svg" in write_sidb_layout_svg_to_string(result)
    output = tmp_path / "and.sqd"
    write_sqd_layout(result, str(output))
    assert read_sqd_layout(str(output)).num_dots() == result.num_dots()
    assert and_circuit.num_pis() == 2
    assert and_circuit.num_pos() == 1
    gate = and_circuit.find_object((1, 1, 0))
    assert gate is not None
    assert and_circuit.is_and(gate)


def test_unsuccessful_design(and_circuit: hexagonal_gate_layout) -> None:
    """An insufficient canvas raises a useful error instead of returning a partial circuit."""
    options: dict[str, Any] = {}

    options["canvas"] = (site_at_row(24, 17), site_at_row(24, 17))
    options["number_of_canvas_sidbs"] = 2
    with pytest.raises(RuntimeError, match="Gate design was unsuccessful"):
        on_the_fly_sidb_circuit_design(and_circuit, **options)


def test_unsupported_gate() -> None:
    """A majority gate reports its unsupported type and tile."""
    layout = hexagonal_gate_layout(arrangement.EVEN_ROW, (2, 2, 1), "ROW")
    first = layout.create_pi("a", (0, 0, 0))
    layout.create_maj(first, first, first, (1, 1, 0))
    with pytest.raises(ValueError, match="Unsupported gate type at tile"):
        on_the_fly_sidb_circuit_design(layout)


def test_wrong_topology() -> None:
    """Cartesian layouts require hexagonalization before circuit design."""
    with pytest.raises(TypeError):
        on_the_fly_sidb_circuit_design(cartesian_gate_layout())  # ty: ignore[invalid-argument-type]  # deliberately wrong topology
