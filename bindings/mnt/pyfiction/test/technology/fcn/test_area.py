# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction import area, inml_layout, lattice_site, qca_layout, sidb_layout, sidb_technology


def test_qca_area() -> None:
    """QCA layout dimensions determine physical area."""
    lyt = qca_layout((4, 4))
    assert area(lyt) == pytest.approx(9604.0, abs=1e-7)


def test_inml_area() -> None:
    """iNML layout dimensions determine physical area."""
    lyt = inml_layout((4, 4))
    assert area(lyt) == pytest.approx(174000.0, abs=1e-7)


def test_sidb_area() -> None:
    """SiDB sites determine the physical area of a lattice layout."""
    lyt = sidb_layout()
    assert area(lyt) == pytest.approx(0.0, abs=1e-7)

    # four columns and four single-SiDB rows
    lyt.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)
    lyt.assign_cell_type(lattice_site(4, 2, 0), sidb_technology.cell_type.NORMAL)
    assert area(lyt) == pytest.approx(2.359296, abs=1e-7)
