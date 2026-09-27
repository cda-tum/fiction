# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction.sidb import DotTag, LatticeSite, SiDBLayout, SimulationParams
from mnt.pyfiction.sidb.analysis import (
    BdlWireDetectionParams,
    BdlWireSelection,
    InputPatternParams,
    OperationalAnalysisStrategy,
    OperationalCondition,
    OperationalParams,
    OperationalStatus,
    detect_bdl_wires,
    input_patterns,
    is_kink_induced_non_operational,
    is_operational,
    kink_induced_non_operational_input_patterns,
    operational_input_patterns,
)
from mnt.pyfiction.sidb.io import read_sqd_layout
from mnt.pyfiction.synthesis import (
    standard_functions,
)


def test_is_operational():
    lyt = SiDBLayout()

    lyt.assign_sidb(LatticeSite(0, 0, 1), DotTag.INPUT)
    lyt.assign_sidb(LatticeSite(2, 1, 1), DotTag.INPUT)

    lyt.assign_sidb(LatticeSite(20, 0, 1), DotTag.INPUT)
    lyt.assign_sidb(LatticeSite(19, 1, 1), DotTag.INPUT)

    lyt.assign_sidb(LatticeSite(4, 2, 1), DotTag.NORMAL)
    lyt.assign_sidb(LatticeSite(6, 3, 1), DotTag.NORMAL)

    lyt.assign_sidb(LatticeSite(14, 3, 1), DotTag.NORMAL)
    lyt.assign_sidb(LatticeSite(16, 2, 1), DotTag.NORMAL)

    lyt.assign_sidb(LatticeSite(10, 6, 0), DotTag.OUTPUT)
    lyt.assign_sidb(LatticeSite(10, 7, 0), DotTag.OUTPUT)

    lyt.assign_sidb(LatticeSite(10, 9, 1), DotTag.NORMAL)

    params = OperationalParams()
    params.simulation_parameters = SimulationParams(2, -0.28)

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [standard_functions("and")[0]], params=params)

    assert op_status == OperationalStatus.OPERATIONAL

    params.simulation_parameters = SimulationParams(2, -0.1)

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [standard_functions("and")[0]], params=params)

    assert op_status == OperationalStatus.NON_OPERATIONAL

    # pre-determined I/O pins
    output_bdl_wires = detect_bdl_wires(lyt, BdlWireDetectionParams(), BdlWireSelection.OUTPUT)
    input_bdl_wires = detect_bdl_wires(lyt, BdlWireDetectionParams(), BdlWireSelection.INPUT)
    [op_status, _evaluated_input_combinations] = is_operational(
        lyt,
        [standard_functions("and")[0]],
        params=params,
        input_wires=input_bdl_wires,
        output_wires=output_bdl_wires,
    )
    assert op_status == OperationalStatus.NON_OPERATIONAL

    # pre-determined I/O pins and canvas layout
    canvas_lyt = SiDBLayout()
    canvas_lyt.assign_sidb(LatticeSite(4, 2, 1), DotTag.LOGIC)
    canvas_lyt.assign_sidb(LatticeSite(6, 3, 1), DotTag.LOGIC)
    [op_status, _evaluated_input_combinations] = is_operational(
        lyt,
        [standard_functions("and")[0]],
        params=params,
        input_wires=input_bdl_wires,
        output_wires=output_bdl_wires,
    )
    assert op_status == OperationalStatus.NON_OPERATIONAL


@pytest.fixture
def and_gate_with_bdl_wires():
    """A 100-lattice AND gate together with its detected input and output BDL wires.

    Returns:
        The layout, its input BDL wires, and its output BDL wires.
    """
    lyt = SiDBLayout()

    lyt.assign_sidb(LatticeSite(0, 0, 1), DotTag.INPUT)
    lyt.assign_sidb(LatticeSite(2, 1, 1), DotTag.INPUT)

    lyt.assign_sidb(LatticeSite(20, 0, 1), DotTag.INPUT)
    lyt.assign_sidb(LatticeSite(19, 1, 1), DotTag.INPUT)

    lyt.assign_sidb(LatticeSite(4, 2, 1), DotTag.NORMAL)
    lyt.assign_sidb(LatticeSite(6, 3, 1), DotTag.NORMAL)

    lyt.assign_sidb(LatticeSite(14, 3, 1), DotTag.NORMAL)
    lyt.assign_sidb(LatticeSite(16, 2, 1), DotTag.NORMAL)

    lyt.assign_sidb(LatticeSite(10, 6, 0), DotTag.OUTPUT)
    lyt.assign_sidb(LatticeSite(10, 7, 0), DotTag.OUTPUT)

    lyt.assign_sidb(LatticeSite(10, 9, 1), DotTag.NORMAL)

    return (
        lyt,
        detect_bdl_wires(lyt, BdlWireDetectionParams(), BdlWireSelection.INPUT),
        detect_bdl_wires(lyt, BdlWireDetectionParams(), BdlWireSelection.OUTPUT),
    )


def test_generate_bdl_input_pattern_layouts(and_gate_with_bdl_wires):
    lyt, input_bdl_wires, _output_bdl_wires = and_gate_with_bdl_wires

    input_pattern_layouts = list(input_patterns(lyt, params=InputPatternParams(), input_wires=input_bdl_wires))

    # a 2-input gate has 4 input patterns
    assert len(input_pattern_layouts) == 4


