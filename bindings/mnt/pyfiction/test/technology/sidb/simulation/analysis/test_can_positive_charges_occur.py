# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction import (
    can_positive_charges_occur,
    sidb_100_lattice,
    sidb_111_lattice,
    sidb_simulation_parameters,
    sidb_technology,
)


def test_three_sidbs_100_lattice():
    layout = sidb_100_lattice((2, 3))
    layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((1, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((2, 0), sidb_technology.cell_type.NORMAL)

    assert can_positive_charges_occur(layout, sidb_simulation_parameters())

    params = sidb_simulation_parameters()
    params.mu_minus = -0.8
    assert not can_positive_charges_occur(layout, params)


def test_three_sidbs_111_lattice():
    layout = sidb_111_lattice((2, 3))
    layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((1, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((2, 0), sidb_technology.cell_type.NORMAL)

    params = sidb_simulation_parameters()
    params.mu_minus = -0.05

    assert can_positive_charges_occur(layout, params)

    params.mu_minus = -0.8
    assert not can_positive_charges_occur(layout, params)
