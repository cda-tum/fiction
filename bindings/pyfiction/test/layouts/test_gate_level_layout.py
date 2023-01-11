from fiction.pyfiction import *
import unittest


class TestCartesianGateLevelLayout(unittest.TestCase):

    def test_cartesian_gate_level_layout_inheritance(self):
        layout = cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout")

        self.assertEqual(layout.incoming_clocked_zones((0, 0)), [])
        self.assertEqual(layout.outgoing_clocked_zones((2, 2)), [])

        for icz in layout.incoming_clocked_zones((1, 1)):
            self.assertIn(icz, [coordinate(1, 0), coordinate(0, 1)])

        for icz in layout.outgoing_clocked_zones((1, 1)):
            self.assertIn(icz, [coordinate(1, 2), coordinate(2, 1)])

    def test_cartesian_gate_level_layout_iteration(self):
        layout = cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout")

        self.assertTrue(layout.is_empty())

        # layout creation
        x1 = layout.create_pi("x1", (1, 0))
        x2 = layout.create_pi("x2", (0, 1))
        x3 = layout.create_pi("x3", (2, 0))
        x4 = layout.create_pi("x4", (0, 2))

        a1 = layout.create_and(x1, x2, (1, 1))

        b1 = layout.create_buf(x3, (2, 1))
        b2 = layout.create_buf(x4, (1, 2))

        a2 = layout.create_and(b1, b2, (2, 2))

        c = layout.create_buf(a1, (2, 1, 1))

        f1 = layout.create_po(c, "f1", (3, 1))
        f2 = layout.create_po(a2, "f2", (3, 2))

        self.assertFalse(layout.is_empty())

        # Pis
        pis = layout.pis()
        self.assertEqual(len(pis), 4)
        self.assertIn(coordinate(x1), pis)
        self.assertIn(coordinate(x2), pis)
        self.assertIn(coordinate(x3), pis)
        self.assertIn(coordinate(x4), pis)
        self.assertEqual(layout.get_node(coordinate(x1)), 2)
        self.assertEqual(layout.get_node(coordinate(x2)), 3)
        self.assertEqual(layout.get_node(coordinate(x3)), 4)
        self.assertEqual(layout.get_node(coordinate(x4)), 5)
        self.assertEqual(layout.get_tile(2), coordinate(x1))
        self.assertEqual(layout.get_tile(3), coordinate(x2))
        self.assertEqual(layout.get_tile(4), coordinate(x3))
        self.assertEqual(layout.get_tile(5), coordinate(x4))

        # POs
        pos = layout.pos()
        self.assertEqual(len(pos), 2)
        self.assertIn(coordinate(f1), pos)
        self.assertIn(coordinate(f2), pos)
        self.assertEqual(layout.get_node(coordinate(f1)), 11)
        self.assertEqual(layout.get_node(coordinate(f2)), 12)
        self.assertEqual(layout.get_tile(11), coordinate(f1))
        self.assertEqual(layout.get_tile(12), coordinate(f2))

        # gates
        gates = layout.gates()
        self.assertEqual(len(gates), 7)
        self.assertIn(coordinate(a1), gates)
        self.assertIn(coordinate(a2), gates)
        self.assertIn(coordinate(b1), gates)
        self.assertIn(coordinate(b2), gates)
        self.assertIn(coordinate(c), gates)
        self.assertIn(coordinate(f1), gates)
        self.assertIn(coordinate(f2), gates)
        self.assertEqual(layout.get_node(coordinate(a1)), 6)
        self.assertEqual(layout.get_node(coordinate(b1)), 7)
        self.assertEqual(layout.get_node(coordinate(b2)), 8)
        self.assertEqual(layout.get_node(coordinate(a2)), 9)
        self.assertEqual(layout.get_node(coordinate(c)), 10)
        self.assertEqual(layout.get_tile(6), coordinate(a1))
        self.assertEqual(layout.get_tile(7), coordinate(b1))
        self.assertEqual(layout.get_tile(8), coordinate(b2))
        self.assertEqual(layout.get_tile(9), coordinate(a2))
        self.assertEqual(layout.get_tile(10), coordinate(c))

        # wires
        wires = layout.wires()
        self.assertEqual(len(wires), 9)
        self.assertIn(coordinate(x1), wires)
        self.assertIn(coordinate(x2), wires)
        self.assertIn(coordinate(x3), wires)
        self.assertIn(coordinate(x4), wires)
        self.assertIn(coordinate(b1), wires)
        self.assertIn(coordinate(b2), wires)
        self.assertIn(coordinate(c), wires)
        self.assertIn(coordinate(f1), wires)
        self.assertIn(coordinate(f2), wires)

        # incoming data flow
        inx1 = layout.fanins(coordinate(x1))
        self.assertEqual(len(inx1), 0)

        inf1 = layout.fanins(coordinate(f1))
        self.assertEqual(len(inf1), 1)
        self.assertIn(coordinate(c), inf1)

        ina2 = layout.fanins(coordinate(a2))
        self.assertEqual(len(ina2), 2)
        self.assertIn(coordinate(b1), ina2)
        self.assertIn(coordinate(b2), ina2)

        # outgoing data flow
        outx1 = layout.fanouts(coordinate(x1))
        self.assertEqual(len(outx1), 1)
        self.assertIn(coordinate(a1), outx1)

        outf1 = layout.fanouts(coordinate(f1))
        self.assertEqual(len(outf1), 0)

        outa2 = layout.fanouts(coordinate(a2))
        self.assertEqual(len(outa2), 1)
        self.assertIn(coordinate(f2), outa2)

        cp, tp = critical_path_length_and_throughput(layout)
        self.assertEqual(cp, 4)
        self.assertEqual(tp, 1)

        self.assertEqual(gate_level_drvs(layout), (0, 0))


