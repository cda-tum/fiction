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
from mnt.pyfiction.sidb.simulation.logic import bdl_input_iterator


@pytest.fixture
def bdl_wire() -> SiDBLayout:
    """A BDL wire of one input pair, two normal pairs, and one output pair.

    Returns:
        The wire as a 100-lattice SiDB layout.
    """
    layout = SiDBLayout()

    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.INPUT)
    layout.assign_sidb(LatticeSite(2, 0, 0), DotTag.INPUT)

    layout.assign_sidb(LatticeSite(6, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(8, 0, 0), DotTag.NORMAL)

    layout.assign_sidb(LatticeSite(12, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(14, 0, 0), DotTag.NORMAL)

    layout.assign_sidb(LatticeSite(18, 0, 0), DotTag.OUTPUT)
    layout.assign_sidb(LatticeSite(20, 0, 0), DotTag.OUTPUT)
    return layout


def test_empty_layout() -> None:
    """Check comparisons for an iterator over an empty layout."""
    layout = SiDBLayout()

    bii = bdl_input_iterator(layout)

    assert bii.num_input_pairs() == 0
    assert bii == 0
    assert bii != 1
    assert bii < 1
    assert bii <= 1
    assert bii >= 0


def test_iteration_empty_layout() -> None:
    """Check manual iterator operations on an empty layout."""
    layout = SiDBLayout()

    bii = bdl_input_iterator(layout)

    assert bii.num_input_pairs() == 0
    assert bii == 0
    assert bii.get_layout().num_dots() == 0

    bii += 1

    assert bii.num_input_pairs() == 0
    assert bii == 1
    assert bii.get_layout().num_dots() == 0

    bii -= 1

    assert bii.num_input_pairs() == 0
    assert bii == 0
    assert bii.get_layout().num_dots() == 0


def test_manual_bdl_wire_iteration(bdl_wire: SiDBLayout) -> None:
    """Check manual input-pattern iteration over a BDL wire.

    Args:
        bdl_wire: BDL wire layout.
    """
    layout = bdl_wire
    bii = bdl_input_iterator(layout)

    assert bii.get_layout().num_dots() == 7  # 2 inputs (1 already deleted for input pattern 0), 4 normal, 2 outputs
    assert bii.num_input_pairs() == 1
    assert bii == 0

    lyt0 = bii.get_layout()

    assert lyt0.get_dot_tag(LatticeSite(0, 0, 0)) == DotTag.INPUT
    assert lyt0.get_dot_tag(LatticeSite(2, 0, 0)) == DotTag.EMPTY

    bii += 1

    lyt1 = bii.get_layout()

    assert lyt1.get_dot_tag(LatticeSite(0, 0, 0)) == DotTag.EMPTY
    assert lyt1.get_dot_tag(LatticeSite(2, 0, 0)) == DotTag.INPUT

    bii += 1

    lyt2 = bii.get_layout()

    assert lyt2.get_dot_tag(LatticeSite(0, 0, 0)) == DotTag.INPUT
    assert lyt2.get_dot_tag(LatticeSite(2, 0, 0)) == DotTag.EMPTY

    bii -= 1

    lyt1 = bii.get_layout()

    assert lyt1.get_dot_tag(LatticeSite(0, 0, 0)) == DotTag.EMPTY
    assert lyt1.get_dot_tag(LatticeSite(2, 0, 0)) == DotTag.INPUT

    bii -= 1

    lyt0 = bii.get_layout()

    assert lyt0.get_dot_tag(LatticeSite(0, 0, 0)) == DotTag.INPUT
    assert lyt0.get_dot_tag(LatticeSite(2, 0, 0)) == DotTag.EMPTY


def test_automatic_bdl_wire_iteration(bdl_wire: SiDBLayout) -> None:
    """Check automatic input-pattern iteration over a BDL wire.

    Args:
        bdl_wire: BDL wire layout.
    """
    layout = bdl_wire
    bii = bdl_input_iterator(layout)

    assert iter(bii) is bii

    input_pattern_layouts = list(bii)
    assert len(input_pattern_layouts) == 2

    for index, lyt in enumerate(input_pattern_layouts):
        if index == 0:
            assert lyt.get_dot_tag(LatticeSite(0, 0, 0)) == DotTag.INPUT
            assert lyt.get_dot_tag(LatticeSite(2, 0, 0)) == DotTag.EMPTY
        elif index == 1:
            assert lyt.get_dot_tag(LatticeSite(0, 0, 0)) == DotTag.EMPTY
            assert lyt.get_dot_tag(LatticeSite(2, 0, 0)) == DotTag.INPUT


def test_automatic_siqad_and_gate_iteration() -> None:
    """Check automatic iteration over all SiQAD AND-gate input patterns."""
    layout = SiDBLayout(Lattice.si_100_2x1(), "AND gate")

    layout.assign_sidb(LatticeSite(0, 0, 1), DotTag.INPUT)
    layout.assign_sidb(LatticeSite(2, 1, 1), DotTag.INPUT)

    layout.assign_sidb(LatticeSite(20, 0, 1), DotTag.INPUT)
    layout.assign_sidb(LatticeSite(18, 1, 1), DotTag.INPUT)

    layout.assign_sidb(LatticeSite(4, 2, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(6, 3, 1), DotTag.NORMAL)

    layout.assign_sidb(LatticeSite(14, 3, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(16, 2, 1), DotTag.NORMAL)

    layout.assign_sidb(LatticeSite(10, 3, 0), DotTag.OUTPUT)
    layout.assign_sidb(LatticeSite(10, 3, 1), DotTag.OUTPUT)

    layout.assign_sidb(LatticeSite(10, 9, 1), DotTag.NORMAL)

    bii = bdl_input_iterator(layout)

    input_pattern_layouts = list(bii)
    assert len(input_pattern_layouts) == 4

    for index, lyt in enumerate(input_pattern_layouts):
        if index == 0:
            assert lyt.get_dot_tag(LatticeSite(0, 0, 1)) == DotTag.INPUT
            assert lyt.get_dot_tag(LatticeSite(2, 1, 1)) == DotTag.EMPTY

            assert lyt.get_dot_tag(LatticeSite(20, 0, 1)) == DotTag.INPUT
            assert lyt.get_dot_tag(LatticeSite(18, 1, 1)) == DotTag.EMPTY

        elif index == 1:
            assert lyt.get_dot_tag(LatticeSite(0, 0, 1)) == DotTag.INPUT
            assert lyt.get_dot_tag(LatticeSite(2, 1, 1)) == DotTag.EMPTY

            assert lyt.get_dot_tag(LatticeSite(20, 0, 1)) == DotTag.EMPTY
            assert lyt.get_dot_tag(LatticeSite(18, 1, 1)) == DotTag.INPUT

        elif index == 2:
            assert lyt.get_dot_tag(LatticeSite(0, 0, 1)) == DotTag.EMPTY
            assert lyt.get_dot_tag(LatticeSite(2, 1, 1)) == DotTag.INPUT

            assert lyt.get_dot_tag(LatticeSite(20, 0, 1)) == DotTag.INPUT
            assert lyt.get_dot_tag(LatticeSite(18, 1, 1)) == DotTag.EMPTY

        elif index == 3:
            assert lyt.get_dot_tag(LatticeSite(0, 0, 1)) == DotTag.EMPTY
            assert lyt.get_dot_tag(LatticeSite(2, 1, 1)) == DotTag.INPUT

            assert lyt.get_dot_tag(LatticeSite(20, 0, 1)) == DotTag.EMPTY
            assert lyt.get_dot_tag(LatticeSite(18, 1, 1)) == DotTag.INPUT
