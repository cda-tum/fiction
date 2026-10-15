# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``clustercomplete``."""

from __future__ import annotations

from typing import Any

from mnt.pyfiction.sidb import lattice, lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.model import sidb_charge_state
from mnt.pyfiction.sidb.simulation.engines import clustercomplete, ground_state_space_reporting


def test_three_sidbs() -> None:
    """ClusterComplete simulates a three-SiDB layout."""

    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(1, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(2, 0, 0), sidb_dot_tag.NORMAL)

    options: dict[str, Any] = {
        "base": 2,
        "mu_minus": -0.25,
    }
    options["validity_witness_partitioning_max_cluster_size_gss"] = 15
    options["num_overlapping_witnesses_limit_gss"] = 8
    options["available_threads"] = 4
    options["report_gss_stats"] = ground_state_space_reporting.ON

    result = clustercomplete(layout, **options)

    assert result.algorithm_name == "ClusterComplete"
    assert result.layout == layout
    assert len(result.charge_distributions) <= 3

    options["base"] = 3

    result = clustercomplete(layout, **options)
    assert len(result.charge_distributions) <= 4

    options["epsilon_r"] = 2
    options["lambda_tf"] = 2
    result = clustercomplete(layout, **options)
    assert len(result.charge_distributions) <= 2


def test_perturber_and_sidb_pair_111() -> None:
    """ClusterComplete finds the H-Si(111)-1x1 ground state."""

    layout = sidb_layout(lattice.si_111_1x1())
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(1, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(2, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(3, 0, 0), sidb_dot_tag.NORMAL)

    result = clustercomplete(layout, base=2, mu_minus=-0.32)

    assert result.algorithm_name == "ClusterComplete"

    groundstate = result.groundstates()

    assert len(groundstate) == 1

    assert groundstate[0].get_charge_state(lattice_site(0, 0, 0)) == sidb_charge_state.NEGATIVE
    assert groundstate[0].get_charge_state(lattice_site(1, 0, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state(lattice_site(2, 0, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state(lattice_site(3, 0, 0)) == sidb_charge_state.NEGATIVE