class TestHexagonalGateLevelLayout(unittest.TestCase):

    def test_hexagonal_gate_level_layout_inheritance(self):
        layout = hexagonal_gate_layout((2, 2, 0), "2DDWave", "Layout")

        self.assertEqual(layout.incoming_clocked_zones((0, 0)), [])
        self.assertEqual(layout.outgoing_clocked_zones((2, 2)), [])

        for icz in layout.incoming_clocked_zones((1, 1)):
            self.assertIn(icz, [coordinate(1, 0), coordinate(0, 1)])

        for icz in layout.outgoing_clocked_zones((1, 1)):
            self.assertIn(icz, [coordinate(1, 2), coordinate(2, 1)])

    def test_hexagonal_gate_level_layout_iteration(self):
        layout = hexagonal_gate_layout((3, 3, 1), "2DDWave", "Layout")

        self.assertTrue(layout.is_empty())

        # layout creation
        x1 = layout.create_pi("x1", (1, 0))
        x2 = layout.create_pi("x2", (0, 1))
        x3 = layout.create_pi("x3", (2, 0))
        x4 = layout.create_pi("x4", (0, 2))

        a1 = layout.create_and(x1, x2, (1, 1))

        b1 = layout.create_buf(x3, (2, 1))
        b2 = layout.create_buf(x4, (1, 2))

        a2 = layout.create_and(b1, b2, (2, 2))

        c = layout.create_buf(a1, (2, 1, 1))

        f1 = layout.create_po(c, "f1", (3, 1))
        f2 = layout.create_po(a2, "f2", (3, 2))

        self.assertFalse(layout.is_empty())

        # Pis
        pis = layout.pis()
        self.assertEqual(len(pis), 4)
        self.assertIn(coordinate(x1), pis)
        self.assertIn(coordinate(x2), pis)
        self.assertIn(coordinate(x3), pis)
        self.assertIn(coordinate(x4), pis)
        self.assertEqual(layout.get_node(coordinate(x1)), 2)
        self.assertEqual(layout.get_node(coordinate(x2)), 3)
        self.assertEqual(layout.get_node(coordinate(x3)), 4)
        self.assertEqual(layout.get_node(coordinate(x4)), 5)
        self.assertEqual(layout.get_tile(2), coordinate(x1))
        self.assertEqual(layout.get_tile(3), coordinate(x2))
        self.assertEqual(layout.get_tile(4), coordinate(x3))
        self.assertEqual(layout.get_tile(5), coordinate(x4))

        # POs
        pos = layout.pos()
        self.assertEqual(len(pos), 2)
        self.assertIn(coordinate(f1), pos)
        self.assertIn(coordinate(f2), pos)
        self.assertEqual(layout.get_node(coordinate(f1)), 11)
        self.assertEqual(layout.get_node(coordinate(f2)), 12)
        self.assertEqual(layout.get_tile(11), coordinate(f1))
        self.assertEqual(layout.get_tile(12), coordinate(f2))

        # gates
        gates = layout.gates()
        self.assertEqual(len(gates), 7)
        self.assertIn(coordinate(a1), gates)
        self.assertIn(coordinate(a2), gates)
        self.assertIn(coordinate(b1), gates)
        self.assertIn(coordinate(b2), gates)
        self.assertIn(coordinate(c), gates)
        self.assertIn(coordinate(f1), gates)
        self.assertIn(coordinate(f2), gates)
        self.assertEqual(layout.get_node(coordinate(a1)), 6)
        self.assertEqual(layout.get_node(coordinate(b1)), 7)
        self.assertEqual(layout.get_node(coordinate(b2)), 8)
        self.assertEqual(layout.get_node(coordinate(a2)), 9)
        self.assertEqual(layout.get_node(coordinate(c)), 10)
        self.assertEqual(layout.get_tile(6), coordinate(a1))
        self.assertEqual(layout.get_tile(7), coordinate(b1))
        self.assertEqual(layout.get_tile(8), coordinate(b2))
        self.assertEqual(layout.get_tile(9), coordinate(a2))
        self.assertEqual(layout.get_tile(10), coordinate(c))

        # wires
        wires = layout.wires()
        self.assertEqual(len(wires), 9)
        self.assertIn(coordinate(x1), wires)
        self.assertIn(coordinate(x2), wires)
        self.assertIn(coordinate(x3), wires)
        self.assertIn(coordinate(x4), wires)
        self.assertIn(coordinate(b1), wires)
        self.assertIn(coordinate(b2), wires)
        self.assertIn(coordinate(c), wires)
        self.assertIn(coordinate(f1), wires)
        self.assertIn(coordinate(f2), wires)

        # incoming data flow
        inx1 = layout.fanins(coordinate(x1))
        self.assertEqual(len(inx1), 0)

        inf1 = layout.fanins(coordinate(f1))
        self.assertEqual(len(inf1), 1)
        self.assertIn(coordinate(c), inf1)

        ina2 = layout.fanins(coordinate(a2))
        self.assertEqual(len(ina2), 2)
        self.assertIn(coordinate(b1), ina2)
        self.assertIn(coordinate(b2), ina2)

        # outgoing data flow
        outx1 = layout.fanouts(coordinate(x1))
        self.assertEqual(len(outx1), 1)
        self.assertIn(coordinate(a1), outx1)

        outf1 = layout.fanouts(coordinate(f1))
        self.assertEqual(len(outf1), 0)

        outa2 = layout.fanouts(coordinate(a2))
        self.assertEqual(len(outa2), 1)
        self.assertIn(coordinate(f2), outa2)

        cp, tp = critical_path_length_and_throughput(layout)
        self.assertEqual(cp, 4)
        self.assertEqual(tp, 1)

        self.assertEqual(gate_level_drvs(layout), (0, 0))


if __name__ == '__main__':
    unittest.main()
