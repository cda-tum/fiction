# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction.sidb import DotTag, Lattice, LatticeSite, SiDBLayout, SimulationParams
from mnt.pyfiction.sidb.simulation.analysis import can_positive_charges_occur


def test_three_sidbs_100_lattice() -> None:
    """Check positive-charge feasibility on the Si(100) lattice."""
    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(1, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(2, 0, 0), DotTag.NORMAL)

    assert can_positive_charges_occur(layout, SimulationParams())

    params = SimulationParams()
    params.mu_minus = -0.8
    assert not can_positive_charges_occur(layout, params)


def test_three_sidbs_111_lattice() -> None:
    """Check positive-charge feasibility on the Si(111) lattice."""
    layout = SiDBLayout(Lattice.si_111_1x1())
    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(1, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(2, 0, 0), DotTag.NORMAL)

    params = SimulationParams()
    params.mu_minus = -0.05

    assert can_positive_charges_occur(layout, params)

    params.mu_minus = -0.8
    assert not can_positive_charges_occur(layout, params)
