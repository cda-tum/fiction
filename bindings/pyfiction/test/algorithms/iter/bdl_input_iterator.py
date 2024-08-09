from mnt.pyfiction import *
import unittest


class TestBDLInputIterator(unittest.TestCase):
    def test_empty_layout(self):
        layout = sidb_100_lattice((0, 0))

        bii = bdl_input_iterator_100(layout)

        self.assertEqual(bii.num_input_pairs(), 0)
        self.assertEqual(bii, 0)
        self.assertNotEqual(bii, 1)
        self.assertLess(bii, 1)
        self.assertLessEqual(bii, 1)
        self.assertGreaterEqual(bii, 0)

    def test_iteration_empty_layout(self):
        layout = sidb_100_lattice((0, 0))

        bii = bdl_input_iterator_100(layout)

        self.assertEqual(bii.num_input_pairs(), 0)
        self.assertEqual(bii, 0)
        self.assertEqual(bii.get_layout().num_cells(), 0)

        bii = bii + 1

        self.assertEqual(bii.num_input_pairs(), 0)
        self.assertEqual(bii, 1)
        self.assertEqual(bii.get_layout().num_cells(), 0)

        bii = bii - 1

        self.assertEqual(bii.num_input_pairs(), 0)
        self.assertEqual(bii, 0)
        self.assertEqual(bii.get_layout().num_cells(), 0)

    def test_manual_bdl_wire_iteration(self):
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

        self.assertEqual(bii.get_layout().num_cells(),
                         7)  # 2 inputs (1 already deleted for input pattern 0), 4 normal, 2 outputs
        self.assertEqual(bii.num_input_pairs(), 1)
        self.assertEqual(bii, 0)

        lyt0 = bii.get_layout()

        self.assertEqual(lyt0.get_cell_type((0, 0, 0)), sidb_technology.cell_type.INPUT)
        self.assertEqual(lyt0.get_cell_type((2, 0, 0)), sidb_technology.cell_type.EMPTY)

        bii = bii + 1

        lyt1 = bii.get_layout()

        self.assertEqual(lyt1.get_cell_type((0, 0, 0)), sidb_technology.cell_type.EMPTY)
        self.assertEqual(lyt1.get_cell_type((2, 0, 0)), sidb_technology.cell_type.INPUT)

        bii = bii + 1

        lyt2 = bii.get_layout()

        self.assertEqual(lyt2.get_cell_type((0, 0, 0)), sidb_technology.cell_type.INPUT)
        self.assertEqual(lyt2.get_cell_type((2, 0, 0)), sidb_technology.cell_type.EMPTY)

        bii = bii - 1

        lyt1 = bii.get_layout()

        self.assertEqual(lyt1.get_cell_type((0, 0, 0)), sidb_technology.cell_type.EMPTY)
        self.assertEqual(lyt1.get_cell_type((2, 0, 0)), sidb_technology.cell_type.INPUT)

        bii = bii - 1

        lyt0 = bii.get_layout()

        self.assertEqual(lyt0.get_cell_type((0, 0, 0)), sidb_technology.cell_type.INPUT)
        self.assertEqual(lyt0.get_cell_type((2, 0, 0)), sidb_technology.cell_type.EMPTY)

    def test_automatic_bdl_wire_iteration(self):
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

        for (index, bii_iterator) in enumerate(bii):
            lyt = bii_iterator.get_layout()
            if index == 0:
                self.assertEqual(lyt.get_cell_type((0, 0, 0)), sidb_technology.cell_type.INPUT)
                self.assertEqual(lyt.get_cell_type((2, 0, 0)), sidb_technology.cell_type.EMPTY)
            elif index == 1:
                self.assertEqual(lyt.get_cell_type((0, 0, 0)), sidb_technology.cell_type.EMPTY)
                self.assertEqual(lyt.get_cell_type((2, 0, 0)), sidb_technology.cell_type.INPUT)
            elif index == 2:
                self.assertEqual(lyt.get_cell_type((0, 0, 0)), sidb_technology.cell_type.INPUT)
                self.assertEqual(lyt.get_cell_type((2, 0, 0)), sidb_technology.cell_type.EMPTY)
            elif index == 3:
                break

    def test_automatic_siqad_and_gate_iteration(self):
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

        for (index, bii_iterator) in enumerate(bii):
            lyt = bii_iterator.get_layout()
            if index == 0:
                self.assertEqual(lyt.get_cell_type((0, 0, 1)), sidb_technology.cell_type.INPUT)
                self.assertEqual(lyt.get_cell_type((2, 1, 1)), sidb_technology.cell_type.EMPTY)

                self.assertEqual(lyt.get_cell_type((20, 0, 1)), sidb_technology.cell_type.INPUT)
                self.assertEqual(lyt.get_cell_type((18, 1, 1)), sidb_technology.cell_type.EMPTY)

            elif index == 1:
                self.assertEqual(lyt.get_cell_type((0, 0, 1)), sidb_technology.cell_type.INPUT)
                self.assertEqual(lyt.get_cell_type((2, 1, 1)), sidb_technology.cell_type.EMPTY)

                self.assertEqual(lyt.get_cell_type((20, 0, 1)), sidb_technology.cell_type.EMPTY)
                self.assertEqual(lyt.get_cell_type((18, 1, 1)), sidb_technology.cell_type.INPUT)

            elif index == 2:
                self.assertEqual(lyt.get_cell_type((0, 0, 1)), sidb_technology.cell_type.EMPTY)
                self.assertEqual(lyt.get_cell_type((2, 1, 1)), sidb_technology.cell_type.INPUT)

                self.assertEqual(lyt.get_cell_type((20, 0, 1)), sidb_technology.cell_type.INPUT)
                self.assertEqual(lyt.get_cell_type((18, 1, 1)), sidb_technology.cell_type.EMPTY)

            elif index == 3:
                self.assertEqual(lyt.get_cell_type((0, 0, 1)), sidb_technology.cell_type.EMPTY)
                self.assertEqual(lyt.get_cell_type((2, 1, 1)), sidb_technology.cell_type.INPUT)

                self.assertEqual(lyt.get_cell_type((20, 0, 1)), sidb_technology.cell_type.EMPTY)
                self.assertEqual(lyt.get_cell_type((18, 1, 1)), sidb_technology.cell_type.INPUT)

            else:
                break


if __name__ == '__main__':
    unittest.main()
