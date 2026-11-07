# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction.fcn import area
from mnt.pyfiction.inml import inml_layout
from mnt.pyfiction.mol_qca import mol_qca_layout
from mnt.pyfiction.qca import qca_layout
from mnt.pyfiction.sidb import lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.model import sidb_defect, sidb_defect_type


@pytest.mark.parametrize(
    ("layout_type", "singleton_area", "five_by_five_area"),
    [(qca_layout, 324.0, 9604.0), (inml_layout, 5000.0, 174000.0), (mol_qca_layout, 4.0, 100.0)],
)
def test_cartesian_area(
    layout_type: type[qca_layout | inml_layout | mol_qca_layout], singleton_area: float, five_by_five_area: float
) -> None:
    """Physical area uses axis sizes, including empty cell positions."""
    assert area(layout_type()) == 0.0
    assert area(layout_type((0, 3))) == 0.0
    assert area(layout_type((3, 0))) == 0.0
    assert area(layout_type((1, 1))) == pytest.approx(singleton_area, rel=0, abs=1e-7)
    assert area(layout_type((5, 5))) == pytest.approx(five_by_five_area, rel=0, abs=1e-7)
    assert area(layout_type((2, 3)), width=3, height=5, hspace=7, vspace=11) == 481.0


def test_sidb_area() -> None:
    """SiDB sites determine the physical area of a lattice layout."""
    lyt = sidb_layout()
    assert area(lyt) == pytest.approx(0.0, abs=1e-7)

    lyt.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(4, 2, 0), sidb_dot_tag.NORMAL)
    assert area(lyt) == pytest.approx(2.359296, rel=0, abs=1e-7)

    lyt.assign_defect(lattice_site(4, 4, 1), sidb_defect(sidb_defect_type.DB, -1))
    assert area(lyt) == pytest.approx(5.308416, rel=0, abs=1e-7)


def test_sidb_area_spans_full_column_range() -> None:
    """Column spans use widened arithmetic before applying physical spacing."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site(-2147483648, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(2147483647, 0), sidb_dot_tag.NORMAL)
    assert area(layout) == 0.0
    assert area(layout, height=1) == pytest.approx(4294967295 * 0.384, rel=1e-6)
