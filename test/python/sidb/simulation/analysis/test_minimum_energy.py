# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``minimum_energy``."""

from __future__ import annotations

import pytest

from mnt.pyfiction.sidb import (
    ChargeState,
    DotTag,
    Lattice,
    LatticeSite,
    SiDBLayout,
    SimulationParams,
)
from mnt.pyfiction.sidb.analysis import minimum_energy
from mnt.pyfiction.sidb.simulation import PotentialLandscape


@pytest.mark.parametrize(
    "lat",
    [pytest.param(Lattice.si_100_2x1(), id="100"), pytest.param(Lattice.si_111_1x1(), id="111")],
)
def test_three_sidbs(lat):
    layout = SiDBLayout(lat)

    layout.assign_sidb(LatticeSite(0, 0, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(4, 0, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(6, 0, 1), DotTag.NORMAL)

    landscape = PotentialLandscape(layout, params=SimulationParams())

    cd1 = landscape.evaluate([ChargeState.NEGATIVE] * layout.num_dots())  # all negative
    cd2 = landscape.evaluate([ChargeState.NEUTRAL] * layout.num_dots())  # all neutral
    cd3 = landscape.evaluate([ChargeState.NEGATIVE, ChargeState.NEGATIVE, ChargeState.NEUTRAL])

    cd4 = landscape.evaluate([ChargeState.NEUTRAL, ChargeState.POSITIVE, ChargeState.NEGATIVE])

    result = minimum_energy([cd1, cd2, cd3, cd4])

    assert result == pytest.approx(min(cd.energy for cd in [cd1, cd2, cd3, cd4]))
    assert result == pytest.approx(cd4.energy)
    assert result < 0.0
