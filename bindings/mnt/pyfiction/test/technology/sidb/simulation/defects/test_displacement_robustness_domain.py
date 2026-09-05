# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction import (
    create_and_tt,
    determine_displacement_robustness_domain,
    dimer_displacement_policy,
    displacement_analysis_mode,
    displacement_robustness_domain_params,
    displacement_robustness_domain_stats,
    lattice_site,
    sidb_layout,
    sidb_simulation_parameters,
    sidb_technology,
)


def test_siqad_and_gate_100_lattice():
    layout = sidb_layout()

    layout.assign_cell_type(lattice_site(0, 0, 1), sidb_technology.cell_type.INPUT)
    layout.assign_cell_type(lattice_site(2, 1, 1), sidb_technology.cell_type.INPUT)

    layout.assign_cell_type(lattice_site(20, 0, 1), sidb_technology.cell_type.INPUT)
    layout.assign_cell_type(lattice_site(18, 1, 1), sidb_technology.cell_type.INPUT)

    layout.assign_cell_type(lattice_site(4, 2, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(6, 3, 1), sidb_technology.cell_type.NORMAL)

    layout.assign_cell_type(lattice_site(14, 3, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(16, 2, 1), sidb_technology.cell_type.NORMAL)

    layout.assign_cell_type(lattice_site(10, 6, 0), sidb_technology.cell_type.OUTPUT)
    layout.assign_cell_type(lattice_site(10, 7, 0), sidb_technology.cell_type.OUTPUT)

    layout.assign_cell_type(lattice_site(10, 9, 1), sidb_technology.cell_type.NORMAL)

    params = displacement_robustness_domain_params()

    params.displacement_variations = [1, 1]
    params.operational_params.simulation_parameters = sidb_simulation_parameters(2, -0.28)

    params.operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.maximum_distance = 2.0
    params.operational_params.input_bdl_iterator_params.bdl_wire_params.bdl_pairs_params.minimum_distance = 0.2

    # only the SiDBs at (4, 5) and (10, 12) are affected by displacement
    params.fixed_sidbs = {
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

    params.percentage_of_analyzed_displaced_layouts = 0.1
    params.dimer_policy = dimer_displacement_policy.ALLOW_OTHER_DIMER
    params.analysis_mode = displacement_analysis_mode.RANDOM

    stats = displacement_robustness_domain_stats()

    _ = determine_displacement_robustness_domain(layout, [create_and_tt()], params, stats)

    assert stats.num_non_operational_sidb_displacements + stats.num_operational_sidb_displacements == 8
