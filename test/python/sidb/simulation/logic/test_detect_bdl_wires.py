# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from collections import Counter

from mnt.pyfiction.sidb import lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.simulation.logic import bdl_pair, bdl_wire, bdl_wire_selection, detect_bdl_wires


def pair_identities(
    pairs: list[bdl_pair] | tuple[bdl_pair, ...],
) -> tuple[tuple[sidb_dot_tag, lattice_site, lattice_site], ...]:
    return tuple((pair.type, pair.upper, pair.lower) for pair in sorted(pairs))


def wire_pairs(wires: list[bdl_wire]) -> Counter[tuple[tuple[sidb_dot_tag, lattice_site, lattice_site], ...]]:
    return Counter(pair_identities(wire.pairs) for wire in wires)


def test_detect_bdl_wires_100_lattice() -> None:
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

    left_output = (
        bdl_pair(sidb_dot_tag.OUTPUT, lattice_site(0, 0, 0), lattice_site(2, 1, 0)),
        bdl_pair(sidb_dot_tag.NORMAL, lattice_site(6, 2, 0), lattice_site(8, 3, 0)),
        bdl_pair(sidb_dot_tag.NORMAL, lattice_site(12, 4, 0), lattice_site(14, 5, 0)),
    )
    right_output = (
        bdl_pair(sidb_dot_tag.OUTPUT, lattice_site(38, 0, 0), lattice_site(36, 1, 0)),
        bdl_pair(sidb_dot_tag.NORMAL, lattice_site(32, 2, 0), lattice_site(30, 3, 0)),
        bdl_pair(sidb_dot_tag.NORMAL, lattice_site(26, 4, 0), lattice_site(24, 5, 0)),
    )
    input_wire = (
        bdl_pair(sidb_dot_tag.NORMAL, lattice_site(24, 15, 0), lattice_site(26, 16, 0)),
        bdl_pair(sidb_dot_tag.INPUT, lattice_site(30, 17, 0), lattice_site(32, 18, 0)),
    )
    expected_output = Counter(pair_identities(pairs) for pairs in (left_output, right_output))
    expected_input = Counter([pair_identities(input_wire)])

    assert wire_pairs(output_bdl_wires) == expected_output
    assert wire_pairs(input_bdl_wires) == expected_input
    assert wire_pairs(all_bdl_wires) == expected_output + expected_input

    outputs_by_last = {wire.last_bdl_pair: wire for wire in output_bdl_wires}
    assert outputs_by_last[left_output[0]].first_bdl_pair == left_output[-1]
    assert outputs_by_last[right_output[0]].first_bdl_pair == right_output[-1]
    assert input_bdl_wires[0].first_bdl_pair == input_wire[-1]
    assert input_bdl_wires[0].last_bdl_pair == input_wire[0]


def test_wire_distance_keyword() -> None:
    """The wire distance joins nearby BDL pairs or leaves separate wires."""
    layout = sidb_layout()
    for x, y in [(0, 0), (2, 1), (6, 2), (8, 3)]:
        layout.assign_sidb(lattice_site(x, y, 0), sidb_dot_tag.INPUT)
    first = bdl_pair(sidb_dot_tag.INPUT, lattice_site(0, 0, 0), lattice_site(2, 1, 0))
    second = bdl_pair(sidb_dot_tag.INPUT, lattice_site(6, 2, 0), lattice_site(8, 3, 0))
    assert wire_pairs(detect_bdl_wires(layout, threshold_bdl_interdistance=0.9)) == Counter([
        pair_identities((first,)),
        pair_identities((second,)),
    ])
    assert wire_pairs(detect_bdl_wires(layout, threshold_bdl_interdistance=2.0)) == Counter([
        pair_identities((first, second))
    ])
