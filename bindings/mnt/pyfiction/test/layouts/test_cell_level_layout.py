from mnt.pyfiction import qca_technology, inml_technology, sidb_technology, qca_layout
import unittest


class TestCellTechnology(unittest.TestCase):
    def test_qca_technology(self):
        qca = qca_technology

        self.assertEqual(str(qca.cell_type.EMPTY), "cell_type.EMPTY")
        self.assertEqual(str(qca.cell_type.NORMAL), "cell_type.NORMAL")
        self.assertEqual(str(qca.cell_type.INPUT), "cell_type.INPUT")
        self.assertEqual(str(qca.cell_type.OUTPUT), "cell_type.OUTPUT")
        self.assertEqual(str(qca.cell_type.CONST_0), "cell_type.CONST_0")
        self.assertEqual(str(qca.cell_type.CONST_1), "cell_type.CONST_1")

    def test_inml_technology(self):
        inml = inml_technology

        self.assertEqual(str(inml.cell_type.EMPTY), "cell_type.EMPTY")
        self.assertEqual(str(inml.cell_type.NORMAL), "cell_type.NORMAL")
        self.assertEqual(str(inml.cell_type.INPUT), "cell_type.INPUT")
        self.assertEqual(str(inml.cell_type.SLANTED_EDGE_UP_MAGNET), "cell_type.SLANTED_EDGE_UP_MAGNET")
        self.assertEqual(str(inml.cell_type.SLANTED_EDGE_DOWN_MAGNET), "cell_type.SLANTED_EDGE_DOWN_MAGNET")
        self.assertEqual(str(inml.cell_type.INVERTER_MAGNET), "cell_type.INVERTER_MAGNET")
        self.assertEqual(str(inml.cell_type.CROSSWIRE_MAGNET), "cell_type.CROSSWIRE_MAGNET")
        self.assertEqual(str(inml.cell_type.FANOUT_COUPLER_MAGNET), "cell_type.FANOUT_COUPLER_MAGNET")

    def test_sidb_technology(self):
        sidb = sidb_technology

        self.assertEqual(str(sidb.cell_type.EMPTY), "cell_type.EMPTY")
        self.assertEqual(str(sidb.cell_type.NORMAL), "cell_type.NORMAL")
        self.assertEqual(str(sidb.cell_type.INPUT), "cell_type.INPUT")
        self.assertEqual(str(sidb.cell_type.OUTPUT), "cell_type.OUTPUT")
        self.assertEqual(str(sidb.cell_type.LOGIC), "cell_type.LOGIC")


class TestQCACellLevelLayout(unittest.TestCase):
    def test_qca_cell_layout_inheritance(self):
        layout = qca_layout((9, 9, 1))

        for t in layout.coordinates():
            self.assertTrue(t <= (9, 9, 1))
            self.assertTrue(layout.is_within_bounds(t))

        for t in layout.ground_coordinates():
            self.assertTrue(t.z == 0)
            self.assertTrue(t <= (9, 9, 0))
            self.assertTrue(layout.is_within_bounds(t))

        for t in layout.adjacent_coordinates((2, 2)):
            self.assertIn(t, [(1, 2), (2, 1), (3, 2), (2, 3)])

    def test_cell_type_assignment(self):
        layout = qca_layout((4, 4), "OPEN", "AND")

        self.assertTrue(layout.is_empty())

        layout.assign_cell_type((0, 2), qca_technology.cell_type.INPUT)
        layout.assign_cell_type((2, 4), qca_technology.cell_type.INPUT)
        layout.assign_cell_type((2, 0), qca_technology.cell_type.CONST_0)
        layout.assign_cell_type((2, 1), qca_technology.cell_type.NORMAL)
        layout.assign_cell_type((2, 2), qca_technology.cell_type.NORMAL)
        layout.assign_cell_type((2, 3), qca_technology.cell_type.NORMAL)
        layout.assign_cell_type((1, 2), qca_technology.cell_type.NORMAL)
        layout.assign_cell_type((3, 2), qca_technology.cell_type.NORMAL)
        layout.assign_cell_type((4, 2), qca_technology.cell_type.OUTPUT)

        self.assertFalse(layout.is_empty())

        layout.assign_cell_name((0, 2), "a")
        layout.assign_cell_name((2, 4), "b")
        layout.assign_cell_name((4, 2), "f")

        self.assertEqual(layout.get_layout_name(), "AND")
        self.assertEqual(layout.get_cell_name((0, 2)), "a")
        self.assertEqual(layout.get_cell_name((2, 4)), "b")
        self.assertEqual(layout.get_cell_name((4, 2)), "f")

        self.assertEqual(layout.num_cells(), 9)
        self.assertEqual(layout.num_pis(), 2)
        self.assertEqual(layout.num_pos(), 1)

        self.assertTrue(layout.is_pi((0, 2)))
        self.assertTrue(layout.is_pi((2, 4)))
        self.assertTrue(layout.is_po((4, 2)))

        self.assertEqual(layout.get_cell_type((2, 0)), qca_technology.cell_type.CONST_0)
        self.assertEqual(layout.get_cell_type((2, 4)), qca_technology.cell_type.INPUT)
        self.assertEqual(layout.get_cell_type((0, 2)), qca_technology.cell_type.INPUT)
        self.assertEqual(layout.get_cell_type((2, 1)), qca_technology.cell_type.NORMAL)
        self.assertEqual(layout.get_cell_type((2, 2)), qca_technology.cell_type.NORMAL)
        self.assertEqual(layout.get_cell_type((2, 3)), qca_technology.cell_type.NORMAL)
        self.assertEqual(layout.get_cell_type((1, 2)), qca_technology.cell_type.NORMAL)
        self.assertEqual(layout.get_cell_type((3, 2)), qca_technology.cell_type.NORMAL)
        self.assertEqual(layout.get_cell_type((4, 2)), qca_technology.cell_type.OUTPUT)

        self.assertTrue(layout.is_empty_cell((0, 0)))
        self.assertTrue(layout.is_empty_cell((0, 1)))
        self.assertTrue(layout.is_empty_cell((1, 0)))
        self.assertTrue(layout.is_empty_cell((1, 1)))
        self.assertTrue(layout.is_empty_cell((3, 0)))
        self.assertTrue(layout.is_empty_cell((3, 1)))
        self.assertTrue(layout.is_empty_cell((4, 0)))
        self.assertTrue(layout.is_empty_cell((4, 1)))
        self.assertTrue(layout.is_empty_cell((0, 3)))
        self.assertTrue(layout.is_empty_cell((1, 3)))
        self.assertTrue(layout.is_empty_cell((0, 4)))
        self.assertTrue(layout.is_empty_cell((1, 0)))
        self.assertTrue(layout.is_empty_cell((3, 3)))
        self.assertTrue(layout.is_empty_cell((3, 4)))
        self.assertTrue(layout.is_empty_cell((4, 3)))
        self.assertTrue(layout.is_empty_cell((4, 4)))

        self.assertFalse(layout.is_empty_cell((2, 0)))
        self.assertFalse(layout.is_empty_cell((2, 4)))
        self.assertFalse(layout.is_empty_cell((0, 2)))
        self.assertFalse(layout.is_empty_cell((2, 1)))
        self.assertFalse(layout.is_empty_cell((2, 2)))
        self.assertFalse(layout.is_empty_cell((2, 3)))
        self.assertFalse(layout.is_empty_cell((1, 2)))
        self.assertFalse(layout.is_empty_cell((3, 2)))
        self.assertFalse(layout.is_empty_cell((4, 2)))


if __name__ == "__main__":
    unittest.main()
