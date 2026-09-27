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
from mnt.pyfiction.inml import INMLLayout
from mnt.pyfiction.qca import QCALayout
from mnt.pyfiction.sidb import DotTag, LatticeSite, SiDBLayout


def test_qca_area() -> None:
    """QCA layout dimensions determine physical area."""
    lyt = QCALayout((4, 4))
    assert area(lyt) == pytest.approx(9604.0, abs=1e-7)


def test_inml_area() -> None:
    """iNML layout dimensions determine physical area."""
    lyt = INMLLayout((4, 4))
    assert area(lyt) == pytest.approx(174000.0, abs=1e-7)


def test_sidb_area() -> None:
    """SiDB sites determine the physical area of a lattice layout."""
    lyt = SiDBLayout()
    assert area(lyt) == pytest.approx(0.0, abs=1e-7)

    # four columns and four single-SiDB rows
    lyt.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)
    lyt.assign_sidb(LatticeSite(4, 2, 0), DotTag.NORMAL)
    assert area(lyt) == pytest.approx(2.359296, abs=1e-7)
