# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``clustercomplete``."""

from __future__ import annotations

from mnt.pyfiction.sidb import ChargeState, DotTag, Lattice, LatticeSite, SiDBLayout
from mnt.pyfiction.sidb.simulation import ClusterCompleteParams, GroundStateSpaceReporting, clustercomplete


def test_three_sidbs() -> None:
    """ClusterComplete simulates a three-SiDB layout."""

    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(1, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(2, 0, 0), DotTag.NORMAL)

    params = ClusterCompleteParams()
    params.simulation_parameters.base = 2
    params.simulation_parameters.mu_minus = -0.25
    params.validity_witness_partitioning_max_cluster_size_gss = 15
    params.num_overlapping_witnesses_limit_gss = 8
    params.available_threads = 4
    params.report_gss_stats = GroundStateSpaceReporting.ON
    assert params.simulation_parameters.base == 2
    assert params.simulation_parameters.mu_minus == -0.25
    assert params.validity_witness_partitioning_max_cluster_size_gss == 15
    assert params.num_overlapping_witnesses_limit_gss == 8
    assert params.available_threads == 4
    assert params.report_gss_stats == GroundStateSpaceReporting.ON

    result = clustercomplete(layout, params=params)

    assert result.algorithm_name == "ClusterComplete"
    assert result.layout == layout
    assert len(result.charge_distributions) <= 3

    params.simulation_parameters.base = 3
    assert params.simulation_parameters.base == 3

    result = clustercomplete(layout, params=params)
    assert len(result.charge_distributions) <= 4

    params.simulation_parameters.epsilon_r = 2
    params.simulation_parameters.lambda_tf = 2
    result = clustercomplete(layout, params=params)
    assert len(result.charge_distributions) <= 2


def test_perturber_and_sidb_pair_111() -> None:
    """ClusterComplete finds the H-Si(111)-1x1 ground state."""

    layout = SiDBLayout(Lattice.si_111_1x1())
    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(1, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(2, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(3, 0, 0), DotTag.NORMAL)

    params = ClusterCompleteParams()
    params.simulation_parameters.base = 2
    params.simulation_parameters.mu_minus = -0.32
    assert params.simulation_parameters.mu_minus == -0.32

    result = clustercomplete(layout, params=params)

    assert result.algorithm_name == "ClusterComplete"

    groundstate = result.ground_states()

    assert len(groundstate) == 1

    assert groundstate[0].get_charge_state(LatticeSite(0, 0, 0)) == ChargeState.NEGATIVE
    assert groundstate[0].get_charge_state(LatticeSite(1, 0, 0)) == ChargeState.NEUTRAL
    assert groundstate[0].get_charge_state(LatticeSite(2, 0, 0)) == ChargeState.NEUTRAL
    assert groundstate[0].get_charge_state(LatticeSite(3, 0, 0)) == ChargeState.NEGATIVE
