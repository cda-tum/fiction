# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction.sidb import DotTag, LatticeSite, SiDBLayout, SimulationParams
from mnt.pyfiction.sidb.simulation.defects import (
    determine_displacement_robustness_domain,
    dimer_displacement_policy,
    displacement_analysis_mode,
    displacement_robustness_domain_params,
    displacement_robustness_domain_stats,
)
from mnt.pyfiction.synthesis import (
    standard_functions,
)


def test_siqad_and_gate_100_lattice():
    layout = SiDBLayout()

    layout.assign_sidb(LatticeSite(0, 0, 1), DotTag.INPUT)
    layout.assign_sidb(LatticeSite(2, 1, 1), DotTag.INPUT)

    layout.assign_sidb(LatticeSite(20, 0, 1), DotTag.INPUT)
    layout.assign_sidb(LatticeSite(18, 1, 1), DotTag.INPUT)

    layout.assign_sidb(LatticeSite(4, 2, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(6, 3, 1), DotTag.NORMAL)

    layout.assign_sidb(LatticeSite(14, 3, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(16, 2, 1), DotTag.NORMAL)

    layout.assign_sidb(LatticeSite(10, 6, 0), DotTag.OUTPUT)
    layout.assign_sidb(LatticeSite(10, 7, 0), DotTag.OUTPUT)

    layout.assign_sidb(LatticeSite(10, 9, 1), DotTag.NORMAL)

    params = displacement_robustness_domain_params()

    params.displacement_variations = (1, 1)
    params.operational_params.simulation_parameters = SimulationParams(2, -0.28)

    params.operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance = 2.0
    params.operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance = 0.2

    # only the SiDBs at (4, 5) and (10, 12) are affected by displacement
    params.fixed_sidbs = {
        LatticeSite(0, 0, 1),
        LatticeSite(2, 1, 1),
        LatticeSite(20, 0, 1),
        LatticeSite(18, 1, 1),
        LatticeSite(4, 2, 1),
        LatticeSite(14, 3, 1),
        LatticeSite(16, 2, 1),
        LatticeSite(10, 7, 0),
        LatticeSite(10, 9, 1),
    }

    params.percentage_of_analyzed_displaced_layouts = 0.1
    params.dimer_policy = dimer_displacement_policy.ALLOW_OTHER_DIMER
    params.analysis_mode = displacement_analysis_mode.RANDOM

    stats = displacement_robustness_domain_stats()

    _ = determine_displacement_robustness_domain(layout, [standard_functions("and")[0]], params, stats)

    assert stats.num_non_operational_sidb_displacements + stats.num_operational_sidb_displacements == 8
