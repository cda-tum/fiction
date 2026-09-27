# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``calculate_energy_distribution``."""

from __future__ import annotations

from itertools import pairwise

import pytest

from mnt.pyfiction.sidb import (
    ChargeState,
    DotTag,
    Lattice,
    LatticeSite,
    SiDBLayout,
    SimulationParams,
)
from mnt.pyfiction.sidb.simulation import PotentialLandscape
from mnt.pyfiction.sidb.simulation.analysis import calculate_energy_distribution


@pytest.mark.parametrize(
    ("lat", "all_negative_energy"),
    [
        pytest.param(Lattice.si_100_2x1(), 0.48066663155586997, id="100"),
        pytest.param(Lattice.si_111_1x1(), 0.233980661373219, id="111"),
    ],
)
def test_three_sidbs(lat, all_negative_energy):
    layout = SiDBLayout(lat)

    layout.assign_sidb(LatticeSite(0, 0, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(4, 0, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(6, 0, 1), DotTag.NORMAL)

    landscape = PotentialLandscape(layout, params=SimulationParams())

    cd1 = landscape.evaluate([ChargeState.NEGATIVE] * layout.num_dots())  # all negative
    cd2 = landscape.evaluate([ChargeState.NEUTRAL] * layout.num_dots())  # all neutral
    cd3 = landscape.evaluate([ChargeState.NEGATIVE, ChargeState.NEGATIVE, ChargeState.NEUTRAL])
    cd4 = landscape.evaluate([ChargeState.NEUTRAL, ChargeState.POSITIVE, ChargeState.NEGATIVE])

    assert cd1.energy == pytest.approx(all_negative_energy, abs=1e-7)
    assert cd2.energy == pytest.approx(0.0, abs=1e-7)
    assert 0.0 < cd3.energy < cd1.energy
    assert cd4.energy < 0.0  # the positive SiDB attracts its negative neighbor

    distribution = calculate_energy_distribution([cd1, cd2, cd3, cd4])

    assert distribution.size() == 4
    assert distribution.min_energy() == pytest.approx(cd4.energy, abs=1e-7)
    assert distribution.max_energy() == pytest.approx(cd1.energy, abs=1e-7)

    states = []
    for i in range(distribution.size()):
        state = distribution.get_nth_state(i)
        assert state is not None
        states.append(state)
    assert all(s.degeneracy == 1 for s in states)
    assert all(
        lower.electrostatic_potential_energy < higher.electrostatic_potential_energy
        for lower, higher in pairwise(states)
    )


def test_degenerate_states():
    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(10, 0, 0), DotTag.NORMAL)

    landscape = PotentialLandscape(layout)
    left = landscape.evaluate([ChargeState.NEGATIVE, ChargeState.NEUTRAL])
    right = landscape.evaluate([ChargeState.NEUTRAL, ChargeState.NEGATIVE])
    excited = landscape.evaluate([ChargeState.NEGATIVE, ChargeState.NEGATIVE])

    distribution = calculate_energy_distribution([excited, left, right])

    assert distribution.size() == 2
    first = distribution.get_nth_state(0)
    second = distribution.get_nth_state(1)
    assert first is not None
    assert second is not None
    assert first.electrostatic_potential_energy == pytest.approx(0.0)
    assert first.degeneracy == 2
    assert second.degeneracy == 1
