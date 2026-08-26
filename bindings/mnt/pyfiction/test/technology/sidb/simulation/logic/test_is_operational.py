# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction import (
    bdl_input_iterator_params,
    bdl_wire_selection,
    create_and_tt,
    detect_bdl_wires_100,
    detect_bdl_wires_params,
    generate_bdl_input_pattern_layouts,
    is_kink_induced_non_operational,
    is_operational,
    is_operational_params,
    kink_induced_non_operational_input_patterns,
    operational_analysis_strategy,
    operational_condition,
    operational_input_patterns,
    operational_status,
    read_sqd_layout_100,
    read_sqd_layout_111,
    sidb_100_lattice,
    sidb_simulation_parameters,
    sidb_technology,
)


def test_is_operational():
    lyt = sidb_100_lattice()

    lyt.assign_cell_type((0, 1), sidb_technology.cell_type.INPUT)
    lyt.assign_cell_type((2, 3), sidb_technology.cell_type.INPUT)

    lyt.assign_cell_type((20, 1), sidb_technology.cell_type.INPUT)
    lyt.assign_cell_type((19, 3), sidb_technology.cell_type.INPUT)

    lyt.assign_cell_type((4, 5), sidb_technology.cell_type.NORMAL)
    lyt.assign_cell_type((6, 7), sidb_technology.cell_type.NORMAL)

    lyt.assign_cell_type((14, 7), sidb_technology.cell_type.NORMAL)
    lyt.assign_cell_type((16, 5), sidb_technology.cell_type.NORMAL)

    lyt.assign_cell_type((10, 12, 0), sidb_technology.cell_type.OUTPUT)
    lyt.assign_cell_type((10, 14, 0), sidb_technology.cell_type.OUTPUT)

    lyt.assign_cell_type((10, 19), sidb_technology.cell_type.NORMAL)

    params = is_operational_params()
    params.simulation_parameters = sidb_simulation_parameters(2, -0.28)

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], params)

    assert op_status == operational_status.OPERATIONAL

    params.simulation_parameters = sidb_simulation_parameters(2, -0.1)

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], params)

    assert op_status == operational_status.NON_OPERATIONAL

    # pre-determined I/O pins
    output_bdl_wires = detect_bdl_wires_100(lyt, detect_bdl_wires_params(), bdl_wire_selection.OUTPUT)
    input_bdl_wires = detect_bdl_wires_100(lyt, detect_bdl_wires_params(), bdl_wire_selection.INPUT)
    [op_status, _evaluated_input_combinations] = is_operational(
        lyt,
        [create_and_tt()],
        params,
        input_bdl_wires,
        output_bdl_wires,
    )
    assert op_status == operational_status.NON_OPERATIONAL

    # pre-determined I/O pins and canvas layout
    canvas_lyt = sidb_100_lattice()
    canvas_lyt.assign_cell_type((4, 5), sidb_technology.cell_type.LOGIC)
    canvas_lyt.assign_cell_type((6, 7), sidb_technology.cell_type.LOGIC)
    [op_status, _evaluated_input_combinations] = is_operational(
        lyt,
        [create_and_tt()],
        params,
        input_bdl_wires,
        output_bdl_wires,
    )
    assert op_status == operational_status.NON_OPERATIONAL


@pytest.fixture
def and_gate_with_bdl_wires():
    """A 100-lattice AND gate together with its detected input and output BDL wires.

    Returns:
        The layout, its input BDL wires, and its output BDL wires.
    """
    lyt = sidb_100_lattice()

    lyt.assign_cell_type((0, 1), sidb_technology.cell_type.INPUT)
    lyt.assign_cell_type((2, 3), sidb_technology.cell_type.INPUT)

    lyt.assign_cell_type((20, 1), sidb_technology.cell_type.INPUT)
    lyt.assign_cell_type((19, 3), sidb_technology.cell_type.INPUT)

    lyt.assign_cell_type((4, 5), sidb_technology.cell_type.NORMAL)
    lyt.assign_cell_type((6, 7), sidb_technology.cell_type.NORMAL)

    lyt.assign_cell_type((14, 7), sidb_technology.cell_type.NORMAL)
    lyt.assign_cell_type((16, 5), sidb_technology.cell_type.NORMAL)

    lyt.assign_cell_type((10, 12, 0), sidb_technology.cell_type.OUTPUT)
    lyt.assign_cell_type((10, 14, 0), sidb_technology.cell_type.OUTPUT)

    lyt.assign_cell_type((10, 19), sidb_technology.cell_type.NORMAL)

    return (
        lyt,
        detect_bdl_wires_100(lyt, detect_bdl_wires_params(), bdl_wire_selection.INPUT),
        detect_bdl_wires_100(lyt, detect_bdl_wires_params(), bdl_wire_selection.OUTPUT),
    )


