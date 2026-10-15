# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction.sidb import lattice, lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.simulation.analysis import physical_population_stability, physical_population_stability_params


def test_population_stability_documents_python_exception() -> None:
    """Document the exception raised for invalid analysis parameters."""
    documentation = physical_population_stability.__doc__
    assert documentation is not None
    assert "ValueError:" in documentation
    assert "std::invalid_argument" not in documentation
    params = physical_population_stability_params()
    params.precision_for_distance_corresponding_to_potential = 16
    with pytest.raises(ValueError, match="potential-to-distance conversion"):
        physical_population_stability(sidb_layout(), params)


def test_three_sidbs_100_lattice() -> None:
    """Check population stability on the Si(100) lattice."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(0, 1, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(1, 0, 1), sidb_dot_tag.NORMAL)
    params = physical_population_stability_params()
    params.simulation_parameters.mu_minus = -0.25
    result = physical_population_stability(layout, params)
    assert len(result) == 5
    assert result[0].critical_dot in layout.sidbs()
    assert result[0].system_energy <= result[1].system_energy
    assert result[1].system_energy <= result[2].system_energy

    params.simulation_parameters.mu_minus = -0.32
    result = physical_population_stability(layout, params)
    assert len(result) == 1


def test_three_sidbs_111_lattice() -> None:
    """Check population stability on the Si(111) lattice."""
    layout = sidb_layout(lattice.si_111_1x1())
    layout.assign_sidb(lattice_site(0, 0, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(0, 1, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(1, 0, 1), sidb_dot_tag.NORMAL)
    params = physical_population_stability_params()
    params.simulation_parameters.mu_minus = -0.25
    result = physical_population_stability(layout, params)
    assert len(result) == 5
    assert result[0].critical_dot in layout.sidbs()
    assert result[0].system_energy <= result[1].system_energy
    assert result[1].system_energy <= result[2].system_energy

    params.simulation_parameters.mu_minus = -0.32
    result = physical_population_stability(layout, params)
    assert len(result) == 2
