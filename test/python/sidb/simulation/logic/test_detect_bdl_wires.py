# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction.sidb import DotTag, LatticeSite, SiDBLayout
from mnt.pyfiction.sidb.analysis import BdlWireDetectionParams, BdlWireSelection, detect_bdl_wires


def test_detect_bdl_wires_100_lattice():
    lyt = SiDBLayout()

    lyt.assign_sidb(LatticeSite(38, 0, 0), DotTag.OUTPUT)
    lyt.assign_sidb(LatticeSite(0, 0, 0), DotTag.OUTPUT)

    lyt.assign_sidb(LatticeSite(36, 1, 0), DotTag.OUTPUT)
    lyt.assign_sidb(LatticeSite(2, 1, 0), DotTag.OUTPUT)

    lyt.assign_sidb(LatticeSite(6, 2, 0), DotTag.NORMAL)
    lyt.assign_sidb(LatticeSite(32, 2, 0), DotTag.NORMAL)

    lyt.assign_sidb(LatticeSite(30, 3, 0), DotTag.NORMAL)
    lyt.assign_sidb(LatticeSite(8, 3, 0), DotTag.NORMAL)

    lyt.assign_sidb(LatticeSite(26, 4, 0), DotTag.NORMAL)
    lyt.assign_sidb(LatticeSite(12, 4, 0), DotTag.NORMAL)

    lyt.assign_sidb(LatticeSite(24, 5, 0), DotTag.NORMAL)
    lyt.assign_sidb(LatticeSite(14, 5, 0), DotTag.NORMAL)

    lyt.assign_sidb(LatticeSite(24, 15, 0), DotTag.NORMAL)
    lyt.assign_sidb(LatticeSite(26, 16, 0), DotTag.NORMAL)

    lyt.assign_sidb(LatticeSite(30, 17, 0), DotTag.INPUT)
    lyt.assign_sidb(LatticeSite(32, 18, 0), DotTag.INPUT)

    params = BdlWireDetectionParams()

    all_bdl_wires = detect_bdl_wires(lyt, params, BdlWireSelection.ALL)
    output_bdl_wires = detect_bdl_wires(lyt, params, BdlWireSelection.OUTPUT)
    input_bdl_wires = detect_bdl_wires(lyt, params, BdlWireSelection.INPUT)

    assert len(all_bdl_wires) == 3
    assert len(output_bdl_wires) == 2
    assert len(input_bdl_wires) == 1