def test_generate_bdl_input_pattern_layouts(and_gate_with_bdl_wires):
    lyt, input_bdl_wires, _output_bdl_wires = and_gate_with_bdl_wires

    input_pattern_layouts = generate_bdl_input_pattern_layouts(lyt, bdl_input_iterator_params(), input_bdl_wires)

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

    input_pattern_layouts = generate_bdl_input_pattern_layouts(lyt, bdl_input_iterator_params(), input_bdl_wires)

    params = is_operational_params()
    params.simulation_parameters = sidb_simulation_parameters(2, mu_minus)

    [reference_status, reference_calls] = is_operational(
        lyt,
        [create_and_tt()],
        params,
        input_bdl_wires,
        output_bdl_wires,
    )
    [op_status, evaluated_input_combinations] = is_operational(
        input_pattern_layouts,
        [create_and_tt()],
        params,
        input_bdl_wires,
        output_bdl_wires,
    )

    assert reference_status == expected
    assert op_status == reference_status
    assert evaluated_input_combinations == reference_calls


def test_a_layout_list_that_does_not_match_the_specification_is_rejected(and_gate_with_bdl_wires):
    lyt, input_bdl_wires, output_bdl_wires = and_gate_with_bdl_wires

    input_pattern_layouts = generate_bdl_input_pattern_layouts(lyt, bdl_input_iterator_params(), input_bdl_wires)

    with pytest.raises(ValueError, match="expected 4 input pattern layouts"):
        is_operational(
            input_pattern_layouts[:2],
            [create_and_tt()],
            is_operational_params(),
            input_bdl_wires,
            output_bdl_wires,
        )


def test_and_gate_kinks(resources_dir):
    lyt = read_sqd_layout_100(str(resources_dir / "AND_mu_032_kinks.sqd"))

    params = is_operational_params()
    params.simulation_parameters = sidb_simulation_parameters(2, -0.32)

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], params)

    assert op_status == operational_status.OPERATIONAL

    params.op_condition = operational_condition.REJECT_KINKS

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], params)

    assert op_status == operational_status.NON_OPERATIONAL


def test_and_gate_non_operational_due_to_kinks(resources_dir):
    lyt = read_sqd_layout_100(str(resources_dir / "AND_mu_032_kinks.sqd"))

    params = is_operational_params()
    params.simulation_parameters = sidb_simulation_parameters(2, -0.32)

    result = is_kink_induced_non_operational(lyt, [create_and_tt()], params)

    assert result


def test_and_gate_non_operational_input_patterns_due_to_kinks(resources_dir):
    lyt = read_sqd_layout_100(str(resources_dir / "AND_mu_032_kinks.sqd"))

    params = is_operational_params()
    params.simulation_parameters = sidb_simulation_parameters(2, -0.32)

    non_operational_pattern_kinks = kink_induced_non_operational_input_patterns(lyt, [create_and_tt()], params)

    assert non_operational_pattern_kinks == {1, 2}


def test_and_gate_111_lattice_11_input_pattern(resources_dir):
    lyt = read_sqd_layout_111(str(resources_dir / "AND_mu_032_111_surface.sqd"))

    params = is_operational_params()
    params.simulation_parameters = sidb_simulation_parameters(2, -0.32)

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], params)

    assert op_status == operational_status.OPERATIONAL

    params.simulation_parameters = sidb_simulation_parameters(2, -0.1)

    assert params.simulation_parameters.mu_minus == -0.1

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], params)

    assert op_status == operational_status.NON_OPERATIONAL

    # filer only
    params.strategy_to_analyze_operational_status = operational_analysis_strategy.FILTER_ONLY
    assert params.strategy_to_analyze_operational_status == operational_analysis_strategy.FILTER_ONLY
    [op_status, _evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], params)
    assert op_status == operational_status.NON_OPERATIONAL

    # filer then simulation
    params.strategy_to_analyze_operational_status = operational_analysis_strategy.FILTER_THEN_SIMULATION
    assert params.strategy_to_analyze_operational_status == operational_analysis_strategy.FILTER_THEN_SIMULATION
    [op_status, _evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], params)
    assert op_status == operational_status.NON_OPERATIONAL


def test_and_gate_111_lattice_operational_input_pattern(resources_dir):
    lyt = read_sqd_layout_111(str(resources_dir / "AND_mu_032_111_surface.sqd"))

    params = is_operational_params()
    params.simulation_parameters = sidb_simulation_parameters(2, -0.30)

    operational_patterns = operational_input_patterns(lyt, [create_and_tt()], params)

    print(operational_patterns)
    assert len(operational_patterns) == 2

    assert operational_patterns == {0, 3}
