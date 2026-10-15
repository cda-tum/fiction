# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``sidb_simulation_result.groundstates``."""

from __future__ import annotations

import pytest

from mnt.pyfiction.sidb import charge_distribution, lattice, lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.model import sidb_charge_state
from mnt.pyfiction.sidb.simulation import potential_landscape, sidb_simulation_result


@pytest.mark.parametrize(
    "lat",
    [pytest.param(lattice.si_100_2x1(), id="100"), pytest.param(lattice.si_111_1x1(), id="111")],
)
def test_three_sidbs(lat):
    layout = sidb_layout(lat)

    layout.assign_sidb(lattice_site(0, 0, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(4, 0, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(6, 0, 1), sidb_dot_tag.NORMAL)

    landscape = potential_landscape(layout, base=3, mu_minus=-0.32, epsilon_r=5.6, lambda_tf=5.0)

    cd1 = landscape.evaluate(charge_distribution(layout))  # all negative
    cd2 = landscape.evaluate(charge_distribution(layout, sidb_charge_state.NEUTRAL))  # all neutral
    cd3 = charge_distribution(layout)
    cd3.assign_charge_state(lattice_site(6, 0, 1), sidb_charge_state.NEUTRAL)  # only two SiDBs are negative
    cd3 = landscape.evaluate(cd3)

    assert cd1.energy() > cd3.energy() > cd2.energy() == 0.0

    results = sidb_simulation_result()
    results.layout = layout
    results.charge_distributions = [cd1, cd2, cd3]

    ground_state = results.groundstates()
    assert len(ground_state) == 1

    groundstate = ground_state[0]
    assert groundstate.get_charge_state(lattice_site(0, 0, 1)) == sidb_charge_state.NEUTRAL
    assert groundstate.get_charge_state(lattice_site(4, 0, 1)) == sidb_charge_state.NEUTRAL
    assert groundstate.get_charge_state(lattice_site(6, 0, 1)) == sidb_charge_state.NEUTRAL
