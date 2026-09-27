# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction.sidb import DotTag, Lattice, LatticeSite, SiDBLayout
from mnt.pyfiction.sidb.analysis import PhysicalPopulationStabilityParams, physical_population_stability


def test_three_sidbs_100_lattice() -> None:
    """Check population stability on the Si(100) lattice."""
    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(0, 0, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(0, 1, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(1, 0, 1), DotTag.NORMAL)
    params = PhysicalPopulationStabilityParams()
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
    layout = SiDBLayout(Lattice.si_111_1x1())
    layout.assign_sidb(LatticeSite(0, 0, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(0, 1, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(1, 0, 1), DotTag.NORMAL)
    params = PhysicalPopulationStabilityParams()
    params.simulation_parameters.mu_minus = -0.25
    result = physical_population_stability(layout, params)
    assert len(result) == 5
    assert result[0].critical_dot in layout.sidbs()
    assert result[0].system_energy <= result[1].system_energy
    assert result[1].system_energy <= result[2].system_energy

    params.simulation_parameters.mu_minus = -0.32
    result = physical_population_stability(layout, params)
    assert len(result) == 2
