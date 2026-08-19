from __future__ import annotations

import pytest

from mnt.pyfiction import sidb_100_lattice, sidb_111_lattice, sidb_technology


def test_qca_cell_layout_inheritance():
    layout = sidb_100_lattice((9, 9))

    for t in layout.coordinates():
        assert t <= (9, 9, 1)
        assert layout.is_within_bounds(t)

    for t in layout.ground_coordinates():
        assert t.z == 0
        assert t <= (9, 9, 0)
        assert layout.is_within_bounds(t)

    for t in layout.adjacent_coordinates((2, 2)):
        assert t in [(1, 2), (2, 1), (3, 2), (2, 3)]


@pytest.mark.parametrize(
    "sidb_lattice",
    [
        pytest.param(sidb_100_lattice, id="100"),
        pytest.param(sidb_111_lattice, id="111"),
    ],
)
def test_cell_type_assignment(sidb_lattice):
    layout = sidb_lattice((4, 4), "AND")

    assert layout.is_empty()

    layout.assign_cell_type((0, 2), sidb_technology.cell_type.INPUT)
    layout.assign_cell_type((2, 4), sidb_technology.cell_type.INPUT)
    layout.assign_cell_type((2, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((2, 2), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((2, 3), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((1, 2), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((3, 2), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((4, 2), sidb_technology.cell_type.OUTPUT)

    assert not layout.is_empty()

    layout.assign_cell_name((0, 2), "a")
    layout.assign_cell_name((2, 4), "b")
    layout.assign_cell_name((4, 2), "f")

    assert layout.get_layout_name() == "AND"
    assert layout.get_cell_name((0, 2)) == "a"
    assert layout.get_cell_name((2, 4)) == "b"
    assert layout.get_cell_name((4, 2)) == "f"

    assert layout.num_cells() == 8
    assert layout.num_pis() == 2
    assert layout.num_pos() == 1

    assert layout.is_pi((0, 2))
    assert layout.is_pi((2, 4))
    assert layout.is_po((4, 2))

    assert layout.get_cell_type((2, 4)) == sidb_technology.cell_type.INPUT
    assert layout.get_cell_type((0, 2)) == sidb_technology.cell_type.INPUT
    assert layout.get_cell_type((2, 1)) == sidb_technology.cell_type.NORMAL
    assert layout.get_cell_type((2, 2)) == sidb_technology.cell_type.NORMAL
    assert layout.get_cell_type((2, 3)) == sidb_technology.cell_type.NORMAL
    assert layout.get_cell_type((1, 2)) == sidb_technology.cell_type.NORMAL
    assert layout.get_cell_type((3, 2)) == sidb_technology.cell_type.NORMAL
    assert layout.get_cell_type((4, 2)) == sidb_technology.cell_type.OUTPUT

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

    assert not layout.is_empty_cell((2, 4))
    assert not layout.is_empty_cell((0, 2))
    assert not layout.is_empty_cell((2, 1))
    assert not layout.is_empty_cell((2, 2))
    assert not layout.is_empty_cell((2, 3))
    assert not layout.is_empty_cell((1, 2))
    assert not layout.is_empty_cell((3, 2))
    assert not layout.is_empty_cell((4, 2))
