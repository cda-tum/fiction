# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.sidb import DotTag, Lattice, LatticeSite, SiDBLayout, SimulationParams
from mnt.pyfiction.sidb.analysis import (
    BdlPairDetectionParams,
    BdlWireDetectionParams,
    BdlWireSelection,
    CriticalTemperatureParams,
    InputPatternParams,
    critical_temperature_gate_based,
    critical_temperature_non_gate_based,
    detect_bdl_pairs,
    detect_bdl_wires,
    input_patterns,
)
from mnt.pyfiction.sidb.io import read_sqd_layout
from mnt.pyfiction.sidb.simulation import SimulationEngine
from mnt.pyfiction.synthesis import (
    standard_functions,
)

if TYPE_CHECKING:
    from pathlib import Path


@pytest.mark.parametrize(
    "lat",
    [pytest.param(Lattice.si_100_2x1(), id="100"), pytest.param(Lattice.si_111_1x1(), id="111")],
)
def test_perturber_and_sidb_pair(lat: Lattice) -> None:
    """Check non-gate critical temperature on both supported lattices.

    Args:
        lat: Lattice used for the test layout.
    """
    layout = SiDBLayout(lat)
    layout.assign_sidb(LatticeSite(0, 0, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(4, 0, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(6, 0, 1), DotTag.NORMAL)

    params = CriticalTemperatureParams()

    params.operational_params.sim_engine = SimulationEngine.QUICKEXACT

    result = critical_temperature_non_gate_based(layout, params=params)
    assert (result).temperature == 400
    stats = result.stats

    assert stats.algorithm_name == "QuickExact"
    assert stats.num_valid_lyt == 1


def test_gate_based_simulation(resources_dir: Path) -> None:
    """Check gate-based critical temperature for an XOR gate.

    Args:
        resources_dir: Directory that contains the test layout.
    """
    layout = read_sqd_layout(str(resources_dir / "hex_21_inputsdbp_xor_v1.sqd"), name="xor_gate")
    params = CriticalTemperatureParams()

    params.operational_params.simulation_parameters.base = 2

    params.operational_params.sim_engine = SimulationEngine.QUICKEXACT

    spec = [standard_functions("xor")[0]]

    result = critical_temperature_gate_based(layout, spec, params=params)
    assert (result).temperature <= 200
    stats = result.stats

    assert stats.algorithm_name == "QuickExact"


def test_bestagon_inv(resources_dir: Path) -> None:
    """Check a Bestagon inverter with QuickSim.

    Args:
        resources_dir: Directory that contains the test layout.
    """
    layout = read_sqd_layout(
        str(resources_dir / "hex_11_inputsdbp_inv_straight_v0_manual.sqd"),
        name="inverter_input_0",
    )

    params = CriticalTemperatureParams()

    params.operational_params.sim_engine = SimulationEngine.QUICKSIM
    params.operational_params.simulation_parameters.base = 2  # QuickSim simulates two charge states only

    spec = [standard_functions("not")[0]]

    result = critical_temperature_gate_based(layout, spec, params=params)
    assert (result).temperature <= 400
    stats = result.stats

    assert stats.algorithm_name == "QuickSim"
    assert stats.num_valid_lyt > 1


def test_bestagon_inv_with_different_mu(resources_dir: Path) -> None:
    """Check a Bestagon inverter at a different chemical potential.

    Args:
        resources_dir: Directory that contains the test layout.
    """
    layout = read_sqd_layout(
        str(resources_dir / "hex_11_inputsdbp_inv_straight_v0_manual.sqd"),
        name="inverter_input_0",
    )

    params = CriticalTemperatureParams()
    params.operational_params.simulation_parameters.base = 2
    params.operational_params.simulation_parameters.mu_minus = -0.2

    params.operational_params.sim_engine = SimulationEngine.QUICKEXACT

    spec = [standard_functions("not")[0]]

    result = critical_temperature_gate_based(layout, spec, params=params)
    assert (result).temperature <= 5
    stats = result.stats

    assert stats.algorithm_name == "QuickExact"


def test_critical_temperature_with_input_pattern_layouts() -> None:
    """Compare pre-generated input layouts with the layout-based overload."""
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

    params = CriticalTemperatureParams()
    params.operational_params.sim_engine = SimulationEngine.QUICKEXACT
    params.operational_params.simulation_parameters = SimulationParams(2, -0.28)

    input_bdl_wires = detect_bdl_wires(lyt, BdlWireDetectionParams(), BdlWireSelection.INPUT)
    output_bdl_wires = detect_bdl_wires(lyt, BdlWireDetectionParams(), BdlWireSelection.OUTPUT)
    output_bdl_pairs = detect_bdl_pairs(lyt, DotTag.OUTPUT, BdlPairDetectionParams())

    input_pattern_layouts = list(input_patterns(lyt, params=InputPatternParams(), input_wires=input_bdl_wires))

    # a 2-input gate has 4 input patterns
    assert len(input_pattern_layouts) == 4

    result = critical_temperature_gate_based(lyt, [standard_functions("and")[0]], params=params)
    reference_ct = result.temperature
    reference_stats = result.stats

    result = critical_temperature_gate_based(
        input_pattern_layouts,
        [standard_functions("and")[0]],
        params=params,
        output_bdl_pairs=output_bdl_pairs,
        input_bdl_wires=input_bdl_wires,
        output_bdl_wires=output_bdl_wires,
    )
    ct = result.temperature
    stats = result.stats

    # the two overloads run the same computation, so the results must be identical
    assert ct == reference_ct
    assert stats.num_valid_lyt == reference_stats.num_valid_lyt
    assert stats.algorithm_name == reference_stats.algorithm_name

    # a layout list that does not match the specification is rejected
    with pytest.raises(ValueError, match="expected 4 input pattern layouts"):
        critical_temperature_gate_based(
            input_pattern_layouts[:1],
            [standard_functions("and")[0]],
            params=params,
            output_bdl_pairs=output_bdl_pairs,
            input_bdl_wires=input_bdl_wires,
            output_bdl_wires=output_bdl_wires,
        )

    # more output BDL pairs than truth tables is rejected
    with pytest.raises(ValueError, match="expected 1 output BDL pairs"):
        critical_temperature_gate_based(
            input_pattern_layouts,
            [standard_functions("and")[0]],
            params=params,
            output_bdl_pairs=[*output_bdl_pairs, output_bdl_pairs[0]],
            input_bdl_wires=input_bdl_wires,
            output_bdl_wires=output_bdl_wires,
        )
