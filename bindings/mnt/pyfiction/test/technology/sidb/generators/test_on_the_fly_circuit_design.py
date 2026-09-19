# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""SiDB circuit design through the public Python interface."""

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction import (
    cartesian_gate_layout,
    design_sidb_gates_mode,
    hexagonal_gate_layout,
    on_the_fly_sidb_circuit_design,
    on_the_fly_sidb_circuit_design_params,
    read_sqd_layout,
    sidb_complex_gate_design_policy,
    sidb_layout,
    sidb_on_the_fly_gate_library_params,
    site_at_row,
    termination_condition,
    write_sidb_layout_svg_to_string,
    write_sqd_layout,
)

if TYPE_CHECKING:
    from pathlib import Path


@pytest.fixture
def and_circuit() -> hexagonal_gate_layout:
    """Return a placed AND circuit with two inputs and one output."""
    layout = hexagonal_gate_layout((2, 2, 0), "ROW", "AND")
    first = layout.create_pi("a", (0, 0, 0))
    second = layout.create_pi("b", (1, 0, 0))
    gate = layout.create_and(first, second, (1, 1, 0))
    layout.create_po(gate, "f", (0, 2, 0))
    return layout


def test_parameters() -> None:
    """Circuit parameters retain the native defaults and writable nested fields."""
    params = on_the_fly_sidb_circuit_design_params()
    library = params.sidb_on_the_fly_gate_library_parameters
    assert params.timeout == 2**64 - 1
    assert library.design_gate_params.timeout == 2**64 - 1
    assert isinstance(library, sidb_on_the_fly_gate_library_params)
    assert library.design_gate_params.design_mode == design_sidb_gates_mode.AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER
    assert library.design_gate_params.number_of_canvas_sidbs == 1
    assert library.design_gate_params.termination_cond == termination_condition.AFTER_FIRST_SOLUTION
    assert library.canvas_sidb_complex_gates == 3
    assert (
        library.using_predefined_crossing_and_double_wire_if_possible
        == sidb_complex_gate_design_policy.USING_PREDEFINED
    )
    assert library.influence_radius_charged_defects == 15

    library.design_gate_params.number_of_canvas_sidbs = 2
    library.canvas_sidb_complex_gates = 4
    library.using_predefined_crossing_and_double_wire_if_possible = sidb_complex_gate_design_policy.DESIGN_ON_THE_FLY
    library.influence_radius_charged_defects = 10
    assert params.sidb_on_the_fly_gate_library_parameters.design_gate_params.number_of_canvas_sidbs == 2
    assert library.canvas_sidb_complex_gates == 4
    assert (
        library.using_predefined_crossing_and_double_wire_if_possible
        == sidb_complex_gate_design_policy.DESIGN_ON_THE_FLY
    )
    assert library.influence_radius_charged_defects == 10


@pytest.mark.parametrize("per_gate", [False, True])
def test_circuit_timeout(and_circuit: hexagonal_gate_layout, *, per_gate: bool) -> None:
    """Circuit and nested gate budgets raise TimeoutError instead of returning a partial circuit."""
    params = on_the_fly_sidb_circuit_design_params()
    gates = params.sidb_on_the_fly_gate_library_parameters.design_gate_params
    gates.design_mode = design_sidb_gates_mode.QUICKCELL
    gates.number_of_canvas_sidbs = 3
    if per_gate:
        gates.timeout = 0
    else:
        params.timeout = 0

    with pytest.raises(TimeoutError):
        on_the_fly_sidb_circuit_design(and_circuit, params)

    assert and_circuit.num_pis() == 2
    assert and_circuit.num_pos() == 1
    assert and_circuit.is_and(and_circuit.get_node((1, 1, 0)))
    assert (gates.timeout if per_gate else params.timeout) == 0


@pytest.mark.parametrize("timeout", [-1, 2**64, 1.5])
def test_invalid_timeout(timeout: float) -> None:
    """The Python API accepts only unsigned 64-bit millisecond budgets."""
    params = on_the_fly_sidb_circuit_design_params()
    with pytest.raises(TypeError):
        params.timeout = timeout
    with pytest.raises(TypeError):
        params.sidb_on_the_fly_gate_library_parameters.design_gate_params.timeout = timeout


@pytest.mark.slow
def test_design_and_export(and_circuit: hexagonal_gate_layout, tmp_path: Path) -> None:
    """A real circuit produces SiDBs without modifying its gate-level input."""
    params = on_the_fly_sidb_circuit_design_params()
    gates = params.sidb_on_the_fly_gate_library_parameters.design_gate_params
    gates.design_mode = design_sidb_gates_mode.QUICKCELL
    gates.number_of_canvas_sidbs = 3
    result = on_the_fly_sidb_circuit_design(and_circuit, params)
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
    assert and_circuit.is_and(and_circuit.get_node((1, 1, 0)))


def test_unsuccessful_design(and_circuit: hexagonal_gate_layout) -> None:
    """An insufficient canvas raises a useful error instead of returning a partial circuit."""
    params = on_the_fly_sidb_circuit_design_params()
    gates = params.sidb_on_the_fly_gate_library_parameters.design_gate_params
    gates.canvas = (site_at_row(24, 17), site_at_row(24, 17))
    gates.number_of_canvas_sidbs = 2
    with pytest.raises(RuntimeError, match="Gate design was unsuccessful"):
        on_the_fly_sidb_circuit_design(and_circuit, params)


def test_unsupported_gate() -> None:
    """A majority gate reports its unsupported type and tile."""
    layout = hexagonal_gate_layout((1, 1, 0), "ROW")
    layout.create_maj(0, 0, 0, (1, 1, 0))
    with pytest.raises(ValueError, match="Unsupported gate type at tile"):
        on_the_fly_sidb_circuit_design(layout)


def test_wrong_topology() -> None:
    """Cartesian layouts require hexagonalization before circuit design."""
    with pytest.raises(TypeError):
        on_the_fly_sidb_circuit_design(cartesian_gate_layout())
