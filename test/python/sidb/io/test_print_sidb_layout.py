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

from mnt.pyfiction.sidb import charge_distribution, lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.io import print_sidb_layout
from mnt.pyfiction.sidb.model import sidb_charge_state, sidb_defect, sidb_defect_type

CHARGED = "●"
"""What a charged SiDB looks like; only the color tells a negative one from a positive one."""
UNCHARGED = "◯"
"""What an SiDB without a charge distribution, and a neutral one with it, look like."""
LATTICE_DOT = "·"
"""What an empty lattice site looks like."""
ESCAPE = "\x1b"
"""The first byte of a color escape sequence."""


@pytest.fixture
def two_sidb_layout() -> sidb_layout:
    """Two SiDBs on the H-Si(100) 2x1 lattice, far enough apart to hold different charges.

    Returns:
        The layout.
    """
    lyt = sidb_layout()
    lyt.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(6, 0, 0), sidb_dot_tag.NORMAL)
    return lyt


def test_prints_the_dots_of_an_uncharged_layout(two_sidb_layout: sidb_layout) -> None:
    picture = print_sidb_layout(two_sidb_layout, lat_color=False)
    assert picture.count(UNCHARGED) == 2
    assert ESCAPE not in picture


def test_draws_the_empty_lattice_sites_by_default(two_sidb_layout: sidb_layout) -> None:
    """``draw_lattice`` defaults to ``True``, exactly as the C++ header does."""
    with_lattice = print_sidb_layout(two_sidb_layout, lat_color=False)
    without_lattice = print_sidb_layout(two_sidb_layout, lat_color=False, draw_lattice=False)
    assert LATTICE_DOT in with_lattice
    assert LATTICE_DOT not in without_lattice


def test_colors_by_default(two_sidb_layout: sidb_layout) -> None:
    """``lat_color`` defaults to ``True``, exactly as the C++ header does."""
    assert ESCAPE in print_sidb_layout(two_sidb_layout)
    assert ESCAPE not in print_sidb_layout(two_sidb_layout, lat_color=False)


def test_crop_layout_pads_the_bounding_box(two_sidb_layout: sidb_layout) -> None:
    tight = print_sidb_layout(two_sidb_layout, lat_color=False, crop_layout=False)
    padded = print_sidb_layout(two_sidb_layout, lat_color=False, crop_layout=True)
    assert padded != tight


def test_a_charge_distribution_replaces_the_dots(two_sidb_layout: sidb_layout) -> None:
    """A charge-annotated picture is one picture, with the charge symbols where the dots were."""
    charges = charge_distribution(two_sidb_layout)
    charges.assign_charge_state(lattice_site(0, 0, 0), sidb_charge_state.NEUTRAL)
    charges.assign_charge_state(lattice_site(6, 0, 0), sidb_charge_state.POSITIVE)
    picture = print_sidb_layout(two_sidb_layout, charges, lat_color=False)
    assert picture.count(UNCHARGED) == 1, picture
    assert picture.count(CHARGED) == 1, picture


def test_an_empty_layout_prints_nothing_but_a_note() -> None:
    picture = print_sidb_layout(sidb_layout(), lat_color=False)
    assert picture == "[i] empty layout\n"


@pytest.mark.parametrize("coordinate", [-(2**31), 2**31 - 1])
def test_text_padding_boundaries(coordinate: int) -> None:
    """Unpadded sites at coordinate limits print, while padding raises IndexError."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site(coordinate, coordinate))
    assert print_sidb_layout(layout, lat_color=False, crop_layout=False) == " ◯ \n"
    with pytest.raises(IndexError):
        print_sidb_layout(layout, lat_color=False, crop_layout=True)


def test_charge_defect_and_dot_priority() -> None:
    """Charge states take priority over overlapping defects, followed by defect symbols and dots."""
    layout = sidb_layout()
    for x, tag in enumerate((sidb_dot_tag.INPUT, sidb_dot_tag.NORMAL, sidb_dot_tag.OUTPUT)):
        layout.assign_sidb(lattice_site(x, 0), tag)
    layout.assign_defect(lattice_site(), sidb_defect(sidb_defect_type.UNKNOWN, 1))
    layout.assign_defect(lattice_site(1, 0), sidb_defect(sidb_defect_type.UNKNOWN, -1))
    charges = charge_distribution(layout, sidb_charge_state.NONE)
    charges.assign_charge_state(lattice_site(), sidb_charge_state.NEGATIVE)
    assert print_sidb_layout(layout, charges, lat_color=False, crop_layout=False) == " ●  ⊟  ◯ \n"


def test_printer_rejects_shifted_distribution() -> None:
    """A charge distribution must refer to the same sites as the printed layout."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site())
    shifted = sidb_layout()
    shifted.assign_sidb(lattice_site(1, 0))
    with pytest.raises(ValueError):  # ruff: ignore[pytest-raises-too-broad] -- error type is the public contract
        print_sidb_layout(layout, charge_distribution(shifted))
