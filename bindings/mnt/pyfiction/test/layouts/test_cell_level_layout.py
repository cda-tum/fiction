# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction import inml_technology, qca_layout, qca_technology, sidb_technology


def test_qca_technology():
    qca = qca_technology

    assert str(qca.cell_type.EMPTY) == "cell_type.EMPTY"
    assert str(qca.cell_type.NORMAL) == "cell_type.NORMAL"
    assert str(qca.cell_type.INPUT) == "cell_type.INPUT"
    assert str(qca.cell_type.OUTPUT) == "cell_type.OUTPUT"
    assert str(qca.cell_type.CONST_0) == "cell_type.CONST_0"
    assert str(qca.cell_type.CONST_1) == "cell_type.CONST_1"


def test_inml_technology():
    inml = inml_technology

    assert str(inml.cell_type.EMPTY) == "cell_type.EMPTY"
    assert str(inml.cell_type.NORMAL) == "cell_type.NORMAL"
    assert str(inml.cell_type.INPUT) == "cell_type.INPUT"
    assert str(inml.cell_type.SLANTED_EDGE_UP_MAGNET) == "cell_type.SLANTED_EDGE_UP_MAGNET"
    assert str(inml.cell_type.SLANTED_EDGE_DOWN_MAGNET) == "cell_type.SLANTED_EDGE_DOWN_MAGNET"
    assert str(inml.cell_type.INVERTER_MAGNET) == "cell_type.INVERTER_MAGNET"
    assert str(inml.cell_type.CROSSWIRE_MAGNET) == "cell_type.CROSSWIRE_MAGNET"
    assert str(inml.cell_type.FANOUT_COUPLER_MAGNET) == "cell_type.FANOUT_COUPLER_MAGNET"


def test_sidb_technology():
    sidb = sidb_technology

    assert str(sidb.cell_type.EMPTY) == "cell_type.EMPTY"
    assert str(sidb.cell_type.NORMAL) == "cell_type.NORMAL"
    assert str(sidb.cell_type.INPUT) == "cell_type.INPUT"
    assert str(sidb.cell_type.OUTPUT) == "cell_type.OUTPUT"
    assert str(sidb.cell_type.LOGIC) == "cell_type.LOGIC"


def test_qca_cell_layout_inheritance():
    layout = qca_layout((9, 9, 1))

    for t in layout.coordinates():
        assert t <= (9, 9, 1)
        assert layout.is_within_bounds(t)

    for t in layout.ground_coordinates():
        assert t.z == 0
        assert t <= (9, 9, 0)
        assert layout.is_within_bounds(t)

    for t in layout.adjacent_coordinates((2, 2)):
        assert t in [(1, 2), (2, 1), (3, 2), (2, 3)]


def test_cell_type_assignment():
    layout = qca_layout((4, 4), "OPEN", "AND")

    assert layout.is_empty()

    layout.assign_cell_type((0, 2), qca_technology.cell_type.INPUT)
    layout.assign_cell_type((2, 4), qca_technology.cell_type.INPUT)
    layout.assign_cell_type((2, 0), qca_technology.cell_type.CONST_0)
    layout.assign_cell_type((2, 1), qca_technology.cell_type.NORMAL)
    layout.assign_cell_type((2, 2), qca_technology.cell_type.NORMAL)
    layout.assign_cell_type((2, 3), qca_technology.cell_type.NORMAL)
    layout.assign_cell_type((1, 2), qca_technology.cell_type.NORMAL)
    layout.assign_cell_type((3, 2), qca_technology.cell_type.NORMAL)
    layout.assign_cell_type((4, 2), qca_technology.cell_type.OUTPUT)

    assert layout.get_cells_by_type(qca_technology.cell_type.OUTPUT) == [(4, 2)]
    assert layout.num_cells_of_given_type(qca_technology.cell_type.INPUT) == 2

    assert not layout.is_empty()

    layout.assign_cell_name((0, 2), "a")
    layout.assign_cell_name((2, 4), "b")
    layout.assign_cell_name((4, 2), "f")

    assert layout.get_layout_name() == "AND"
    assert layout.get_cell_name((0, 2)) == "a"
    assert layout.get_cell_name((2, 4)) == "b"
    assert layout.get_cell_name((4, 2)) == "f"

    assert layout.num_cells() == 9
    assert layout.num_pis() == 2
    assert layout.num_pos() == 1

    assert layout.is_pi((0, 2))
    assert layout.is_pi((2, 4))
    assert layout.is_po((4, 2))

    assert layout.get_cell_type((2, 0)) == qca_technology.cell_type.CONST_0
    assert layout.get_cell_type((2, 4)) == qca_technology.cell_type.INPUT
    assert layout.get_cell_type((0, 2)) == qca_technology.cell_type.INPUT
    assert layout.get_cell_type((2, 1)) == qca_technology.cell_type.NORMAL
    assert layout.get_cell_type((2, 2)) == qca_technology.cell_type.NORMAL
    assert layout.get_cell_type((2, 3)) == qca_technology.cell_type.NORMAL
    assert layout.get_cell_type((1, 2)) == qca_technology.cell_type.NORMAL
    assert layout.get_cell_type((3, 2)) == qca_technology.cell_type.NORMAL
    assert layout.get_cell_type((4, 2)) == qca_technology.cell_type.OUTPUT

    assert layout.is_empty_cell((0, 0))
    assert layout.is_empty_cell((0, 1))
    assert layout.is_empty_cell((1, 0))
    assert layout.is_empty_cell((1, 1))
    assert layout.is_empty_cell((3, 0))
    assert layout.is_empty_cell((3, 1))
    assert layout.is_empty_cell((4, 0))
    assert layout.is_empty_cell((4, 1))
    assert layout.is_empty_cell((0, 3))
    assert layout.is_empty_cell((1, 3))
    assert layout.is_empty_cell((0, 4))
    assert layout.is_empty_cell((1, 0))
    assert layout.is_empty_cell((3, 3))
    assert layout.is_empty_cell((3, 4))
    assert layout.is_empty_cell((4, 3))
    assert layout.is_empty_cell((4, 4))

    assert not layout.is_empty_cell((2, 0))
    assert not layout.is_empty_cell((2, 4))
    assert not layout.is_empty_cell((0, 2))
    assert not layout.is_empty_cell((2, 1))
    assert not layout.is_empty_cell((2, 2))
    assert not layout.is_empty_cell((2, 3))
    assert not layout.is_empty_cell((1, 2))
    assert not layout.is_empty_cell((3, 2))
    assert not layout.is_empty_cell((4, 2))
