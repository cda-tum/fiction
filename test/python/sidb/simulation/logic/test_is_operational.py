# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import Any

import pytest

from mnt.pyfiction.sidb import lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.io import read_sqd_layout
from mnt.pyfiction.sidb.model import sidb_defect, sidb_defect_type
from mnt.pyfiction.sidb.simulation import sidb_simulation_engine
from mnt.pyfiction.sidb.simulation.logic import (
    bdl_wire,
    bdl_wire_selection,
    detect_bdl_wires,
    generate_bdl_input_pattern_layouts,
    is_kink_induced_non_operational,
    is_operational,
    kink_induced_non_operational_input_patterns,
    operational_analysis_strategy,
    operational_condition,
    operational_input_patterns,
    operational_status,
)
from mnt.pyfiction.synthesis import create_and_tt, create_id_tt


def test_is_operational():
    lyt = sidb_layout()

    lyt.assign_sidb(lattice_site(0, 0, 1), sidb_dot_tag.INPUT)
    lyt.assign_sidb(lattice_site(2, 1, 1), sidb_dot_tag.INPUT)

    lyt.assign_sidb(lattice_site(20, 0, 1), sidb_dot_tag.INPUT)
    lyt.assign_sidb(lattice_site(19, 1, 1), sidb_dot_tag.INPUT)

    lyt.assign_sidb(lattice_site(4, 2, 1), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(6, 3, 1), sidb_dot_tag.NORMAL)

    lyt.assign_sidb(lattice_site(14, 3, 1), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(16, 2, 1), sidb_dot_tag.NORMAL)

    lyt.assign_sidb(lattice_site(10, 6, 0), sidb_dot_tag.OUTPUT)
    lyt.assign_sidb(lattice_site(10, 7, 0), sidb_dot_tag.OUTPUT)

    lyt.assign_sidb(lattice_site(10, 9, 1), sidb_dot_tag.NORMAL)

    options: dict[str, Any] = {"base": 2, "mu_minus": -0.28, "epsilon_r": 5.6, "lambda_tf": 5.0}

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], **options)

    assert op_status == operational_status.OPERATIONAL

    options.update({"base": 2, "mu_minus": -0.1, "epsilon_r": 5.6, "lambda_tf": 5.0})

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], **options)

    assert op_status == operational_status.NON_OPERATIONAL

    # pre-determined I/O pins
    output_bdl_wires = detect_bdl_wires(lyt, wire_selection=bdl_wire_selection.OUTPUT)
    input_bdl_wires = detect_bdl_wires(lyt, wire_selection=bdl_wire_selection.INPUT)
    [op_status, _evaluated_input_combinations] = is_operational(
        lyt, [create_and_tt()], input_bdl_wires, output_bdl_wires, **options
    )
    assert op_status == operational_status.NON_OPERATIONAL

    # pre-determined I/O pins and canvas layout
    canvas_lyt = sidb_layout()
    canvas_lyt.assign_sidb(lattice_site(4, 2, 1), sidb_dot_tag.LOGIC)
    canvas_lyt.assign_sidb(lattice_site(6, 3, 1), sidb_dot_tag.LOGIC)
    canvas_lyt.assign_sidb(lattice_site(1000, 0, 0), sidb_dot_tag.LOGIC)
    options.update(
        mu_minus=-0.28,
        op_condition=operational_condition.REJECT_KINKS,
        strategy_to_analyze_operational_status=operational_analysis_strategy.FILTER_ONLY,
    )
    with pytest.raises(ValueError, match="canvas SiDB is not part of the layout"):
        is_operational(lyt, [create_and_tt()], input_bdl_wires, output_bdl_wires, canvas_lyt=canvas_lyt, **options)


