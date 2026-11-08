# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction.sidb import lattice, lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.model import sidb_defect, sidb_defect_type


def test_empty_layout() -> None:
    """Empty layouts retain their lattice and name without dots or defects."""
    lyt = sidb_layout()
    assert lyt.is_empty()
    assert lyt.num_dots() == 0
    assert lyt.num_defects() == 0
    assert lyt.get_lattice() == lattice.si_100_2x1()
    assert lyt.get_dot_tag(lattice_site(0, 0, 0)) == sidb_dot_tag.EMPTY

    named = sidb_layout(lattice.si_111_1x1(), "named")
    assert named.get_lattice() == lattice.si_111_1x1()
    assert named.get_layout_name() == "named"


def test_dot_tags() -> None:
    """Dot tags determine layout traversal, terminals, bounds, and equality."""
    lyt = sidb_layout()
    lyt.assign_sidb(lattice_site(3, 1, 0), sidb_dot_tag.OUTPUT)
    lyt.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.INPUT)
    lyt.assign_sidb(lattice_site(1, 0, 0))

    assert lyt.num_dots() == 3
    assert lyt.get_dot_tag(lattice_site(1, 0, 0)) == sidb_dot_tag.NORMAL
    assert lyt.sidbs() == [lattice_site(0, 0, 0), lattice_site(1, 0, 0), lattice_site(3, 1, 0)]
    assert lyt.index_of(lattice_site(1, 0, 0)) == 1
    assert lyt.index_of(lattice_site(9, 9, 0)) is None
    assert lyt.pis() == [lattice_site(0, 0, 0)]
    assert lyt.pos() == [lattice_site(3, 1, 0)]
    assert lyt.num_pis() == 1
    assert lyt.is_po(lattice_site(3, 1, 0))
    assert lyt.bounding_box() == (lattice_site(0, 0, 0), lattice_site(3, 1, 0))

    lyt.assign_sidb(lattice_site(1, 0, 0), sidb_dot_tag.EMPTY)
    assert lyt.num_dots() == 2
    assert lyt.is_empty_site(lattice_site(1, 0, 0))

    copy = sidb_layout(lyt.get_lattice())
    for site in lyt.sidbs():
        copy.assign_sidb(site, lyt.get_dot_tag(site))
    assert copy == lyt
    assert hash(copy) == hash(lyt)
    assert "◯" in repr(lyt)

    lyt.assign_sidb(lattice_site(0, 0, 0))
    assert lyt.get_dot_tag(lattice_site(0, 0, 0)) == sidb_dot_tag.NORMAL
    assert lyt.num_dots() == 2
    assert lyt.num_pis() == 0


def test_defects() -> None:
    """Defects retain charge data and move between lattice sites."""
    lyt = sidb_layout()
    vacancy = sidb_defect(sidb_defect_type.SI_VACANCY, -1, 5.6, 5.0)
    lyt.assign_defect(lattice_site(5, 2, 0), vacancy)
    lyt.assign_defect(lattice_site(1, 0, 1), sidb_defect(sidb_defect_type.SILOXANE, 0))

    assert lyt.num_defects() == 2
    assert lyt.num_charged_defects() == 1
    assert lyt.num_neutral_defects() == 1
    assert lyt.get_defect(lattice_site(5, 2, 0)) == vacancy
    assert lyt.get_defect(lattice_site(9, 9, 0)).type == sidb_defect_type.NONE
    assert len(lyt.affected_sidbs(lattice_site(1, 0, 1))) == 3
    assert len(lyt.affected_sidbs(lattice_site(5, 2, 0), (1, 1))) == 9
    assert lyt.defects()[0][0] == lattice_site(1, 0, 1)

    lyt.move_defect(lattice_site(5, 2, 0), lattice_site(6, 2, 0))
    assert lyt.get_defect(lattice_site(6, 2, 0)) == vacancy
    assert lyt.bounding_box() == (lattice_site(1, 0, 1), lattice_site(6, 2, 0))


def test_defect_move_erase_and_neutral_charge_counts() -> None:
    """Defect movement preserves values, empty moves do nothing, and NONE erases a defect."""
    layout = sidb_layout()
    first, second = lattice_site(), lattice_site(5, 2)
    positive = sidb_defect(sidb_defect_type.UNKNOWN, 1)
    vacancy = sidb_defect(sidb_defect_type.SI_VACANCY)
    layout.assign_defect(first, positive)
    layout.assign_defect(second, vacancy)
    assert layout.num_charged_defects() == 2
    assert layout.num_neutral_defects() == 1
    assert layout.num_positively_charged_defects() == 1
    assert layout.num_negatively_charged_defects() == 0
    expected = [(first, positive), (second, vacancy)]
    layout.move_defect(lattice_site(9, 9), first)
    layout.move_defect(second, second)
    assert layout.defects() == expected
    layout.move_defect(layout.defects()[0][0], layout.defects()[1][0])
    assert layout.defects() == [(second, positive)]
    layout.assign_defect(second, sidb_defect(sidb_defect_type.NONE))
    assert layout.defects() == []


@pytest.mark.parametrize(("coordinate", "offsets", "basis"), [(-(2**31), [0, 1], 0), (2**31 - 1, [-1, 0], 1)])
def test_defect_extent_clips_coordinate_limits(coordinate: int, offsets: list[int], basis: int) -> None:
    """Affected sites stop at representable coordinates without wrapping or changing basis."""
    layout = sidb_layout()
    site = lattice_site(coordinate, coordinate, basis)
    layout.assign_defect(site, sidb_defect(sidb_defect_type.SI_VACANCY))
    assert set(layout.affected_sidbs(site, (1, 1))) == {
        lattice_site(coordinate + x, coordinate + y, basis) for x in offsets for y in offsets
    }


def test_complete_affected_sites() -> None:
    """Affected-site union preserves both defect extents and lattice basis indices."""
    layout = sidb_layout()
    charged_site, neutral_site = lattice_site(5, 2), lattice_site(1, 0, 1)
    layout.assign_defect(charged_site, sidb_defect(sidb_defect_type.SI_VACANCY))
    layout.assign_defect(neutral_site, sidb_defect(sidb_defect_type.SILOXANE))
    expected = {lattice_site(x, y) for x in (4, 5, 6) for y in (1, 2, 3)}
    assert set(layout.affected_sidbs(charged_site, (1, 1))) == expected
    assert set(layout.all_affected_sidbs((1, 1), (0, 0))) == expected | {neutral_site}
    assert layout.affected_sidbs(lattice_site(9, 9)) == set()
