# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction import (
    detect_bdl_pairs,
    detect_bdl_pairs_params,
    lattice,
    lattice_site,
    sidb_dot_tag,
    sidb_layout,
)


@pytest.mark.parametrize(
    "lat",
    [pytest.param(lattice.si_100_2x1(), id="100"), pytest.param(lattice.si_111_1x1(), id="111")],
)
def test_detect_bdl_pairs(lat):
    lyt = sidb_layout(lat)

    lyt.assign_dot_tag(lattice_site(0, 0, 0), sidb_dot_tag.INPUT)
    lyt.assign_dot_tag(lattice_site(1, 0, 0), sidb_dot_tag.INPUT)

    lyt.assign_dot_tag(lattice_site(2, 0, 0), sidb_dot_tag.NORMAL)
    lyt.assign_dot_tag(lattice_site(3, 0, 0), sidb_dot_tag.NORMAL)
    lyt.assign_dot_tag(lattice_site(4, 0, 0), sidb_dot_tag.NORMAL)
    lyt.assign_dot_tag(lattice_site(5, 0, 0), sidb_dot_tag.NORMAL)

    lyt.assign_dot_tag(lattice_site(6, 0, 0), sidb_dot_tag.OUTPUT)
    lyt.assign_dot_tag(lattice_site(7, 0, 0), sidb_dot_tag.OUTPUT)

    params = detect_bdl_pairs_params()

    input_bdl_pairs = detect_bdl_pairs(lyt, sidb_dot_tag.INPUT, params)
    output_bdl_pairs = detect_bdl_pairs(lyt, sidb_dot_tag.OUTPUT, params)
    normal_bdl_pairs = detect_bdl_pairs(lyt, sidb_dot_tag.NORMAL, params)

    assert len(input_bdl_pairs) == 0
    assert len(output_bdl_pairs) == 0
    assert len(normal_bdl_pairs) == 2
