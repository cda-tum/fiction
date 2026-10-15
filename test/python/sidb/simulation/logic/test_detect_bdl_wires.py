# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction.sidb import lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.simulation.logic import bdl_wire_selection, detect_bdl_wires


def test_detect_bdl_wires_100_lattice():
    lyt = sidb_layout()

    lyt.assign_sidb(lattice_site(38, 0, 0), sidb_dot_tag.OUTPUT)
    lyt.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.OUTPUT)

    lyt.assign_sidb(lattice_site(36, 1, 0), sidb_dot_tag.OUTPUT)
    lyt.assign_sidb(lattice_site(2, 1, 0), sidb_dot_tag.OUTPUT)

    lyt.assign_sidb(lattice_site(6, 2, 0), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(32, 2, 0), sidb_dot_tag.NORMAL)

    lyt.assign_sidb(lattice_site(30, 3, 0), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(8, 3, 0), sidb_dot_tag.NORMAL)

    lyt.assign_sidb(lattice_site(26, 4, 0), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(12, 4, 0), sidb_dot_tag.NORMAL)

    lyt.assign_sidb(lattice_site(24, 5, 0), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(14, 5, 0), sidb_dot_tag.NORMAL)

    lyt.assign_sidb(lattice_site(24, 15, 0), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(26, 16, 0), sidb_dot_tag.NORMAL)

    lyt.assign_sidb(lattice_site(30, 17, 0), sidb_dot_tag.INPUT)
    lyt.assign_sidb(lattice_site(32, 18, 0), sidb_dot_tag.INPUT)

    all_bdl_wires = detect_bdl_wires(lyt, wire_selection=bdl_wire_selection.ALL)
    output_bdl_wires = detect_bdl_wires(lyt, wire_selection=bdl_wire_selection.OUTPUT)
    input_bdl_wires = detect_bdl_wires(lyt, wire_selection=bdl_wire_selection.INPUT)

    assert len(all_bdl_wires) == 3
    assert len(output_bdl_wires) == 2
    assert len(input_bdl_wires) == 1


def test_wire_distance_keyword() -> None:
    """The wire distance joins nearby BDL pairs or leaves separate wires."""
    layout = sidb_layout()
    for x, y in [(0, 0), (2, 1), (6, 2), (8, 3)]:
        layout.assign_sidb(lattice_site(x, y, 0), sidb_dot_tag.INPUT)
    assert len(detect_bdl_wires(layout, threshold_bdl_interdistance=0.9)) == 2
    assert len(detect_bdl_wires(layout, threshold_bdl_interdistance=2.0)) == 1
