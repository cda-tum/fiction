# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction.sidb import lattice, lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.simulation.analysis import can_positive_charges_occur


def test_three_sidbs_100_lattice() -> None:
    """Check positive-charge feasibility on the Si(100) lattice."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(1, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(2, 0, 0), sidb_dot_tag.NORMAL)

    assert can_positive_charges_occur(layout, base=3, mu_minus=-0.32, epsilon_r=5.6, lambda_tf=5.0)

    options = {"base": 3, "mu_minus": -0.32, "epsilon_r": 5.6, "lambda_tf": 5.0}
    options["mu_minus"] = -0.8
    assert not can_positive_charges_occur(layout, **options)


def test_three_sidbs_111_lattice() -> None:
    """Check positive-charge feasibility on the Si(111) lattice."""
    layout = sidb_layout(lattice.si_111_1x1())
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(1, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(2, 0, 0), sidb_dot_tag.NORMAL)

    options = {"base": 3, "mu_minus": -0.32, "epsilon_r": 5.6, "lambda_tf": 5.0}
    options["mu_minus"] = -0.05

    assert can_positive_charges_occur(layout, **options)

    options["mu_minus"] = -0.8
    assert not can_positive_charges_occur(layout, **options)
