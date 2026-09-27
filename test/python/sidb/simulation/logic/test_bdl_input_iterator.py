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
from mnt.pyfiction.sidb.analysis import InputEncoding, InputPatternParams, input_patterns


@pytest.fixture
def wire_layout() -> SiDBLayout:
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
    """A zero-input layout has one pattern, and exhaustion is stable."""
    patterns = input_patterns(SiDBLayout())
    assert next(patterns).num_dots() == 0
    for _ in range(2):
        with pytest.raises(StopIteration):
            next(patterns)


def test_independent_patterns(wire_layout: SiDBLayout) -> None:
    """Advancing the iterator and editing one snapshot preserve the other layouts."""
    patterns = input_patterns(wire_layout)
    first = next(patterns)
    second = next(patterns)
    assert first.get_dot_tag(LatticeSite()) == DotTag.INPUT
    assert second.get_dot_tag(LatticeSite()) == DotTag.EMPTY
    first.assign_sidb(LatticeSite(6, 0), DotTag.EMPTY)
    assert second.get_dot_tag(LatticeSite(6, 0)) == DotTag.NORMAL
    assert wire_layout.num_dots() == 8


def test_invalid_input_wires(wire_layout: SiDBLayout) -> None:
    """An explicit empty wire list cannot encode an input pair."""
    with pytest.raises(ValueError, match="complete input wire"):
        input_patterns(wire_layout, input_wires=[])


def test_absence_encoding(wire_layout: SiDBLayout) -> None:
    """Absence encoding removes the input perturber for pattern zero."""
    params = InputPatternParams()
    params.input_bdl_config = InputEncoding.PERTURBER_ABSENCE_ENCODED
    zero, one = input_patterns(wire_layout, params=params)
    assert zero.num_pis() == 0
    assert one.num_pis() == 1
    assert wire_layout.num_pis() == 2


def test_input_pattern_bound() -> None:
    """Reject a pattern space that does not fit the native 64-bit counter."""
    layout = SiDBLayout()
    for row in range(64):
        layout.assign_sidb(LatticeSite(0, row * 10), DotTag.INPUT)
        layout.assign_sidb(LatticeSite(2, row * 10), DotTag.INPUT)
    with pytest.raises(ValueError, match="63 input BDL pairs"):
        input_patterns(layout, input_wires=[])


def test_automatic_wire_layout_iteration(wire_layout: SiDBLayout) -> None:
    """Check automatic input-pattern iteration over a BDL wire.

    Args:
        wire_layout: BDL wire layout.
    """
    layout = wire_layout
    bii = input_patterns(layout)

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

    bii = input_patterns(layout)

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
