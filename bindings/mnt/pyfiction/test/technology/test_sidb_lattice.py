import unittest

from mnt.pyfiction import sidb_lattice, sidb_technology


class TestSiDBLattice(unittest.TestCase):
    def test_qca_cell_layout_inheritance(self):
        layout = sidb_lattice((9, 9), orientation="100")

        for t in layout.coordinates():
            self.assertTrue(t <= (9, 9, 1))
            self.assertTrue(layout.is_within_bounds(t))

        for t in layout.ground_coordinates():
            self.assertTrue(t.z == 0)
            self.assertTrue(t <= (9, 9, 0))
            self.assertTrue(layout.is_within_bounds(t))

        for t in layout.adjacent_coordinates((2, 2)):
            self.assertIn(t, [(1, 2), (2, 1), (3, 2), (2, 3)])

    def test_cell_type_assignment_100_lattice(self):
        layout = sidb_lattice((4, 4), orientation="100", name="AND")

        self.assertTrue(layout.is_empty())

        layout.assign_cell_type((0, 2), sidb_technology.cell_type.INPUT)
        layout.assign_cell_type((2, 4), sidb_technology.cell_type.INPUT)
        layout.assign_cell_type((2, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((2, 2), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((2, 3), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((1, 2), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((3, 2), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((4, 2), sidb_technology.cell_type.OUTPUT)

        self.assertFalse(layout.is_empty())

        layout.assign_cell_name((0, 2), "a")
        layout.assign_cell_name((2, 4), "b")
        layout.assign_cell_name((4, 2), "f")

        self.assertEqual(layout.get_layout_name(), "AND")
        self.assertEqual(layout.get_cell_name((0, 2)), "a")
        self.assertEqual(layout.get_cell_name((2, 4)), "b")
        self.assertEqual(layout.get_cell_name((4, 2)), "f")

        self.assertEqual(layout.num_cells(), 8)
        self.assertEqual(layout.num_pis(), 2)
        self.assertEqual(layout.num_pos(), 1)

        self.assertTrue(layout.is_pi((0, 2)))
        self.assertTrue(layout.is_pi((2, 4)))
        self.assertTrue(layout.is_po((4, 2)))

        self.assertEqual(layout.get_cell_type((2, 4)), sidb_technology.cell_type.INPUT)
        self.assertEqual(layout.get_cell_type((0, 2)), sidb_technology.cell_type.INPUT)
        self.assertEqual(layout.get_cell_type((2, 1)), sidb_technology.cell_type.NORMAL)
        self.assertEqual(layout.get_cell_type((2, 2)), sidb_technology.cell_type.NORMAL)
        self.assertEqual(layout.get_cell_type((2, 3)), sidb_technology.cell_type.NORMAL)
        self.assertEqual(layout.get_cell_type((1, 2)), sidb_technology.cell_type.NORMAL)
        self.assertEqual(layout.get_cell_type((3, 2)), sidb_technology.cell_type.NORMAL)
        self.assertEqual(layout.get_cell_type((4, 2)), sidb_technology.cell_type.OUTPUT)

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

        self.assertFalse(layout.is_empty_cell((2, 4)))
        self.assertFalse(layout.is_empty_cell((0, 2)))
        self.assertFalse(layout.is_empty_cell((2, 1)))
        self.assertFalse(layout.is_empty_cell((2, 2)))
        self.assertFalse(layout.is_empty_cell((2, 3)))
        self.assertFalse(layout.is_empty_cell((1, 2)))
        self.assertFalse(layout.is_empty_cell((3, 2)))
        self.assertFalse(layout.is_empty_cell((4, 2)))

    def test_cell_type_assignment_111_lattice(self):
        layout = sidb_lattice((4, 4), orientation="111", name="AND")

        self.assertTrue(layout.is_empty())

        layout.assign_cell_type((0, 2), sidb_technology.cell_type.INPUT)
        layout.assign_cell_type((2, 4), sidb_technology.cell_type.INPUT)
        layout.assign_cell_type((2, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((2, 2), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((2, 3), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((1, 2), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((3, 2), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((4, 2), sidb_technology.cell_type.OUTPUT)

        self.assertFalse(layout.is_empty())

        layout.assign_cell_name((0, 2), "a")
        layout.assign_cell_name((2, 4), "b")
        layout.assign_cell_name((4, 2), "f")

        self.assertEqual(layout.get_layout_name(), "AND")
        self.assertEqual(layout.get_cell_name((0, 2)), "a")
        self.assertEqual(layout.get_cell_name((2, 4)), "b")
        self.assertEqual(layout.get_cell_name((4, 2)), "f")

        self.assertEqual(layout.num_cells(), 8)
        self.assertEqual(layout.num_pis(), 2)
        self.assertEqual(layout.num_pos(), 1)

        self.assertTrue(layout.is_pi((0, 2)))
        self.assertTrue(layout.is_pi((2, 4)))
        self.assertTrue(layout.is_po((4, 2)))

        self.assertEqual(layout.get_cell_type((2, 4)), sidb_technology.cell_type.INPUT)
        self.assertEqual(layout.get_cell_type((0, 2)), sidb_technology.cell_type.INPUT)
        self.assertEqual(layout.get_cell_type((2, 1)), sidb_technology.cell_type.NORMAL)
        self.assertEqual(layout.get_cell_type((2, 2)), sidb_technology.cell_type.NORMAL)
        self.assertEqual(layout.get_cell_type((2, 3)), sidb_technology.cell_type.NORMAL)
        self.assertEqual(layout.get_cell_type((1, 2)), sidb_technology.cell_type.NORMAL)
        self.assertEqual(layout.get_cell_type((3, 2)), sidb_technology.cell_type.NORMAL)
        self.assertEqual(layout.get_cell_type((4, 2)), sidb_technology.cell_type.OUTPUT)

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
