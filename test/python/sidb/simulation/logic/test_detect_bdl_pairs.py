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
from mnt.pyfiction.sidb.simulation.logic import bdl_pair, detect_bdl_pairs


@pytest.mark.parametrize(
    "lat",
    [pytest.param(lattice.si_100_2x1(), id="100"), pytest.param(lattice.si_111_1x1(), id="111")],
)
def test_detect_bdl_pairs(lat):
    lyt = sidb_layout(lat)

    lyt.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.INPUT)
    lyt.assign_sidb(lattice_site(1, 0, 0), sidb_dot_tag.INPUT)

    lyt.assign_sidb(lattice_site(2, 0, 0), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(3, 0, 0), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(4, 0, 0), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(5, 0, 0), sidb_dot_tag.NORMAL)

    lyt.assign_sidb(lattice_site(6, 0, 0), sidb_dot_tag.OUTPUT)
    lyt.assign_sidb(lattice_site(7, 0, 0), sidb_dot_tag.OUTPUT)

    input_bdl_pairs = detect_bdl_pairs(lyt, sidb_dot_tag.INPUT)
    output_bdl_pairs = detect_bdl_pairs(lyt, sidb_dot_tag.OUTPUT)
    normal_bdl_pairs = detect_bdl_pairs(lyt, sidb_dot_tag.NORMAL)

    assert len(input_bdl_pairs) == 0
    assert len(output_bdl_pairs) == 0
    assert normal_bdl_pairs == [
        bdl_pair(sidb_dot_tag.NORMAL, lattice_site(2, 0), lattice_site(4, 0)),
        bdl_pair(sidb_dot_tag.NORMAL, lattice_site(3, 0), lattice_site(5, 0)),
    ]


def test_pair_distance_keywords() -> None:
    """Both distance bounds select the detected BDL pairs."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.INPUT)
    layout.assign_sidb(lattice_site(2, 1, 0), sidb_dot_tag.INPUT)
    assert len(detect_bdl_pairs(layout, sidb_dot_tag.INPUT, minimum_distance=0.7, maximum_distance=1.3)) == 1
    assert not detect_bdl_pairs(layout, sidb_dot_tag.INPUT, minimum_distance=1.3, maximum_distance=1.5)
    assert not detect_bdl_pairs(layout, sidb_dot_tag.INPUT, minimum_distance=0.3, maximum_distance=1.0)


def test_pair_distance_bounds_are_inclusive() -> None:
    """A pair exactly on either physical-distance bound remains eligible."""
    layout = sidb_layout()
    first, second = lattice_site(), lattice_site(2, 0)
    layout.assign_sidb(first, sidb_dot_tag.INPUT)
    layout.assign_sidb(second, sidb_dot_tag.INPUT)
    distance = layout.get_lattice().nm_distance(first, second)
    assert detect_bdl_pairs(layout, sidb_dot_tag.INPUT, minimum_distance=distance, maximum_distance=distance) == [
        bdl_pair(sidb_dot_tag.INPUT, first, second)
    ]
    assert detect_bdl_pairs(sidb_layout(), sidb_dot_tag.NORMAL) == []
