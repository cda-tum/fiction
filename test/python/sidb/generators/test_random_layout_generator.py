# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import Any

import pytest

from mnt.pyfiction.sidb import lattice, lattice_site, row_of, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.generators import (
    generate_multiple_random_sidb_layouts,
    generate_random_sidb_layout,
    positive_charges,
)
from mnt.pyfiction.sidb.model import sidb_defect, sidb_defect_type
from mnt.pyfiction.sidb.simulation.analysis import can_positive_charges_occur


def test_area_with_one_sidb_100_lattice() -> None:
    """A one-site H-Si(100)-2x1 area fixes the generated SiDB position."""
    result_lyt = generate_random_sidb_layout(
        sidb_layout(), number_of_sidbs=1, coordinate_pair=(lattice_site(10, 5, 0), lattice_site(10, 5, 0))
    )
    assert result_lyt is not None
    assert result_lyt.num_dots() == 1
    assert result_lyt.sidbs() == [lattice_site(10, 5, 0)]


def test_area_with_five_sidb_100_lattice() -> None:
    """The generator places five SiDBs on an H-Si(100)-2x1 lattice."""
    result_lyt = generate_random_sidb_layout(
        sidb_layout(), number_of_sidbs=5, coordinate_pair=(lattice_site(0, 0, 0), lattice_site(10, 5, 0))
    )
    assert result_lyt is not None
    assert result_lyt.num_dots() == 5


def test_area_with_one_coordinate_111_lattice() -> None:
    """A one-site H-Si(111)-1x1 area fixes the generated SiDB position."""
    result_lyt = generate_random_sidb_layout(
        sidb_layout(lattice.si_111_1x1()),
        number_of_sidbs=1,
        coordinate_pair=(lattice_site(10, 5, 0), lattice_site(10, 5, 0)),
    )
    assert result_lyt is not None
    assert result_lyt.num_dots() == 1
    assert result_lyt.sidbs() == [lattice_site(10, 5, 0)]


def test_impossible_design_of_single_layout() -> None:
    """The generator returns None when the requested SiDBs do not fit."""
    result_lyt = generate_random_sidb_layout(sidb_layout(), number_of_sidbs=2)
    assert result_lyt is None


def test_impossible_design_of_multiple_layouts() -> None:
    """Repeated generation returns None when the requested SiDBs do not fit."""
    result_lyt = generate_multiple_random_sidb_layouts(
        sidb_layout(), maximal_attempts_for_multiple_layouts=5, number_of_sidbs=2
    )
    assert result_lyt is None


@pytest.mark.parametrize("option", ["number_of_unique_generated_layouts", "maximal_attempts_for_multiple_layouts"])
def test_single_layout_rejects_multiple_layout_options(option: str) -> None:
    """Single-layout generation accepts only options used for one layout."""
    options: dict[str, Any] = {option: 1}
    with pytest.raises(TypeError):
        generate_random_sidb_layout(**options)


def test_default_generation_is_empty() -> None:
    """Default options request no SiDBs and return an empty layout."""
    layout = generate_random_sidb_layout()
    assert layout is not None
    assert layout.sidbs() == []


@pytest.mark.parametrize("restriction", [positive_charges.ALLOWED, positive_charges.FORBIDDEN])
def test_generated_sites_stay_in_area(restriction: positive_charges) -> None:
    """Generation preserves site uniqueness, the requested area, and the charge restriction."""
    layout = generate_random_sidb_layout(
        number_of_sidbs=5,
        positive_sidbs=restriction,
        coordinate_pair=(lattice_site(0, 0, 1), lattice_site(50, 20, 1)),
        maximal_attempts=10000,
    )
    assert layout is not None
    assert len(set(layout.sidbs())) == layout.num_dots() == 5
    assert all(0 <= site.x <= 50 and 1 <= row_of(site) <= 41 for site in layout.sidbs())
    if restriction == positive_charges.FORBIDDEN:
        assert not can_positive_charges_occur(layout)


