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
    lattice,
    lattice_site,
    sidb_layout,
    sidb_simulation_parameters,
    sidb_technology,
)


def test_three_sidbs_100_lattice() -> None:
    """Check positive-charge feasibility on the Si(100) lattice."""
    layout = sidb_layout()
    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(1, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(2, 0, 0), sidb_technology.cell_type.NORMAL)

    assert can_positive_charges_occur(layout, sidb_simulation_parameters())

    params = sidb_simulation_parameters()
    params.mu_minus = -0.8
    assert not can_positive_charges_occur(layout, params)


def test_three_sidbs_111_lattice() -> None:
    """Check positive-charge feasibility on the Si(111) lattice."""
    layout = sidb_layout(lattice.si_111_1x1())
    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(1, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(2, 0, 0), sidb_technology.cell_type.NORMAL)

    params = sidb_simulation_parameters()
    params.mu_minus = -0.05

    assert can_positive_charges_occur(layout, params)

    params.mu_minus = -0.8
    assert not can_positive_charges_occur(layout, params)
