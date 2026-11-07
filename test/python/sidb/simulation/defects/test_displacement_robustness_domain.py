# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import Any

import pytest

from mnt.pyfiction.sidb import lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.io import read_sqd_layout
from mnt.pyfiction.sidb.model import sidb_defect, sidb_defect_type
from mnt.pyfiction.sidb.simulation.defects import (
    determine_displacement_robustness_domain,
    dimer_displacement_policy,
    displacement_analysis_mode,
    displacement_robustness_domain_stats,
)
from mnt.pyfiction.sidb.simulation.logic import operational_status
from mnt.pyfiction.synthesis import create_and_tt, create_or_tt


def test_siqad_and_gate_100_lattice():
    layout = sidb_layout()

    layout.assign_sidb(lattice_site(0, 0, 1), sidb_dot_tag.INPUT)
    layout.assign_sidb(lattice_site(2, 1, 1), sidb_dot_tag.INPUT)

    layout.assign_sidb(lattice_site(20, 0, 1), sidb_dot_tag.INPUT)
    layout.assign_sidb(lattice_site(18, 1, 1), sidb_dot_tag.INPUT)

    layout.assign_sidb(lattice_site(4, 2, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(6, 3, 1), sidb_dot_tag.NORMAL)

    layout.assign_sidb(lattice_site(14, 3, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(16, 2, 1), sidb_dot_tag.NORMAL)

    layout.assign_sidb(lattice_site(10, 6, 0), sidb_dot_tag.OUTPUT)
    layout.assign_sidb(lattice_site(10, 7, 0), sidb_dot_tag.OUTPUT)

    layout.assign_sidb(lattice_site(10, 9, 1), sidb_dot_tag.NORMAL)

    options: dict[str, Any] = {}

    options["displacement_variations"] = (1, 1)
    options.update({"base": 2, "mu_minus": -0.28, "epsilon_r": 5.6, "lambda_tf": 5.0})

    options["maximum_distance"] = 2.0
    options["minimum_distance"] = 0.2

    # only the SiDBs at (4, 5) and (10, 12) are affected by displacement
    options["fixed_sidbs"] = {
        lattice_site(0, 0, 1),
        lattice_site(2, 1, 1),
        lattice_site(20, 0, 1),
        lattice_site(18, 1, 1),
        lattice_site(4, 2, 1),
        lattice_site(14, 3, 1),
        lattice_site(16, 2, 1),
        lattice_site(10, 7, 0),
        lattice_site(10, 9, 1),
    }

    options["percentage_of_analyzed_displaced_layouts"] = 0.1
    options["dimer_policy"] = dimer_displacement_policy.ALLOW_OTHER_DIMER
    options["analysis_mode"] = displacement_analysis_mode.RANDOM

    stats = displacement_robustness_domain_stats()

    _ = determine_displacement_robustness_domain(layout, [create_and_tt()], stats=stats, **options)

    assert stats.num_non_operational_sidb_displacements + stats.num_operational_sidb_displacements == 8


@pytest.mark.parametrize("policy", list(dimer_displacement_policy))
@pytest.mark.parametrize("mode", list(displacement_analysis_mode))
def test_stationary_displacement_preserves_layout(resources_dir, policy, mode) -> None:
    """Both search modes and dimer policies preserve a fully fixed layout, including defects and name."""
    layout = read_sqd_layout(str(resources_dir / "siqad_or_gate.sqd"), "fixed surface")
    layout.assign_defect(lattice_site(100, 100), sidb_defect(sidb_defect_type.SILOXANE))
    stats = displacement_robustness_domain_stats()
    reports = []
    # A fixed layout has exactly one candidate, so exhaustive and random modes have the same complete oracle.
    domain = determine_displacement_robustness_domain(
        layout,
        [create_or_tt()],
        displacement_variations=(0, 0),
        fixed_sidbs=set(layout.sidbs()),
        analysis_mode=mode,
        dimer_policy=policy,
        threshold_bdl_interdistance=1.5,
        mu_minus=-0.28,
        epsilon_r=5.7,
        lambda_tf=3.0,
        stats=stats,
        on_progress=lambda task, done, total: reports.append((task, done, total)),
    )
    assert domain.influence_information == [(layout, operational_status.OPERATIONAL)]
    assert stats.num_non_operational_sidb_displacements == 0
    assert stats.num_operational_sidb_displacements == 1
    assert reports[-1][1:] == (1, 1)


@pytest.mark.parametrize("percentage", [-0.1, 1.1, float("nan"), float("inf")])
def test_invalid_displacement_percentage(percentage: float) -> None:
    """Sampling fractions reject non-finite values and values outside the unit interval."""
    with pytest.raises(ValueError, match="between 0 and 1"):
        determine_displacement_robustness_domain(
            sidb_layout(), [create_and_tt()], percentage_of_analyzed_displaced_layouts=percentage
        )


def test_displacement_range_preserves_statistics(resources_dir) -> None:
    """Unrepresentable displacements raise before publishing statistics."""
    layout = read_sqd_layout(str(resources_dir / "siqad_or_gate.sqd"))
    stats = displacement_robustness_domain_stats()
    stats.num_operational_sidb_displacements = 42
    with pytest.raises(IndexError):
        determine_displacement_robustness_domain(
            layout,
            [create_and_tt()],
            displacement_variations=(2**64 - 1, 0),
            threshold_bdl_interdistance=1.5,
            stats=stats,
        )
    assert stats.num_operational_sidb_displacements == 42