def test_generation_preserves_skeleton() -> None:
    """An occupied canvas site retains its input tag while generation fills the free site."""
    skeleton = sidb_layout()
    skeleton.assign_sidb(lattice_site(), sidb_dot_tag.INPUT)
    result = generate_random_sidb_layout(
        skeleton, coordinate_pair=(lattice_site(), lattice_site(1, 0)), number_of_sidbs=1, maximal_attempts=10000
    )
    assert result is not None
    assert result.sidbs() == [lattice_site(), lattice_site(1, 0)]
    assert result.get_dot_tag(lattice_site()) == sidb_dot_tag.INPUT
    assert skeleton.sidbs() == [lattice_site()]


def test_required_positive_charges_obey_attempt_limit() -> None:
    """A single isolated SiDB cannot satisfy the positive-charge requirement."""
    assert (
        generate_random_sidb_layout(number_of_sidbs=1, positive_sidbs=positive_charges.MAY_OCCUR, maximal_attempts=2)
        is None
    )


@pytest.mark.parametrize("blocked", [False, True])
def test_generation_preserves_defects(*, blocked: bool) -> None:
    """A defect blocks its site while a defect outside the canvas survives generation."""
    skeleton = sidb_layout()
    site = lattice_site(2, 1, 1)
    defect_site = site if blocked else lattice_site(3, 1, 1)
    defect = sidb_defect(sidb_defect_type.DB, -1, 5.6, 5.0)
    skeleton.assign_defect(defect_site, defect)
    result = generate_random_sidb_layout(
        skeleton,
        coordinate_pair=(site, site),
        number_of_sidbs=1,
        positive_sidbs=positive_charges.FORBIDDEN,
        maximal_attempts=5,
    )
    if blocked:
        assert result is None
    else:
        assert result is not None
        assert result.sidbs() == [site]
        assert result.get_dot_tag(site) == sidb_dot_tag.LOGIC
        assert result.defects() == [(defect_site, defect)]
    assert skeleton.sidbs() == []


def test_multiple_layouts_are_unique_and_preserve_defects() -> None:
    """Distinct generated layouts preserve defects and leave neutral defect neighborhoods empty."""
    skeleton = sidb_layout()
    skeleton.assign_defect(lattice_site(2, 2), sidb_defect(sidb_defect_type.DB, -1, 5.6, 5.0))
    skeleton.assign_defect(lattice_site(4, 1), sidb_defect(sidb_defect_type.SINGLE_DIHYDRIDE))
    reports = []
    layouts = generate_multiple_random_sidb_layouts(
        skeleton,
        number_of_sidbs=5,
        coordinate_pair=(lattice_site(), lattice_site(10, 2)),
        number_of_unique_generated_layouts=3,
        maximal_attempts=10000,
        maximal_attempts_for_multiple_layouts=100,
        on_progress=lambda task, done, total: reports.append((task, done, total)),
    )
    assert layouts is not None
    assert len(layouts) == len(set(layouts)) == 3
    for layout in layouts:
        assert layout.num_dots() == 5
        assert layout.defects() == skeleton.defects()
        assert all(
            layout.is_empty_site(site) for site in [lattice_site(2, 2), *(lattice_site(x, 1) for x in (3, 4, 5))]
        )
    assert ("layouts", 3, 3) in reports


def test_generation_reports_placed_sidbs() -> None:
    """Placement progress completes at the requested SiDB count."""
    reports = []
    result = generate_random_sidb_layout(
        number_of_sidbs=5,
        coordinate_pair=(lattice_site(1, 1), lattice_site(5, 7, 1)),
        on_progress=lambda task, done, total: reports.append((task, done, total)),
    )
    assert result is not None
    assert result.num_dots() == 5
    placement = [(done, total) for task, done, total in reports if task == "placed SiDBs"]
    assert placement == sorted(placement)
    assert placement[-1] == (5, 5)
