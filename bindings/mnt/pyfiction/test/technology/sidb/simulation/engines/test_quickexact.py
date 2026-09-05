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

from mnt.pyfiction import (
    automatic_base_number_detection,
    lattice,
    lattice_site,
    quickexact,
    quickexact_params,
    read_sqd_layout,
    sidb_charge_state,
    sidb_layout,
    sidb_simulation_parameters,
    sidb_technology,
)

if TYPE_CHECKING:
    from pathlib import Path


def test_three_sidbs() -> None:
    """QuickExact simulates a three-SiDB layout."""

    layout = sidb_layout()
    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(1, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(2, 0, 0), sidb_technology.cell_type.NORMAL)

    params = quickexact_params()
    params.simulation_parameters.base = 2
    params.simulation_parameters.mu_minus = -0.25
    params.base_number_detection = automatic_base_number_detection.OFF
    assert params.simulation_parameters.mu_minus == -0.25
    assert params.base_number_detection == automatic_base_number_detection.OFF

    result = quickexact(layout, params)

    assert result.algorithm_name == "QuickExact"
    assert result.layout == layout
    assert len(result.charge_distributions) <= 3

    params.base_number_detection = automatic_base_number_detection.ON
    assert params.base_number_detection == automatic_base_number_detection.ON

    result = quickexact(layout, params)
    assert len(result.charge_distributions) <= 4

    params.simulation_parameters.epsilon_r = 2
    params.simulation_parameters.lambda_tf = 2
    result = quickexact(layout, params)
    assert len(result.charge_distributions) <= 2


def test_perturber_and_sidb_pair_111() -> None:
    """QuickExact finds the H-Si(111)-1x1 ground state."""

    layout = sidb_layout(lattice.si_111_1x1())
    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(1, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(2, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(3, 0, 0), sidb_technology.cell_type.NORMAL)

    params = quickexact_params()
    params.simulation_parameters.base = 2
    params.simulation_parameters.mu_minus = -0.32
    params.base_number_detection = automatic_base_number_detection.OFF

    result = quickexact(layout, params)

    assert result.algorithm_name == "QuickExact"

    groundstate = result.groundstates()

    assert len(groundstate) == 1

    assert groundstate[0].get_charge_state(lattice_site(0, 0, 0)) == sidb_charge_state.NEGATIVE
    assert groundstate[0].get_charge_state(lattice_site(1, 0, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state(lattice_site(2, 0, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state(lattice_site(3, 0, 0)) == sidb_charge_state.NEGATIVE

    # the result offers the same lookup by distribution index and site
    assert result.charge_state(0, lattice_site(0, 0, 0)) == result.charge_distributions[0].get_charge_state(
        lattice_site(0, 0, 0)
    )


def test_simulate_all_inputs_of_and_gate(resources_dir: Path) -> None:
    """QuickExact finds a charge distribution for every AND-gate input.

    Args:
        resources_dir: Directory that contains the test layout.
    """

    and_gate = read_sqd_layout(str(resources_dir / "Bestagon_AND_mu_025_v0.sqd"))
    physical_parameters = sidb_simulation_parameters()
    physical_parameters.base = 2
    physical_parameters.epsilon_r = 5.6
    physical_parameters.lambda_tf = 5.0  # (nm)
    physical_parameters.mu_minus = -0.25  # (eV)
    quickexact_parameter = quickexact_params()
    quickexact_parameter.simulation_parameters = physical_parameters

    left_a, left_b = lattice_site(0, 0, 0), lattice_site(2, 1, 0)
    right_a, right_b = lattice_site(24, 1, 0), lattice_site(26, 0, 0)

    for left, right in [(left_b, right_a), (left_b, right_b), (left_a, right_a), (left_a, right_b)]:
        # delete one SiDB of each input BDL pair to set the input pattern
        and_gate.assign_cell_type(left, sidb_technology.cell_type.EMPTY)
        and_gate.assign_cell_type(right, sidb_technology.cell_type.EMPTY)

        assert len(quickexact(and_gate, quickexact_parameter).charge_distributions) > 0

        # restore the original layout
        and_gate.assign_cell_type(left, sidb_technology.cell_type.INPUT)
        and_gate.assign_cell_type(right, sidb_technology.cell_type.INPUT)
