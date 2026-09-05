# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction import (
    lattice,
    lattice_site,
    physical_population_stability,
    physical_population_stability_params,
    sidb_layout,
    sidb_technology,
)


def test_three_sidbs_100_lattice() -> None:
    """Check population stability on the Si(100) lattice."""
    layout = sidb_layout()
    layout.assign_cell_type(lattice_site(0, 0, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(0, 1, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(1, 0, 1), sidb_technology.cell_type.NORMAL)
    params = physical_population_stability_params()
    params.simulation_parameters.mu_minus = -0.25
    result = physical_population_stability(layout, params)
    assert len(result) == 5
    assert result[0].system_energy <= result[1].system_energy
    assert result[1].system_energy <= result[2].system_energy

    params.simulation_parameters.mu_minus = -0.32
    result = physical_population_stability(layout, params)
    assert len(result) == 1


def test_three_sidbs_111_lattice() -> None:
    """Check population stability on the Si(111) lattice."""
    layout = sidb_layout(lattice.si_111_1x1())
    layout.assign_cell_type(lattice_site(0, 0, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(0, 1, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(1, 0, 1), sidb_technology.cell_type.NORMAL)
    params = physical_population_stability_params()
    params.simulation_parameters.mu_minus = -0.25
    result = physical_population_stability(layout, params)
    assert len(result) == 5
    assert result[0].system_energy <= result[1].system_energy
    assert result[1].system_energy <= result[2].system_energy

    params.simulation_parameters.mu_minus = -0.32
    result = physical_population_stability(layout, params)
    assert len(result) == 2
