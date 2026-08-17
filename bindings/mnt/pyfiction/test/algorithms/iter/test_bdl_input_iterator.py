from mnt.pyfiction import bdl_input_iterator_100, sidb_100_lattice, sidb_technology


def test_empty_layout():
    layout = sidb_100_lattice((0, 0))

    bii = bdl_input_iterator_100(layout)

    assert bii.num_input_pairs() == 0
    assert bii == 0
    assert bii != 1
    assert bii < 1
    assert bii <= 1
    assert bii >= 0


def test_iteration_empty_layout():
    layout = sidb_100_lattice((0, 0))

    bii = bdl_input_iterator_100(layout)

    assert bii.num_input_pairs() == 0
    assert bii == 0
    assert bii.get_layout().num_cells() == 0

    bii = bii + 1

    assert bii.num_input_pairs() == 0
    assert bii == 1
    assert bii.get_layout().num_cells() == 0

    bii = bii - 1

    assert bii.num_input_pairs() == 0
    assert bii == 0
    assert bii.get_layout().num_cells() == 0


def test_manual_bdl_wire_iteration():
    layout = sidb_100_lattice((20, 0))

    layout.assign_cell_type((0, 0, 0), sidb_technology.cell_type.INPUT)
    layout.assign_cell_type((2, 0, 0), sidb_technology.cell_type.INPUT)

    layout.assign_cell_type((6, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((8, 0, 0), sidb_technology.cell_type.NORMAL)

    layout.assign_cell_type((12, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((14, 0, 0), sidb_technology.cell_type.NORMAL)

    layout.assign_cell_type((18, 0, 0), sidb_technology.cell_type.OUTPUT)
    layout.assign_cell_type((20, 0, 0), sidb_technology.cell_type.OUTPUT)

    bii = bdl_input_iterator_100(layout)

    assert bii.get_layout().num_cells() == 7  # 2 inputs (1 already deleted for input pattern 0), 4 normal, 2 outputs
    assert bii.num_input_pairs() == 1
    assert bii == 0

    lyt0 = bii.get_layout()

    assert lyt0.get_cell_type((0, 0, 0)) == sidb_technology.cell_type.INPUT
    assert lyt0.get_cell_type((2, 0, 0)) == sidb_technology.cell_type.EMPTY

    bii = bii + 1

    lyt1 = bii.get_layout()

    assert lyt1.get_cell_type((0, 0, 0)) == sidb_technology.cell_type.EMPTY
    assert lyt1.get_cell_type((2, 0, 0)) == sidb_technology.cell_type.INPUT

    bii = bii + 1

    lyt2 = bii.get_layout()

    assert lyt2.get_cell_type((0, 0, 0)) == sidb_technology.cell_type.INPUT
    assert lyt2.get_cell_type((2, 0, 0)) == sidb_technology.cell_type.EMPTY

    bii = bii - 1

    lyt1 = bii.get_layout()

    assert lyt1.get_cell_type((0, 0, 0)) == sidb_technology.cell_type.EMPTY
    assert lyt1.get_cell_type((2, 0, 0)) == sidb_technology.cell_type.INPUT

    bii = bii - 1

    lyt0 = bii.get_layout()

    assert lyt0.get_cell_type((0, 0, 0)) == sidb_technology.cell_type.INPUT
    assert lyt0.get_cell_type((2, 0, 0)) == sidb_technology.cell_type.EMPTY


def test_automatic_bdl_wire_iteration():
    layout = sidb_100_lattice((20, 0))

    layout.assign_cell_type((0, 0, 0), sidb_technology.cell_type.INPUT)
    layout.assign_cell_type((2, 0, 0), sidb_technology.cell_type.INPUT)

    layout.assign_cell_type((6, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((8, 0, 0), sidb_technology.cell_type.NORMAL)

    layout.assign_cell_type((12, 0, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((14, 0, 0), sidb_technology.cell_type.NORMAL)

    layout.assign_cell_type((18, 0, 0), sidb_technology.cell_type.OUTPUT)
    layout.assign_cell_type((20, 0, 0), sidb_technology.cell_type.OUTPUT)

    bii = bdl_input_iterator_100(layout)

    for index, bii_iterator in enumerate(bii):
        lyt = bii_iterator.get_layout()
        if index == 0:
            assert lyt.get_cell_type((0, 0, 0)) == sidb_technology.cell_type.INPUT
            assert lyt.get_cell_type((2, 0, 0)) == sidb_technology.cell_type.EMPTY
        elif index == 1:
            assert lyt.get_cell_type((0, 0, 0)) == sidb_technology.cell_type.EMPTY
            assert lyt.get_cell_type((2, 0, 0)) == sidb_technology.cell_type.INPUT
        elif index == 2:
            assert lyt.get_cell_type((0, 0, 0)) == sidb_technology.cell_type.INPUT
            assert lyt.get_cell_type((2, 0, 0)) == sidb_technology.cell_type.EMPTY
        elif index == 3:
            break


def test_automatic_siqad_and_gate_iteration():
    layout = sidb_100_lattice((20, 10), "AND gate")

    layout.assign_cell_type((0, 0, 1), sidb_technology.cell_type.INPUT)
    layout.assign_cell_type((2, 1, 1), sidb_technology.cell_type.INPUT)

    layout.assign_cell_type((20, 0, 1), sidb_technology.cell_type.INPUT)
    layout.assign_cell_type((18, 1, 1), sidb_technology.cell_type.INPUT)

    layout.assign_cell_type((4, 2, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((6, 3, 1), sidb_technology.cell_type.NORMAL)

    layout.assign_cell_type((14, 3, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((16, 2, 1), sidb_technology.cell_type.NORMAL)

    layout.assign_cell_type((10, 6, 0), sidb_technology.cell_type.OUTPUT)
    layout.assign_cell_type((10, 7, 0), sidb_technology.cell_type.OUTPUT)

    layout.assign_cell_type((10, 9, 1), sidb_technology.cell_type.NORMAL)

    bii = bdl_input_iterator_100(layout)

    for index, bii_iterator in enumerate(bii):
        lyt = bii_iterator.get_layout()
        if index == 0:
            assert lyt.get_cell_type((0, 0, 1)) == sidb_technology.cell_type.INPUT
            assert lyt.get_cell_type((2, 1, 1)) == sidb_technology.cell_type.EMPTY

            assert lyt.get_cell_type((20, 0, 1)) == sidb_technology.cell_type.INPUT
            assert lyt.get_cell_type((18, 1, 1)) == sidb_technology.cell_type.EMPTY

        elif index == 1:
            assert lyt.get_cell_type((0, 0, 1)) == sidb_technology.cell_type.INPUT
            assert lyt.get_cell_type((2, 1, 1)) == sidb_technology.cell_type.EMPTY

            assert lyt.get_cell_type((20, 0, 1)) == sidb_technology.cell_type.EMPTY
            assert lyt.get_cell_type((18, 1, 1)) == sidb_technology.cell_type.INPUT

        elif index == 2:
            assert lyt.get_cell_type((0, 0, 1)) == sidb_technology.cell_type.EMPTY
            assert lyt.get_cell_type((2, 1, 1)) == sidb_technology.cell_type.INPUT

            assert lyt.get_cell_type((20, 0, 1)) == sidb_technology.cell_type.INPUT
            assert lyt.get_cell_type((18, 1, 1)) == sidb_technology.cell_type.EMPTY

        elif index == 3:
            assert lyt.get_cell_type((0, 0, 1)) == sidb_technology.cell_type.EMPTY
            assert lyt.get_cell_type((2, 1, 1)) == sidb_technology.cell_type.INPUT

            assert lyt.get_cell_type((20, 0, 1)) == sidb_technology.cell_type.EMPTY
            assert lyt.get_cell_type((18, 1, 1)) == sidb_technology.cell_type.INPUT

        else:
            break