@pytest.mark.parametrize(
    ("mu_minus", "expected"),
    [
        pytest.param(-0.28, OperationalStatus.OPERATIONAL, id="operational"),
        pytest.param(-0.1, OperationalStatus.NON_OPERATIONAL, id="non_operational"),
    ],
)
def test_input_pattern_layouts_yield_the_same_verdict(and_gate_with_bdl_wires, mu_minus, expected):
    lyt, input_bdl_wires, output_bdl_wires = and_gate_with_bdl_wires

    input_pattern_layouts = list(input_patterns(lyt, params=InputPatternParams(), input_wires=input_bdl_wires))

    params = OperationalParams()
    params.simulation_parameters = SimulationParams(2, mu_minus)

    [reference_status, reference_calls] = is_operational(
        lyt,
        [standard_functions("and")[0]],
        params=params,
        input_wires=input_bdl_wires,
        output_wires=output_bdl_wires,
    )
    [op_status, evaluated_input_combinations] = is_operational(
        input_pattern_layouts,
        [standard_functions("and")[0]],
        params=params,
        input_wires=input_bdl_wires,
        output_wires=output_bdl_wires,
    )

    assert reference_status == expected
    assert op_status == reference_status
    assert evaluated_input_combinations == reference_calls


def test_a_layout_list_that_does_not_match_the_specification_is_rejected(and_gate_with_bdl_wires):
    lyt, input_bdl_wires, output_bdl_wires = and_gate_with_bdl_wires

    input_pattern_layouts = list(input_patterns(lyt, params=InputPatternParams(), input_wires=input_bdl_wires))

    with pytest.raises(ValueError, match="expected 4 input pattern layouts"):
        is_operational(
            input_pattern_layouts[:2],
            [standard_functions("and")[0]],
            params=OperationalParams(),
            input_wires=input_bdl_wires,
            output_wires=output_bdl_wires,
        )


def test_and_gate_kinks(resources_dir):
    lyt = read_sqd_layout(str(resources_dir / "AND_mu_032_kinks.sqd"))

    params = OperationalParams()
    params.simulation_parameters = SimulationParams(2, -0.32)

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [standard_functions("and")[0]], params=params)

    assert op_status == OperationalStatus.OPERATIONAL

    params.op_condition = OperationalCondition.REJECT_KINKS

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [standard_functions("and")[0]], params=params)

    assert op_status == OperationalStatus.NON_OPERATIONAL


def test_and_gate_non_operational_due_to_kinks(resources_dir):
    lyt = read_sqd_layout(str(resources_dir / "AND_mu_032_kinks.sqd"))

    params = OperationalParams()
    params.simulation_parameters = SimulationParams(2, -0.32)

    result = is_kink_induced_non_operational(lyt, [standard_functions("and")[0]], params)

    assert result


def test_and_gate_non_operational_input_patterns_due_to_kinks(resources_dir):
    lyt = read_sqd_layout(str(resources_dir / "AND_mu_032_kinks.sqd"))

    params = OperationalParams()
    params.simulation_parameters = SimulationParams(2, -0.32)

    non_operational_pattern_kinks = kink_induced_non_operational_input_patterns(
        lyt, [standard_functions("and")[0]], params
    )

    assert non_operational_pattern_kinks == {1, 2}


def test_and_gate_111_lattice_11_input_pattern(resources_dir):
    lyt = read_sqd_layout(str(resources_dir / "AND_mu_032_111_surface.sqd"))

    params = OperationalParams()
    params.simulation_parameters = SimulationParams(2, -0.32)

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [standard_functions("and")[0]], params=params)

    assert op_status == OperationalStatus.OPERATIONAL

    params.simulation_parameters = SimulationParams(2, -0.1)

    assert params.simulation_parameters.mu_minus == -0.1

    [op_status, _evaluated_input_combinations] = is_operational(lyt, [standard_functions("and")[0]], params=params)

    assert op_status == OperationalStatus.NON_OPERATIONAL

    # filer only
    params.strategy_to_analyze_operational_status = OperationalAnalysisStrategy.FILTER_ONLY
    assert params.strategy_to_analyze_operational_status == OperationalAnalysisStrategy.FILTER_ONLY
    [op_status, _evaluated_input_combinations] = is_operational(lyt, [standard_functions("and")[0]], params=params)
    assert op_status == OperationalStatus.NON_OPERATIONAL

    # filer then simulation
    params.strategy_to_analyze_operational_status = OperationalAnalysisStrategy.FILTER_THEN_SIMULATION
    assert params.strategy_to_analyze_operational_status == OperationalAnalysisStrategy.FILTER_THEN_SIMULATION
    [op_status, _evaluated_input_combinations] = is_operational(lyt, [standard_functions("and")[0]], params=params)
    assert op_status == OperationalStatus.NON_OPERATIONAL


def test_and_gate_111_lattice_operational_input_pattern(resources_dir):
    lyt = read_sqd_layout(str(resources_dir / "AND_mu_032_111_surface.sqd"))

    params = OperationalParams()
    params.simulation_parameters = SimulationParams(2, -0.30)

    operational_patterns = operational_input_patterns(lyt, [standard_functions("and")[0]], params)

    print(operational_patterns)
    assert len(operational_patterns) == 2

    assert operational_patterns == {0, 3}
