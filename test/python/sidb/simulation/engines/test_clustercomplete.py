# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``clustercomplete``."""

from __future__ import annotations

from collections import Counter
from typing import Any

import pytest

from mnt.pyfiction.sidb import lattice, lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.model import sidb_charge_state
from mnt.pyfiction.sidb.simulation import engines

pytestmark = pytest.mark.skipif(not hasattr(engines, "clustercomplete"), reason="pyfiction was built without ALGLIB")


def test_three_sidbs() -> None:
    """ClusterComplete simulates a three-SiDB layout."""

    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(1, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(2, 0, 0), sidb_dot_tag.NORMAL)

    options: dict[str, Any] = {
        "base": 2,
        "mu_minus": -0.25,
        "epsilon_r": 5.6,
        "lambda_tf": 5.0,
    }
    options["validity_witness_partitioning_max_cluster_size_gss"] = 15
    options["num_overlapping_witnesses_limit_gss"] = 8
    options["available_threads"] = 4
    options["report_gss_stats"] = engines.ground_state_space_reporting.ON

    def check_distributions(result: Any, base: int, *, expect_positive: bool = False) -> None:
        """Compare every charge configuration with exhaustive simulation, ignoring order."""
        reference = engines.exhaustive_ground_state_simulation(
            layout, base=base, **{key: options[key] for key in ("mu_minus", "epsilon_r", "lambda_tf")}
        )
        sites = [lattice_site(x, 0, 0) for x in range(3)]
        expected = Counter(tuple(cd.get_charge_state(site) for site in sites) for cd in reference.charge_distributions)
        actual = Counter(tuple(cd.get_charge_state(site) for site in sites) for cd in result.charge_distributions)
        assert expected
        negative, neutral, positive = sidb_charge_state.NEGATIVE, sidb_charge_state.NEUTRAL, sidb_charge_state.POSITIVE
        singles = [(negative, neutral, neutral), (neutral, negative, neutral), (neutral, neutral, negative)]
        if base == 2:
            known = singles
        elif options["epsilon_r"] == 5.6:
            known = [*singles, (negative, positive, negative)]
        else:
            known = [(negative, positive, negative), (positive, negative, positive)]
        assert expected == Counter(known)
        assert any(sidb_charge_state.POSITIVE in state for state in expected) is expect_positive
        assert actual == expected

    result = engines.clustercomplete(layout, **options)

    assert result.algorithm_name == "ClusterComplete"
    assert result.layout == layout
    assert len(result.charge_distributions) <= 3
    check_distributions(result, 2)

    options["base"] = 3

    result = engines.clustercomplete(layout, **options)
    assert len(result.charge_distributions) <= 4
    check_distributions(result, 3, expect_positive=True)

    options["epsilon_r"] = 2
    options["lambda_tf"] = 2
    result = engines.clustercomplete(layout, **options)
    assert len(result.charge_distributions) <= 2
    check_distributions(result, 3, expect_positive=True)


def test_perturber_and_sidb_pair_111() -> None:
    """ClusterComplete finds the H-Si(111)-1x1 ground state."""

    layout = sidb_layout(lattice.si_111_1x1())
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(1, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(2, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(3, 0, 0), sidb_dot_tag.NORMAL)

    result = engines.clustercomplete(layout, base=2, mu_minus=-0.32)

    assert result.algorithm_name == "ClusterComplete"

    groundstate = result.groundstates()

    assert len(groundstate) == 1

    assert groundstate[0].get_charge_state(lattice_site(0, 0, 0)) == sidb_charge_state.NEGATIVE
    assert groundstate[0].get_charge_state(lattice_site(1, 0, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state(lattice_site(2, 0, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state(lattice_site(3, 0, 0)) == sidb_charge_state.NEGATIVE
