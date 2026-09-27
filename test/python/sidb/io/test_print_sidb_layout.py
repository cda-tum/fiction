# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of the textual SiDB layout printer."""

from __future__ import annotations

import pytest

from mnt.pyfiction.sidb import ChargeState, DotTag, LatticeSite, SiDBLayout
from mnt.pyfiction.sidb.io import print_sidb_layout
from mnt.pyfiction.sidb.simulation import PotentialLandscape

CHARGED = "●"
"""What a charged SiDB looks like; only the color tells a negative one from a positive one."""
UNCHARGED = "◯"
"""What an SiDB without a charge distribution, and a neutral one with it, look like."""
LATTICE_DOT = "·"
"""What an empty lattice site looks like."""
ESCAPE = "\x1b"
"""The first byte of a color escape sequence."""


@pytest.fixture
def two_sidb_layout() -> SiDBLayout:
    """Two SiDBs on the H-Si(100) 2x1 lattice, far enough apart to hold different charges.

    Returns:
        The layout.
    """
    lyt = SiDBLayout()
    lyt.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)
    lyt.assign_sidb(LatticeSite(6, 0, 0), DotTag.NORMAL)
    return lyt


def test_prints_the_dots_of_an_uncharged_layout(two_sidb_layout: SiDBLayout) -> None:
    picture = print_sidb_layout(two_sidb_layout, lat_color=False)
    assert picture.count(UNCHARGED) == 2
    assert ESCAPE not in picture


def test_draws_the_empty_lattice_sites_by_default(two_sidb_layout: SiDBLayout) -> None:
    """``draw_lattice`` defaults to ``True``, exactly as the C++ header does."""
    with_lattice = print_sidb_layout(two_sidb_layout, lat_color=False)
    without_lattice = print_sidb_layout(two_sidb_layout, lat_color=False, draw_lattice=False)
    assert LATTICE_DOT in with_lattice
    assert LATTICE_DOT not in without_lattice


def test_colors_by_default(two_sidb_layout: SiDBLayout) -> None:
    """``lat_color`` defaults to ``True``, exactly as the C++ header does."""
    assert ESCAPE in print_sidb_layout(two_sidb_layout)
    assert ESCAPE not in print_sidb_layout(two_sidb_layout, lat_color=False)


def test_crop_layout_pads_the_bounding_box(two_sidb_layout: SiDBLayout) -> None:
    tight = print_sidb_layout(two_sidb_layout, lat_color=False, crop_layout=False)
    padded = print_sidb_layout(two_sidb_layout, lat_color=False, crop_layout=True)
    assert padded != tight


def test_a_charge_distribution_replaces_the_dots(two_sidb_layout: SiDBLayout) -> None:
    """A charge-annotated picture is one picture, with the charge symbols where the dots were."""
    charges = PotentialLandscape(two_sidb_layout).evaluate([ChargeState.NEUTRAL, ChargeState.POSITIVE])
    picture = print_sidb_layout(two_sidb_layout, charges, lat_color=False)
    assert picture.count(UNCHARGED) == 1, picture
    assert picture.count(CHARGED) == 1, picture


def test_an_empty_layout_prints_nothing_but_a_note() -> None:
    picture = print_sidb_layout(SiDBLayout(), lat_color=False)
    assert picture.strip()
