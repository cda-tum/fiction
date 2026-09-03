# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``check_simulation_results_for_equivalence``."""

from __future__ import annotations

import pytest

from mnt.pyfiction import (
    charge_distribution,
    check_simulation_results_for_equivalence,
    lattice,
    lattice_site,
    sidb_charge_state,
    sidb_layout,
    sidb_simulation_result,
    sidb_technology,
)


@pytest.mark.parametrize(
    "lat",
    [pytest.param(lattice.si_100_2x1(), id="100"), pytest.param(lattice.si_111_1x1(), id="111")],
)
def test_two_sidbs(lat):
    layout = sidb_layout(lat)
    layout.assign_cell_type(lattice_site(0, 0, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(0, 1, 1), sidb_technology.cell_type.NORMAL)

    first_result = sidb_simulation_result()
    second_result = sidb_simulation_result()

    first_result.charge_distributions = [charge_distribution(layout)]
    second_result.charge_distributions = [charge_distribution(layout)]

    assert check_simulation_results_for_equivalence(first_result, second_result)

    # a different charge state breaks the equivalence
    other = charge_distribution(layout)
    other.assign_charge_state(lattice_site(0, 1, 1), sidb_charge_state.NEUTRAL)
    second_result.charge_distributions = [other]

    assert not check_simulation_results_for_equivalence(first_result, second_result)

    # so does a different number of distributions
    second_result.charge_distributions = [charge_distribution(layout), other]

    assert not check_simulation_results_for_equivalence(first_result, second_result)
