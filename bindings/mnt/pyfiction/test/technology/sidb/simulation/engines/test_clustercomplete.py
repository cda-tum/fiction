# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``clustercomplete``."""

from __future__ import annotations

from mnt.pyfiction import (
    clustercomplete,
    clustercomplete_params,
    ground_state_space_reporting,
    lattice,
    lattice_site,
    sidb_charge_state,
    sidb_layout,
    sidb_technology,
)


def test_three_sidbs():
    layout = sidb_layout()
    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(1, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(2, 0, 0), sidb_technology.cell_type.NORMAL)

    params = clustercomplete_params()
    params.simulation_parameters.base = 2
    params.simulation_parameters.mu_minus = -0.25
    params.validity_witness_partitioning_max_cluster_size_gss = 15
    params.num_overlapping_witnesses_limit_gss = 8
    params.available_threads = 4
    params.report_gss_stats = ground_state_space_reporting.ON
    assert params.simulation_parameters.base == 2
    assert params.simulation_parameters.mu_minus == -0.25
    assert params.validity_witness_partitioning_max_cluster_size_gss == 15
    assert params.num_overlapping_witnesses_limit_gss == 8
    assert params.available_threads == 4
    assert params.report_gss_stats == ground_state_space_reporting.ON

    result = clustercomplete(layout, params)

    assert result.algorithm_name == "ClusterComplete"
    assert result.layout == layout
    assert len(result.charge_distributions) <= 3

    params.simulation_parameters.base = 3
    assert params.simulation_parameters.base == 3

    result = clustercomplete(layout, params)
    assert len(result.charge_distributions) <= 4

    params.simulation_parameters.epsilon_r = 2
    params.simulation_parameters.lambda_tf = 2
    result = clustercomplete(layout, params)
    assert len(result.charge_distributions) <= 2


def test_perturber_and_sidb_pair_111():
    layout = sidb_layout(lattice.si_111_1x1())
    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(1, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(2, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(3, 0, 0), sidb_technology.cell_type.NORMAL)

    params = clustercomplete_params()
    params.simulation_parameters.base = 2
    params.simulation_parameters.mu_minus = -0.32
    assert params.simulation_parameters.mu_minus == -0.32

    result = clustercomplete(layout, params)

    assert result.algorithm_name == "ClusterComplete"

    groundstate = result.groundstates()

    assert len(groundstate) == 1

    assert groundstate[0].get_charge_state(lattice_site(0, 0, 0)) == sidb_charge_state.NEGATIVE
    assert groundstate[0].get_charge_state(lattice_site(1, 0, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state(lattice_site(2, 0, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state(lattice_site(3, 0, 0)) == sidb_charge_state.NEGATIVE
