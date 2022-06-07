from fiction import pyfiction
import unittest


class TestGateLevelLayout(unittest.TestCase):

    def test_gate_level_layout_inheritance(self):
        layout = pyfiction.cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout")

        self.assertEqual(layout.incoming_clocked_zones((0, 0)), set())
        self.assertEqual(layout.outgoing_clocked_zones((2, 2)), set())

        for icz in layout.incoming_clocked_zones((1, 1)):
            self.assertIn(icz, [pyfiction.coordinate(1, 0), pyfiction.coordinate(0, 1)])

        for icz in layout.outgoing_clocked_zones((1, 1)):
            self.assertIn(icz, [pyfiction.coordinate(1, 2), pyfiction.coordinate(2, 1)])

    def test_gate_level_layout_iteration(self):
        layout = pyfiction.cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout")

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
        self.assertIn(pyfiction.coordinate(x1), pis)
        self.assertIn(pyfiction.coordinate(x2), pis)
        self.assertIn(pyfiction.coordinate(x3), pis)
        self.assertIn(pyfiction.coordinate(x4), pis)

        # POs
        pos = layout.pos()
        self.assertEqual(len(pos), 2)
        self.assertIn(pyfiction.coordinate(f1), pos)
        self.assertIn(pyfiction.coordinate(f2), pos)

        # gates
        gates = layout.gates()
        self.assertEqual(len(gates), 7)
        self.assertIn(pyfiction.coordinate(a1), gates)
        self.assertIn(pyfiction.coordinate(a2), gates)
        self.assertIn(pyfiction.coordinate(b1), gates)
        self.assertIn(pyfiction.coordinate(b2), gates)
        self.assertIn(pyfiction.coordinate(c), gates)
        self.assertIn(pyfiction.coordinate(f1), gates)
        self.assertIn(pyfiction.coordinate(f2), gates)

        # wires
        wires = layout.wires()
        self.assertEqual(len(wires), 9)
        self.assertIn(pyfiction.coordinate(x1), wires)
        self.assertIn(pyfiction.coordinate(x2), wires)
        self.assertIn(pyfiction.coordinate(x3), wires)
        self.assertIn(pyfiction.coordinate(x4), wires)
        self.assertIn(pyfiction.coordinate(b1), wires)
        self.assertIn(pyfiction.coordinate(b2), wires)
        self.assertIn(pyfiction.coordinate(c), wires)
        self.assertIn(pyfiction.coordinate(f1), wires)
        self.assertIn(pyfiction.coordinate(f2), wires)

        # incoming data flow
        inx1 = layout.incoming_data_flow(pyfiction.coordinate(x1))
        self.assertEqual(len(inx1), 0)

        inf1 = layout.incoming_data_flow(pyfiction.coordinate(f1))
        self.assertEqual(len(inf1), 1)
        self.assertIn(pyfiction.coordinate(c), inf1)
        
        ina2 = layout.incoming_data_flow(pyfiction.coordinate(a2))
        self.assertEqual(len(ina2), 2)
        self.assertIn(pyfiction.coordinate(b1), ina2)
        self.assertIn(pyfiction.coordinate(b2), ina2)

        # outgoing data flow
        outx1 = layout.outgoing_data_flow(pyfiction.coordinate(x1))
        self.assertEqual(len(outx1), 1)
        self.assertIn(pyfiction.coordinate(a1), outx1)

        outf1 = layout.outgoing_data_flow(pyfiction.coordinate(f1))
        self.assertEqual(len(outf1), 0)

        outa2 = layout.outgoing_data_flow(pyfiction.coordinate(a2))
        self.assertEqual(len(outa2), 1)
        self.assertIn(pyfiction.coordinate(f2), outa2)


if __name__ == '__main__':
    unittest.main()
