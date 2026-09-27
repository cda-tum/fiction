# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction.sidb import Lattice, LatticeSite, row_of, site_at_row, sites_in_area


def test_lattice_site() -> None:
    """Lattice sites support coordinates, ordering, arithmetic, and hashing."""
    origin = LatticeSite()
    assert (origin.x, origin.y, origin.z) == (0, 0, 0)

    site = LatticeSite(3, -4, 1)
    assert (site.x, site.y, site.z) == (3, -4, 1)
    assert LatticeSite(3, -4) == LatticeSite(3, -4, 0)
    assert repr(site) == "(3,-4,1)"

    assert LatticeSite(5, 0, 0) < LatticeSite(0, 0, 1) < LatticeSite(0, 1, 0)
    assert LatticeSite(1, 1, 1) + LatticeSite(2, 3, 1) == LatticeSite(3, 5, 0)
    assert LatticeSite(3, 4, 0) - LatticeSite(2, 3, 1) == LatticeSite(1, 0, 1)
    assert len({LatticeSite(0, 0, 0), LatticeSite(0, 0, 0), LatticeSite(0, 0, 1)}) == 2


def test_rows() -> None:
    """Rows cover every lattice site and reject unrepresentable coordinates."""
    assert row_of(LatticeSite(4, 3, 1)) == 7
    assert site_at_row(4, 7) == LatticeSite(4, 3, 1)
    assert site_at_row(0, -1) == LatticeSite(0, -1, 1)
    for site in (LatticeSite(0, -(2**31), 0), LatticeSite(0, 2**31 - 1, 1)):
        assert site_at_row(site.x, row_of(site)) == site
    with pytest.raises(IndexError):
        site_at_row(0, 2**32)
    assert sites_in_area(LatticeSite(0, 0, 0), LatticeSite(1, 0, 1)) == [
        LatticeSite(0, 0, 0),
        LatticeSite(1, 0, 0),
        LatticeSite(0, 0, 1),
        LatticeSite(1, 0, 1),
    ]


def test_basis_indices() -> None:
    """Constructors validate the basis; hashable lattice sites are immutable."""
    with pytest.raises(IndexError):
        LatticeSite(0, 0, 2)
    site = LatticeSite(0, 0, 1)
    for attribute in ("x", "y", "z"):
        with pytest.raises(AttributeError):
            setattr(site, attribute, 0)
        assert site.z == 1


def test_coordinate_ranges() -> None:
    """Python integers cannot wrap into fixed-width lattice coordinates."""
    for coordinate in (-(2**31) - 1, 2**31, 2**64):
        with pytest.raises(TypeError):
            LatticeSite(coordinate, 0)
        with pytest.raises(TypeError):
            LatticeSite(0, coordinate, 1)
    for basis_index in (-129, 128, 256):
        with pytest.raises(TypeError):
            LatticeSite(0, 0, basis_index)


def test_predefined_lattices() -> None:
    """Predefined silicon lattices map sites to nanometer positions."""
    si_100 = Lattice.si_100_2x1()
    assert si_100.name == "Si(100) 2x1"
    assert si_100.nm_position(LatticeSite(1, 1, 1)) == pytest.approx((0.384, 0.993))
    assert si_100.nm_distance(LatticeSite(0, 0, 0), LatticeSite(0, 0, 1)) == pytest.approx(0.225)

    si_111 = Lattice.si_111_1x1()
    assert si_111.name == "Si(111) 1x1"
    assert si_111.nm_position(LatticeSite(0, 0, 1)) == pytest.approx((0.33255, 0.192))
    assert si_111 != si_100


def test_custom_lattice() -> None:
    """Custom lattice vectors and basis sites determine physical positions."""
    square = Lattice("square", (5.0, 0.0), (0.0, 5.0), [(0.0, 0.0), (2.5, 2.5)])
    assert square.nm_position(LatticeSite(2, 3, 1)) == pytest.approx((1.25, 1.75))
    assert repr(square) == "square"


def test_arithmetic_boundaries() -> None:
    """Lattice-site arithmetic reports unrepresentable coordinates as IndexError."""
    with pytest.raises(IndexError):
        _ = LatticeSite(2**31 - 1, 0) + LatticeSite(1, 0)
    with pytest.raises(IndexError):
        _ = LatticeSite(0, -(2**31), 0) - LatticeSite(0, 0, 1)