@pytest.fixture
def and_gate_with_bdl_wires():
    """A 100-lattice AND gate together with its detected input and output BDL wires.

    Returns:
        The layout, its input BDL wires, and its output BDL wires.
    """
    lyt = sidb_layout()

    lyt.assign_sidb(lattice_site(0, 0, 1), sidb_dot_tag.INPUT)
    lyt.assign_sidb(lattice_site(2, 1, 1), sidb_dot_tag.INPUT)

    lyt.assign_sidb(lattice_site(20, 0, 1), sidb_dot_tag.INPUT)
    lyt.assign_sidb(lattice_site(19, 1, 1), sidb_dot_tag.INPUT)

    lyt.assign_sidb(lattice_site(4, 2, 1), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(6, 3, 1), sidb_dot_tag.NORMAL)

    lyt.assign_sidb(lattice_site(14, 3, 1), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(16, 2, 1), sidb_dot_tag.NORMAL)

    lyt.assign_sidb(lattice_site(10, 6, 0), sidb_dot_tag.OUTPUT)
    lyt.assign_sidb(lattice_site(10, 7, 0), sidb_dot_tag.OUTPUT)

    lyt.assign_sidb(lattice_site(10, 9, 1), sidb_dot_tag.NORMAL)

    return (
        lyt,
        detect_bdl_wires(lyt, wire_selection=bdl_wire_selection.INPUT),
        detect_bdl_wires(lyt, wire_selection=bdl_wire_selection.OUTPUT),
    )


def test_generate_bdl_input_pattern_layouts(and_gate_with_bdl_wires):
    lyt, input_bdl_wires, _output_bdl_wires = and_gate_with_bdl_wires

    input_pattern_layouts = generate_bdl_input_pattern_layouts(lyt, input_bdl_wires)

    # a 2-input gate has 4 input patterns
    assert len(input_pattern_layouts) == 4


@pytest.mark.parametrize(
    ("mu_minus", "expected"),
    [
        pytest.param(-0.28, operational_status.OPERATIONAL, id="operational"),
        pytest.param(-0.1, operational_status.NON_OPERATIONAL, id="non_operational"),
    ],
)
def test_input_pattern_layouts_yield_the_same_verdict(and_gate_with_bdl_wires, mu_minus, expected):
    lyt, input_bdl_wires, output_bdl_wires = and_gate_with_bdl_wires

    input_pattern_layouts = generate_bdl_input_pattern_layouts(lyt, input_bdl_wires)

    options: dict[str, Any] = {"base": 2, "mu_minus": mu_minus, "epsilon_r": 5.6, "lambda_tf": 5.0}

    [reference_status, reference_calls] = is_operational(
        lyt, [create_and_tt()], input_bdl_wires, output_bdl_wires, **options
    )
    [op_status, evaluated_input_combinations] = is_operational(
        input_pattern_layouts, [create_and_tt()], input_bdl_wires, output_bdl_wires, **options
    )

    assert reference_status == expected
    assert op_status == reference_status
    assert evaluated_input_combinations == reference_calls


def test_a_layout_list_that_does_not_match_the_specification_is_rejected(and_gate_with_bdl_wires):
    lyt, input_bdl_wires, output_bdl_wires = and_gate_with_bdl_wires

    input_pattern_layouts = generate_bdl_input_pattern_layouts(lyt, input_bdl_wires)

    with pytest.raises(ValueError, match="expected 4 input pattern layouts"):
        is_operational(input_pattern_layouts[:2], [create_and_tt()], input_bdl_wires, output_bdl_wires)


def test_and_gate_kinks(resources_dir):
    lyt = read_sqd_layout(str(resources_dir / "AND_mu_032_kinks.sqd"))

    options: dict[str, Any] = {"base": 2, "mu_minus": -0.32, "epsilon_r": 5.6, "lambda_tf": 5.0}

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], **options)

    assert op_status == operational_status.OPERATIONAL

    options["op_condition"] = operational_condition.REJECT_KINKS

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], **options)

    assert op_status == operational_status.NON_OPERATIONAL


def test_and_gate_non_operational_due_to_kinks(resources_dir):
    lyt = read_sqd_layout(str(resources_dir / "AND_mu_032_kinks.sqd"))

    result = is_kink_induced_non_operational(
        lyt, [create_and_tt()], base=2, mu_minus=-0.32, epsilon_r=5.6, lambda_tf=5.0
    )

    assert result


def test_and_gate_non_operational_input_patterns_due_to_kinks(resources_dir):
    lyt = read_sqd_layout(str(resources_dir / "AND_mu_032_kinks.sqd"))

    non_operational_pattern_kinks = kink_induced_non_operational_input_patterns(
        lyt, [create_and_tt()], base=2, mu_minus=-0.32, epsilon_r=5.6, lambda_tf=5.0
    )

    assert non_operational_pattern_kinks == {1, 2}


