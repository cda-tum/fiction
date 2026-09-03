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
    bdl_input_iterator,
    lattice,
    lattice_site,
    sidb_layout,
    sidb_technology,
)


@pytest.fixture
def bdl_wire() -> sidb_layout:
    """A BDL wire of one input pair, two normal pairs, and one output pair.

    Returns:
        The wire as a 100-lattice SiDB layout.
    """
    layout = sidb_layout()

    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.INPUT)
    layout.assign_cell_type(lattice_site(2, 0, 0), sidb_technology.cell_type.INPUT)

    layout.assign_cell_type(lattice_site(6, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(8, 0, 0), sidb_technology.cell_type.NORMAL)

    layout.assign_cell_type(lattice_site(12, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(14, 0, 0), sidb_technology.cell_type.NORMAL)

    layout.assign_cell_type(lattice_site(18, 0, 0), sidb_technology.cell_type.OUTPUT)
    layout.assign_cell_type(lattice_site(20, 0, 0), sidb_technology.cell_type.OUTPUT)
    return layout


def test_empty_layout():
    layout = sidb_layout()

    bii = bdl_input_iterator(layout)

    assert bii.num_input_pairs() == 0
    assert bii == 0
    assert bii != 1
    assert bii < 1
    assert bii <= 1
    assert bii >= 0


def test_iteration_empty_layout():
    layout = sidb_layout()

    bii = bdl_input_iterator(layout)

    assert bii.num_input_pairs() == 0
    assert bii == 0
    assert bii.get_layout().num_cells() == 0

    bii += 1

    assert bii.num_input_pairs() == 0
    assert bii == 1
    assert bii.get_layout().num_cells() == 0

    bii -= 1

    assert bii.num_input_pairs() == 0
    assert bii == 0
    assert bii.get_layout().num_cells() == 0


def test_manual_bdl_wire_iteration(bdl_wire):
    layout = bdl_wire
    bii = bdl_input_iterator(layout)

    assert bii.get_layout().num_cells() == 7  # 2 inputs (1 already deleted for input pattern 0), 4 normal, 2 outputs
    assert bii.num_input_pairs() == 1
    assert bii == 0

    lyt0 = bii.get_layout()

    assert lyt0.get_cell_type(lattice_site(0, 0, 0)) == sidb_technology.cell_type.INPUT
    assert lyt0.get_cell_type(lattice_site(2, 0, 0)) == sidb_technology.cell_type.EMPTY

    bii += 1

    lyt1 = bii.get_layout()

    assert lyt1.get_cell_type(lattice_site(0, 0, 0)) == sidb_technology.cell_type.EMPTY
    assert lyt1.get_cell_type(lattice_site(2, 0, 0)) == sidb_technology.cell_type.INPUT

    bii += 1

    lyt2 = bii.get_layout()

    assert lyt2.get_cell_type(lattice_site(0, 0, 0)) == sidb_technology.cell_type.INPUT
    assert lyt2.get_cell_type(lattice_site(2, 0, 0)) == sidb_technology.cell_type.EMPTY

    bii -= 1

    lyt1 = bii.get_layout()

    assert lyt1.get_cell_type(lattice_site(0, 0, 0)) == sidb_technology.cell_type.EMPTY
    assert lyt1.get_cell_type(lattice_site(2, 0, 0)) == sidb_technology.cell_type.INPUT

    bii -= 1

    lyt0 = bii.get_layout()

    assert lyt0.get_cell_type(lattice_site(0, 0, 0)) == sidb_technology.cell_type.INPUT
    assert lyt0.get_cell_type(lattice_site(2, 0, 0)) == sidb_technology.cell_type.EMPTY


def test_automatic_bdl_wire_iteration(bdl_wire):
    layout = bdl_wire
    bii = bdl_input_iterator(layout)

    for index, bii_iterator in enumerate(bii):
        lyt = bii_iterator.get_layout()
        if index == 0:
            assert lyt.get_cell_type(lattice_site(0, 0, 0)) == sidb_technology.cell_type.INPUT
            assert lyt.get_cell_type(lattice_site(2, 0, 0)) == sidb_technology.cell_type.EMPTY
        elif index == 1:
            assert lyt.get_cell_type(lattice_site(0, 0, 0)) == sidb_technology.cell_type.EMPTY
            assert lyt.get_cell_type(lattice_site(2, 0, 0)) == sidb_technology.cell_type.INPUT
        elif index == 2:
            assert lyt.get_cell_type(lattice_site(0, 0, 0)) == sidb_technology.cell_type.INPUT
            assert lyt.get_cell_type(lattice_site(2, 0, 0)) == sidb_technology.cell_type.EMPTY
        elif index == 3:
            break


def test_automatic_siqad_and_gate_iteration():
    layout = sidb_layout(lattice.si_100_2x1(), "AND gate")

    layout.assign_cell_type(lattice_site(0, 0, 1), sidb_technology.cell_type.INPUT)
    layout.assign_cell_type(lattice_site(2, 1, 1), sidb_technology.cell_type.INPUT)

    layout.assign_cell_type(lattice_site(20, 0, 1), sidb_technology.cell_type.INPUT)
    layout.assign_cell_type(lattice_site(18, 1, 1), sidb_technology.cell_type.INPUT)

    layout.assign_cell_type(lattice_site(4, 2, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(6, 3, 1), sidb_technology.cell_type.NORMAL)

    layout.assign_cell_type(lattice_site(14, 3, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(16, 2, 1), sidb_technology.cell_type.NORMAL)

    layout.assign_cell_type(lattice_site(10, 3, 0), sidb_technology.cell_type.OUTPUT)
    layout.assign_cell_type(lattice_site(10, 3, 1), sidb_technology.cell_type.OUTPUT)

    layout.assign_cell_type(lattice_site(10, 9, 1), sidb_technology.cell_type.NORMAL)

    bii = bdl_input_iterator(layout)

    for index, bii_iterator in enumerate(bii):
        lyt = bii_iterator.get_layout()
        if index == 0:
            assert lyt.get_cell_type(lattice_site(0, 0, 1)) == sidb_technology.cell_type.INPUT
            assert lyt.get_cell_type(lattice_site(2, 1, 1)) == sidb_technology.cell_type.EMPTY

            assert lyt.get_cell_type(lattice_site(20, 0, 1)) == sidb_technology.cell_type.INPUT
            assert lyt.get_cell_type(lattice_site(18, 1, 1)) == sidb_technology.cell_type.EMPTY

        elif index == 1:
            assert lyt.get_cell_type(lattice_site(0, 0, 1)) == sidb_technology.cell_type.INPUT
            assert lyt.get_cell_type(lattice_site(2, 1, 1)) == sidb_technology.cell_type.EMPTY

            assert lyt.get_cell_type(lattice_site(20, 0, 1)) == sidb_technology.cell_type.EMPTY
            assert lyt.get_cell_type(lattice_site(18, 1, 1)) == sidb_technology.cell_type.INPUT

        elif index == 2:
            assert lyt.get_cell_type(lattice_site(0, 0, 1)) == sidb_technology.cell_type.EMPTY
            assert lyt.get_cell_type(lattice_site(2, 1, 1)) == sidb_technology.cell_type.INPUT

            assert lyt.get_cell_type(lattice_site(20, 0, 1)) == sidb_technology.cell_type.INPUT
            assert lyt.get_cell_type(lattice_site(18, 1, 1)) == sidb_technology.cell_type.EMPTY

        elif index == 3:
            assert lyt.get_cell_type(lattice_site(0, 0, 1)) == sidb_technology.cell_type.EMPTY
            assert lyt.get_cell_type(lattice_site(2, 1, 1)) == sidb_technology.cell_type.INPUT

            assert lyt.get_cell_type(lattice_site(20, 0, 1)) == sidb_technology.cell_type.EMPTY
            assert lyt.get_cell_type(lattice_site(18, 1, 1)) == sidb_technology.cell_type.INPUT

        else:
            break
