# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``quickexact``."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction.sidb import ChargeState, DotTag, Lattice, LatticeSite, SiDBLayout, SimulationParams
from mnt.pyfiction.sidb.io import read_sqd_layout
from mnt.pyfiction.sidb.simulation import AutomaticBaseNumberDetection, QuickExactParams, quickexact

if TYPE_CHECKING:
    from pathlib import Path


def test_three_sidbs() -> None:
    """QuickExact simulates a three-SiDB layout."""

    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(1, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(2, 0, 0), DotTag.NORMAL)

    params = QuickExactParams()
    params.simulation_parameters.base = 2
    params.simulation_parameters.mu_minus = -0.25
    params.base_number_detection = AutomaticBaseNumberDetection.OFF
    assert params.simulation_parameters.mu_minus == -0.25
    assert params.base_number_detection == AutomaticBaseNumberDetection.OFF

    result = quickexact(layout, params=params)

    assert result.algorithm_name == "QuickExact"
    assert result.layout == layout
    assert len(result.charge_distributions) <= 3

    params.base_number_detection = AutomaticBaseNumberDetection.ON
    assert params.base_number_detection == AutomaticBaseNumberDetection.ON

    result = quickexact(layout, params=params)
    assert len(result.charge_distributions) <= 4

    params.simulation_parameters.epsilon_r = 2
    params.simulation_parameters.lambda_tf = 2
    result = quickexact(layout, params=params)
    assert len(result.charge_distributions) <= 2


def test_perturber_and_sidb_pair_111() -> None:
    """QuickExact finds the H-Si(111)-1x1 ground state."""

    layout = SiDBLayout(Lattice.si_111_1x1())
    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(1, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(2, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(3, 0, 0), DotTag.NORMAL)

    params = QuickExactParams()
    params.simulation_parameters.base = 2
    params.simulation_parameters.mu_minus = -0.32
    params.base_number_detection = AutomaticBaseNumberDetection.OFF

    result = quickexact(layout, params=params)

    assert result.algorithm_name == "QuickExact"

    groundstate = result.ground_states()

    assert len(groundstate) == 1

    assert groundstate[0].get_charge_state(LatticeSite(0, 0, 0)) == ChargeState.NEGATIVE
    assert groundstate[0].get_charge_state(LatticeSite(1, 0, 0)) == ChargeState.NEUTRAL
    assert groundstate[0].get_charge_state(LatticeSite(2, 0, 0)) == ChargeState.NEUTRAL
    assert groundstate[0].get_charge_state(LatticeSite(3, 0, 0)) == ChargeState.NEGATIVE

    # the result offers the same lookup by distribution index and site
    assert result.charge_state(0, LatticeSite(0, 0, 0)) == result.charge_distributions[0].get_charge_state(
        LatticeSite(0, 0, 0)
    )


def test_simulate_all_inputs_of_and_gate(resources_dir: Path) -> None:
    """QuickExact finds a charge distribution for every AND-gate input.

    Args:
        resources_dir: Directory that contains the test layout.
    """

    and_gate = read_sqd_layout(str(resources_dir / "Bestagon_AND_mu_025_v0.sqd"))
    physical_parameters = SimulationParams()
    physical_parameters.base = 2
    physical_parameters.epsilon_r = 5.6
    physical_parameters.lambda_tf = 5.0  # (nm)
    physical_parameters.mu_minus = -0.25  # (eV)
    quickexact_parameter = QuickExactParams()
    quickexact_parameter.simulation_parameters = physical_parameters

    left_a, left_b = LatticeSite(0, 0, 0), LatticeSite(2, 1, 0)
    right_a, right_b = LatticeSite(24, 1, 0), LatticeSite(26, 0, 0)

    for left, right in [(left_b, right_a), (left_b, right_b), (left_a, right_a), (left_a, right_b)]:
        # delete one SiDB of each input BDL pair to set the input pattern
        and_gate.assign_sidb(left, DotTag.EMPTY)
        and_gate.assign_sidb(right, DotTag.EMPTY)

        assert len(quickexact(and_gate, params=quickexact_parameter).charge_distributions) > 0

        # restore the original layout
        and_gate.assign_sidb(left, DotTag.INPUT)
        and_gate.assign_sidb(right, DotTag.INPUT)
