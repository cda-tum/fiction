# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction.sidb import DotTag, Lattice, LatticeSite, SiDBLayout
from mnt.pyfiction.sidb.analysis import BdlPairDetectionParams, detect_bdl_pairs


@pytest.mark.parametrize(
    "lat",
    [pytest.param(Lattice.si_100_2x1(), id="100"), pytest.param(Lattice.si_111_1x1(), id="111")],
)
def test_detect_bdl_pairs(lat):
    lyt = SiDBLayout(lat)

    lyt.assign_sidb(LatticeSite(0, 0, 0), DotTag.INPUT)
    lyt.assign_sidb(LatticeSite(1, 0, 0), DotTag.INPUT)

    lyt.assign_sidb(LatticeSite(2, 0, 0), DotTag.NORMAL)
    lyt.assign_sidb(LatticeSite(3, 0, 0), DotTag.NORMAL)
    lyt.assign_sidb(LatticeSite(4, 0, 0), DotTag.NORMAL)
    lyt.assign_sidb(LatticeSite(5, 0, 0), DotTag.NORMAL)

    lyt.assign_sidb(LatticeSite(6, 0, 0), DotTag.OUTPUT)
    lyt.assign_sidb(LatticeSite(7, 0, 0), DotTag.OUTPUT)

    params = BdlPairDetectionParams()

    input_bdl_pairs = detect_bdl_pairs(lyt, DotTag.INPUT, params)
    output_bdl_pairs = detect_bdl_pairs(lyt, DotTag.OUTPUT, params)
    normal_bdl_pairs = detect_bdl_pairs(lyt, DotTag.NORMAL, params)

    assert len(input_bdl_pairs) == 0
    assert len(output_bdl_pairs) == 0
    assert len(normal_bdl_pairs) == 2
