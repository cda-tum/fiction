# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction.sidb import Defect, DefectType, DotTag, Lattice, LatticeSite, SiDBLayout


def test_empty_layout() -> None:
    """Empty layouts retain their lattice and name without dots or defects."""
    lyt = SiDBLayout()
    assert lyt.is_empty()
    assert lyt.num_dots() == 0
    assert lyt.num_defects() == 0
    assert lyt.lattice == Lattice.si_100_2x1()
    assert lyt.get_dot_tag(LatticeSite(0, 0, 0)) == DotTag.EMPTY

    named = SiDBLayout(Lattice.si_111_1x1(), "named")
    assert named.lattice == Lattice.si_111_1x1()
    assert named.name == "named"


def test_dot_tags() -> None:
    """Dot tags determine layout traversal, terminals, bounds, and equality."""
    lyt = SiDBLayout()
    lyt.assign_sidb(LatticeSite(3, 1, 0), DotTag.OUTPUT)
    lyt.assign_sidb(LatticeSite(0, 0, 0), DotTag.INPUT)
    lyt.assign_sidb(LatticeSite(1, 0, 0))

    assert lyt.num_dots() == 3
    assert lyt.get_dot_tag(LatticeSite(1, 0, 0)) == DotTag.NORMAL
    assert lyt.sidbs() == [LatticeSite(0, 0, 0), LatticeSite(1, 0, 0), LatticeSite(3, 1, 0)]
    assert lyt.index_of(LatticeSite(1, 0, 0)) == 1
    assert lyt.index_of(LatticeSite(9, 9, 0)) is None
    assert lyt.pis() == [LatticeSite(0, 0, 0)]
    assert lyt.pos() == [LatticeSite(3, 1, 0)]
    assert lyt.num_pis() == 1
    assert lyt.is_po(LatticeSite(3, 1, 0))
    assert lyt.bounding_box() == (LatticeSite(0, 0, 0), LatticeSite(3, 1, 0))

    lyt.assign_sidb(LatticeSite(1, 0, 0), DotTag.EMPTY)
    assert lyt.num_dots() == 2
    assert lyt.is_empty_site(LatticeSite(1, 0, 0))

    copy = SiDBLayout(lyt.lattice)
    for site in lyt.sidbs():
        copy.assign_sidb(site, lyt.get_dot_tag(site))
    assert copy == lyt
    assert hash(copy) == hash(lyt)
    assert "◯" in repr(lyt)

    lyt.assign_sidb(LatticeSite(0, 0, 0))
    assert lyt.get_dot_tag(LatticeSite(0, 0, 0)) == DotTag.NORMAL
    assert lyt.num_dots() == 2
    assert lyt.num_pis() == 0


def test_defects() -> None:
    """Defects retain charge data and move between lattice sites."""
    lyt = SiDBLayout()
    vacancy = Defect(DefectType.SI_VACANCY, -1, 5.6, 5.0)
    lyt.assign_defect(LatticeSite(5, 2, 0), vacancy)
    lyt.assign_defect(LatticeSite(1, 0, 1), Defect(DefectType.SILOXANE, 0))

    assert lyt.num_defects() == 2
    assert lyt.num_charged_defects() == 1
    assert lyt.num_neutral_defects() == 1
    assert lyt.get_defect(LatticeSite(5, 2, 0)) == vacancy
    assert lyt.get_defect(LatticeSite(9, 9, 0)).type == DefectType.NONE
    assert len(lyt.affected_sidbs(LatticeSite(1, 0, 1))) == 3
    assert len(lyt.affected_sidbs(LatticeSite(5, 2, 0), (1, 1))) == 9
    assert lyt.defects()[0][0] == LatticeSite(1, 0, 1)

    lyt.move_defect(LatticeSite(5, 2, 0), LatticeSite(6, 2, 0))
    assert lyt.get_defect(LatticeSite(6, 2, 0)) == vacancy
    assert lyt.bounding_box() == (LatticeSite(1, 0, 1), LatticeSite(6, 2, 0))
