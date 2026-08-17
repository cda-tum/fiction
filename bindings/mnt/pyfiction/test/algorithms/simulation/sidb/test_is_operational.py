import os

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

dir_path = os.path.dirname(os.path.realpath(__file__))


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


def test_is_operational_with_input_pattern_layouts():
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

    input_bdl_wires = detect_bdl_wires_100(lyt, detect_bdl_wires_params(), bdl_wire_selection.INPUT)
    output_bdl_wires = detect_bdl_wires_100(lyt, detect_bdl_wires_params(), bdl_wire_selection.OUTPUT)

    input_pattern_layouts = generate_bdl_input_pattern_layouts(
        lyt,
        bdl_input_iterator_params(),
        input_bdl_wires,
    )

    # a 2-input gate has 4 input patterns
    assert len(input_pattern_layouts) == 4

    # the layout list yields the same verdict as the layout itself
    for simulation_parameters, expected in [
        (sidb_simulation_parameters(2, -0.28), operational_status.OPERATIONAL),
        (sidb_simulation_parameters(2, -0.1), operational_status.NON_OPERATIONAL),
    ]:
        params.simulation_parameters = simulation_parameters

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

    # a layout list that does not match the specification is rejected
    with pytest.raises(ValueError, match="expected 4 input pattern layouts"):
        is_operational(
            input_pattern_layouts[:2],
            [create_and_tt()],
            params,
            input_bdl_wires,
            output_bdl_wires,
        )


def test_and_gate_kinks():
    lyt = read_sqd_layout_100(dir_path + "/../../../resources/AND_mu_032_kinks.sqd")

    params = is_operational_params()
    params.simulation_parameters = sidb_simulation_parameters(2, -0.32)

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], params)

    assert op_status == operational_status.OPERATIONAL

    params.op_condition = operational_condition.REJECT_KINKS

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [create_and_tt()], params)

    assert op_status == operational_status.NON_OPERATIONAL


def test_and_gate_non_operational_due_to_kinks():
    lyt = read_sqd_layout_100(dir_path + "/../../../resources/AND_mu_032_kinks.sqd")

    params = is_operational_params()
    params.simulation_parameters = sidb_simulation_parameters(2, -0.32)

    result = is_kink_induced_non_operational(lyt, [create_and_tt()], params)

    assert result


def test_and_gate_non_operational_input_patterns_due_to_kinks():
    lyt = read_sqd_layout_100(dir_path + "/../../../resources/AND_mu_032_kinks.sqd")

    params = is_operational_params()
    params.simulation_parameters = sidb_simulation_parameters(2, -0.32)

    non_operational_pattern_kinks = kink_induced_non_operational_input_patterns(lyt, [create_and_tt()], params)

    assert non_operational_pattern_kinks == {1, 2}


def test_and_gate_111_lattice_11_input_pattern():
    lyt = read_sqd_layout_111(dir_path + "/../../../resources/AND_mu_032_111_surface.sqd")

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


def test_and_gate_111_lattice_operational_input_pattern():
    lyt = read_sqd_layout_111(dir_path + "/../../../resources/AND_mu_032_111_surface.sqd")

    params = is_operational_params()
    params.simulation_parameters = sidb_simulation_parameters(2, -0.30)

    operational_patterns = operational_input_patterns(lyt, [create_and_tt()], params)

    print(operational_patterns)
    assert len(operational_patterns) == 2

    assert operational_patterns == {0, 3}