def test_and_gate_111_lattice_11_input_pattern(resources_dir):
    lyt = read_sqd_layout(str(resources_dir / "AND_mu_032_111_surface.sqd"))

    options: dict[str, Any] = {"base": 2, "mu_minus": -0.32, "epsilon_r": 5.6, "lambda_tf": 5.0}

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], **options)

    assert op_status == operational_status.OPERATIONAL

    options.update({"base": 2, "mu_minus": -0.1, "epsilon_r": 5.6, "lambda_tf": 5.0})

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], **options)

    assert op_status == operational_status.NON_OPERATIONAL

    # filer only
    options["strategy_to_analyze_operational_status"] = operational_analysis_strategy.FILTER_ONLY

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], **options)
    assert op_status == operational_status.NON_OPERATIONAL

    # filer then simulation
    options["strategy_to_analyze_operational_status"] = operational_analysis_strategy.FILTER_THEN_SIMULATION

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], **options)
    assert op_status == operational_status.NON_OPERATIONAL


def test_and_gate_111_lattice_operational_input_pattern(resources_dir):
    lyt = read_sqd_layout(str(resources_dir / "AND_mu_032_111_surface.sqd"))

    operational_patterns = operational_input_patterns(
        lyt, [create_and_tt()], base=2, mu_minus=-0.30, epsilon_r=5.6, lambda_tf=5.0
    )

    assert len(operational_patterns) == 2

    assert operational_patterns == {0, 3}


@pytest.mark.parametrize("mixed_variables", [False, True])
def test_invalid_specification_raises_value_error(
    and_gate_with_bdl_wires: tuple[sidb_layout, list[bdl_wire], list[bdl_wire]], *, mixed_variables: bool
) -> None:
    lyt, input_wires, output_wires = and_gate_with_bdl_wires
    spec = [create_and_tt(), create_id_tt()] if mixed_variables else []
    layouts = generate_bdl_input_pattern_layouts(lyt, input_wires)
    for check in (
        is_operational,
        operational_input_patterns,
        kink_induced_non_operational_input_patterns,
        is_kink_induced_non_operational,
    ):
        with pytest.raises(ValueError, match="variables" if mixed_variables else "empty"):
            check(lyt, spec)
    with pytest.raises(ValueError, match="variables" if mixed_variables else "empty"):
        is_operational(layouts, spec, input_wires, output_wires)


def test_quicksim_rejects_charged_defects(
    and_gate_with_bdl_wires: tuple[sidb_layout, list[bdl_wire], list[bdl_wire]],
) -> None:
    lyt, input_wires, output_wires = and_gate_with_bdl_wires
    lyt.assign_defect(lattice_site(100, 100, 0), sidb_defect(sidb_defect_type.DB, -1))
    layouts = generate_bdl_input_pattern_layouts(lyt, input_wires)
    for check in (is_operational, operational_input_patterns):
        with pytest.raises(ValueError, match="QuickSim does not support charged defects"):
            check(lyt, [create_and_tt()], sim_engine=sidb_simulation_engine.QUICKSIM, mu_minus=-0.28)
    with pytest.raises(ValueError, match="QuickSim does not support charged defects"):
        is_operational(
            layouts,
            [create_and_tt()],
            input_wires,
            output_wires,
            sim_engine=sidb_simulation_engine.QUICKSIM,
            mu_minus=-0.28,
        )


def test_quicksim_rejects_base_three(
    and_gate_with_bdl_wires: tuple[sidb_layout, list[bdl_wire], list[bdl_wire]],
) -> None:
    lyt, input_wires, output_wires = and_gate_with_bdl_wires
    layouts = generate_bdl_input_pattern_layouts(lyt, input_wires)
    for check in (is_operational, operational_input_patterns):
        with pytest.raises(ValueError, match=r"QuickSim.*base"):
            check(lyt, [create_and_tt()], sim_engine=sidb_simulation_engine.QUICKSIM, base=3, mu_minus=-0.28)
    with pytest.raises(ValueError, match=r"QuickSim.*base"):
        is_operational(
            layouts,
            [create_and_tt()],
            input_wires,
            output_wires,
            sim_engine=sidb_simulation_engine.QUICKSIM,
            base=3,
            mu_minus=-0.28,
        )
