# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import Any

from mnt.pyfiction.sidb import lattice, lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.simulation.analysis import physical_population_stability


def test_three_sidbs_100_lattice() -> None:
    """Check population stability on the Si(100) lattice."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(0, 1, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(1, 0, 1), sidb_dot_tag.NORMAL)
    options: dict[str, Any] = {
        "mu_minus": -0.25,
    }
    result = physical_population_stability(layout, **options)
    assert len(result) == 5
    assert result[0].critical_dot in layout.sidbs()
    assert result[0].system_energy <= result[1].system_energy
    assert result[1].system_energy <= result[2].system_energy

    options["mu_minus"] = -0.32
    result = physical_population_stability(layout, **options)
    assert len(result) == 1


def test_three_sidbs_111_lattice() -> None:
    """Check population stability on the Si(111) lattice."""
    layout = sidb_layout(lattice.si_111_1x1())
    layout.assign_sidb(lattice_site(0, 0, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(0, 1, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(1, 0, 1), sidb_dot_tag.NORMAL)
    options: dict[str, Any] = {
        "mu_minus": -0.25,
    }
    result = physical_population_stability(layout, **options)
    assert len(result) == 5
    assert result[0].critical_dot in layout.sidbs()
    assert result[0].system_energy <= result[1].system_energy
    assert result[1].system_energy <= result[2].system_energy

    options["mu_minus"] = -0.32
    result = physical_population_stability(layout, **options)
    assert len(result) == 2
