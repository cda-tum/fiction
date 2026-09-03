# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction import lattice, lattice_site, row_of, site_at_row, sites_in_area


def test_lattice_site() -> None:
    origin = lattice_site()
    assert (origin.x, origin.y, origin.z) == (0, 0, 0)

    site = lattice_site(3, -4, 1)
    assert (site.x, site.y, site.z) == (3, -4, 1)
    assert lattice_site(3, -4) == lattice_site(3, -4, 0)
    assert repr(site) == "(3,-4,1)"

    assert lattice_site(5, 0, 0) < lattice_site(0, 0, 1) < lattice_site(0, 1, 0)
    assert lattice_site(1, 1, 1) + lattice_site(2, 3, 1) == lattice_site(3, 5, 0)
    assert lattice_site(3, 4, 0) - lattice_site(2, 3, 1) == lattice_site(1, 0, 1)
    assert len({lattice_site(0, 0, 0), lattice_site(0, 0, 0), lattice_site(0, 0, 1)}) == 2


def test_rows() -> None:
    assert row_of(lattice_site(4, 3, 1)) == 7
    assert site_at_row(4, 7) == lattice_site(4, 3, 1)
    assert site_at_row(0, -1) == lattice_site(0, -1, 1)
    assert sites_in_area(lattice_site(0, 0, 0), lattice_site(1, 0, 1)) == [
        lattice_site(0, 0, 0),
        lattice_site(1, 0, 0),
        lattice_site(0, 0, 1),
        lattice_site(1, 0, 1),
    ]


def test_predefined_lattices() -> None:
    si_100 = lattice.si_100_2x1()
    assert si_100.name == "Si(100) 2x1"
    assert si_100.nm_position(lattice_site(1, 1, 1)) == pytest.approx((0.384, 0.993))
    assert si_100.nm_distance(lattice_site(0, 0, 0), lattice_site(0, 0, 1)) == pytest.approx(0.225)

    si_111 = lattice.si_111_1x1()
    assert si_111.name == "Si(111) 1x1"
    assert si_111.nm_position(lattice_site(0, 0, 1)) == pytest.approx((0.33255, 0.192))
    assert si_111 != si_100


def test_custom_lattice() -> None:
    square = lattice("square", (5.0, 0.0), (0.0, 5.0), [(0.0, 0.0), (2.5, 2.5)])
    assert square.nm_position(lattice_site(2, 3, 1)) == pytest.approx((1.25, 1.75))
    assert repr(square) == "square"
