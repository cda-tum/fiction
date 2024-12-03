import unittest

from mnt.pyfiction import (
    cartesian_gate_layout,
    cartesian_obstruction_layout,
    critical_path_length_and_throughput,
    gate_level_drv_params,
    gate_level_drvs,
    hexagonal_gate_layout,
    hexagonal_obstruction_layout,
    offset_coordinate,
    shifted_cartesian_gate_layout,
    shifted_cartesian_obstruction_layout,
)


class TestObstructionLayout(unittest.TestCase):
    def test_obstruction_layout_clocking_inheritance(self):
        for layout in [
            cartesian_obstruction_layout(cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout")),
            shifted_cartesian_obstruction_layout(shifted_cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout")),
            hexagonal_obstruction_layout(hexagonal_gate_layout((2, 2, 0), "2DDWave", "Layout")),
        ]:
            self.assertEqual(layout.incoming_clocked_zones((0, 0)), [])
            self.assertEqual(layout.outgoing_clocked_zones((2, 2)), [])

            for icz in layout.incoming_clocked_zones((1, 1)):
                self.assertIn(icz, [offset_coordinate(1, 0), offset_coordinate(0, 1)])

            for icz in layout.outgoing_clocked_zones((1, 1)):
                self.assertIn(icz, [offset_coordinate(1, 2), offset_coordinate(2, 1)])

    def test_obstructed_coordinates(self):
        for layout in [
            cartesian_obstruction_layout(cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout")),
            shifted_cartesian_obstruction_layout(shifted_cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout")),
            hexagonal_obstruction_layout(hexagonal_gate_layout((3, 3, 1), "2DDWave", "Layout")),
        ]:
            for c in layout.coordinates():
                self.assertFalse(layout.is_obstructed_coordinate(c))

            layout.obstruct_coordinate((0, 0))
            layout.obstruct_coordinate((1, 1))
            layout.obstruct_coordinate((2, 2))

            self.assertTrue(layout.is_obstructed_coordinate((0, 0)))
            self.assertTrue(layout.is_obstructed_coordinate((1, 1)))
            self.assertTrue(layout.is_obstructed_coordinate((2, 2)))

    def test_obstructed_connections(self):
        for layout in [
            cartesian_obstruction_layout(cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout")),
            shifted_cartesian_obstruction_layout(shifted_cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout")),
            hexagonal_obstruction_layout(hexagonal_gate_layout((3, 3, 1), "2DDWave", "Layout")),
        ]:
            for c1 in layout.coordinates():
                for c2 in layout.coordinates():
                    self.assertFalse(layout.is_obstructed_connection(c1, c2))

            layout.obstruct_connection((0, 0), (1, 1))
            layout.obstruct_connection((1, 1), (2, 2))

            self.assertTrue(layout.is_obstructed_connection((0, 0), (1, 1)))
            self.assertTrue(layout.is_obstructed_connection((1, 1), (2, 2)))

    def test_obstruction_via_gates(self):
        for layout in [
            cartesian_obstruction_layout(cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout")),
            shifted_cartesian_obstruction_layout(shifted_cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout")),
            hexagonal_obstruction_layout(hexagonal_gate_layout((3, 3, 1), "2DDWave", "Layout")),
        ]:
            x1 = layout.create_pi("x1", (0, 1))
            x2 = layout.create_pi("x2", (3, 2))
            x3 = layout.create_pi("x3", (2, 0))

            buf1 = layout.create_buf(x3, (2, 1))
            layout.create_buf(buf1, (2, 2))

            layout.create_and(x1, x2, (4, 2))

            self.assertTrue(layout.is_obstructed_coordinate((0, 1)))
            self.assertTrue(layout.is_obstructed_coordinate((3, 2)))
            self.assertTrue(layout.is_obstructed_coordinate((2, 0)))
            self.assertTrue(layout.is_obstructed_coordinate((2, 1)))
            self.assertTrue(layout.is_obstructed_coordinate((2, 2)))
            self.assertTrue(layout.is_obstructed_coordinate((4, 2)))

            self.assertTrue(layout.is_obstructed_connection((2, 0), (2, 1)))
            self.assertTrue(layout.is_obstructed_connection((2, 1), (2, 2)))
            self.assertTrue(layout.is_obstructed_connection((3, 2), (4, 2)))


class TestCartesianObstructionLayout(unittest.TestCase):
    def test_cartesian_obstruction_layout_gate_level_inheritance(self):
        layout = cartesian_obstruction_layout(cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout"))

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
        self.assertIn(offset_coordinate(x1), pis)
        self.assertIn(offset_coordinate(x2), pis)
        self.assertIn(offset_coordinate(x3), pis)
        self.assertIn(offset_coordinate(x4), pis)
        self.assertEqual(layout.get_node(offset_coordinate(x1)), 2)
        self.assertEqual(layout.get_node(offset_coordinate(x2)), 3)
        self.assertEqual(layout.get_node(offset_coordinate(x3)), 4)
        self.assertEqual(layout.get_node(offset_coordinate(x4)), 5)
        self.assertEqual(layout.get_tile(2), offset_coordinate(x1))
        self.assertEqual(layout.get_tile(3), offset_coordinate(x2))
        self.assertEqual(layout.get_tile(4), offset_coordinate(x3))
        self.assertEqual(layout.get_tile(5), offset_coordinate(x4))
        self.assertEqual(layout.make_signal(2), x1)
        self.assertEqual(layout.make_signal(3), x2)
        self.assertEqual(layout.make_signal(4), x3)
        self.assertEqual(layout.make_signal(5), x4)

        # POs
        pos = layout.pos()
        self.assertEqual(len(pos), 2)
        self.assertIn(offset_coordinate(f1), pos)
        self.assertIn(offset_coordinate(f2), pos)
        self.assertEqual(layout.get_node(offset_coordinate(f1)), 11)
        self.assertEqual(layout.get_node(offset_coordinate(f2)), 12)
        self.assertEqual(layout.get_tile(11), offset_coordinate(f1))
        self.assertEqual(layout.get_tile(12), offset_coordinate(f2))
        self.assertEqual(layout.make_signal(11), f1)
        self.assertEqual(layout.make_signal(12), f2)

        # gates
        gates = layout.gates()
        self.assertEqual(len(gates), 7)
        self.assertIn(offset_coordinate(a1), gates)
        self.assertIn(offset_coordinate(a2), gates)
        self.assertIn(offset_coordinate(b1), gates)
        self.assertIn(offset_coordinate(b2), gates)
        self.assertIn(offset_coordinate(c), gates)
        self.assertIn(offset_coordinate(f1), gates)
        self.assertIn(offset_coordinate(f2), gates)
        self.assertEqual(layout.get_node(offset_coordinate(a1)), 6)
        self.assertEqual(layout.get_node(offset_coordinate(b1)), 7)
        self.assertEqual(layout.get_node(offset_coordinate(b2)), 8)
        self.assertEqual(layout.get_node(offset_coordinate(a2)), 9)
        self.assertEqual(layout.get_node(offset_coordinate(c)), 10)
        self.assertEqual(layout.get_tile(6), offset_coordinate(a1))
        self.assertEqual(layout.get_tile(7), offset_coordinate(b1))
        self.assertEqual(layout.get_tile(8), offset_coordinate(b2))
        self.assertEqual(layout.get_tile(9), offset_coordinate(a2))
        self.assertEqual(layout.get_tile(10), offset_coordinate(c))
        self.assertEqual(layout.make_signal(6), a1)
        self.assertEqual(layout.make_signal(7), b1)
        self.assertEqual(layout.make_signal(8), b2)
        self.assertEqual(layout.make_signal(9), a2)
        self.assertEqual(layout.make_signal(10), c)

        # wires
        wires = layout.wires()
        self.assertEqual(len(wires), 9)
        self.assertIn(offset_coordinate(x1), wires)
        self.assertIn(offset_coordinate(x2), wires)
        self.assertIn(offset_coordinate(x3), wires)
        self.assertIn(offset_coordinate(x4), wires)
        self.assertIn(offset_coordinate(b1), wires)
        self.assertIn(offset_coordinate(b2), wires)
        self.assertIn(offset_coordinate(c), wires)
        self.assertIn(offset_coordinate(f1), wires)
        self.assertIn(offset_coordinate(f2), wires)

        # incoming data flow
        inx1 = layout.fanins(offset_coordinate(x1))
        self.assertEqual(len(inx1), 0)

        inf1 = layout.fanins(offset_coordinate(f1))
        self.assertEqual(len(inf1), 1)
        self.assertIn(offset_coordinate(c), inf1)

        ina2 = layout.fanins(offset_coordinate(a2))
        self.assertEqual(len(ina2), 2)
        self.assertIn(offset_coordinate(b1), ina2)
        self.assertIn(offset_coordinate(b2), ina2)

        # outgoing data flow
        outx1 = layout.fanouts(offset_coordinate(x1))
        self.assertEqual(len(outx1), 1)
        self.assertIn(offset_coordinate(a1), outx1)

        outf1 = layout.fanouts(offset_coordinate(f1))
        self.assertEqual(len(outf1), 0)

        outa2 = layout.fanouts(offset_coordinate(a2))
        self.assertEqual(len(outa2), 1)
        self.assertIn(offset_coordinate(f2), outa2)

        cp, tp = critical_path_length_and_throughput(layout)
        self.assertEqual(cp, 4)
        self.assertEqual(tp, 1)

        drv_params = gate_level_drv_params()
        self.assertEqual(gate_level_drvs(layout, drv_params, False), (0, 0))


class TestHexagonalObstructionLayout(unittest.TestCase):
    def test_hexagonal_obstruction_layout_gate_level_inheritance(self):
        layout = hexagonal_obstruction_layout(hexagonal_gate_layout((3, 3, 1), "2DDWave", "Layout"))

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
        self.assertIn(offset_coordinate(x1), pis)
        self.assertIn(offset_coordinate(x2), pis)
        self.assertIn(offset_coordinate(x3), pis)
        self.assertIn(offset_coordinate(x4), pis)
        self.assertEqual(layout.get_node(offset_coordinate(x1)), 2)
        self.assertEqual(layout.get_node(offset_coordinate(x2)), 3)
        self.assertEqual(layout.get_node(offset_coordinate(x3)), 4)
        self.assertEqual(layout.get_node(offset_coordinate(x4)), 5)
        self.assertEqual(layout.get_tile(2), offset_coordinate(x1))
        self.assertEqual(layout.get_tile(3), offset_coordinate(x2))
        self.assertEqual(layout.get_tile(4), offset_coordinate(x3))
        self.assertEqual(layout.get_tile(5), offset_coordinate(x4))
        self.assertEqual(layout.make_signal(2), x1)
        self.assertEqual(layout.make_signal(3), x2)
        self.assertEqual(layout.make_signal(4), x3)
        self.assertEqual(layout.make_signal(5), x4)

        # POs
        pos = layout.pos()
        self.assertEqual(len(pos), 2)
        self.assertIn(offset_coordinate(f1), pos)
        self.assertIn(offset_coordinate(f2), pos)
        self.assertEqual(layout.get_node(offset_coordinate(f1)), 11)
        self.assertEqual(layout.get_node(offset_coordinate(f2)), 12)
        self.assertEqual(layout.get_tile(11), offset_coordinate(f1))
        self.assertEqual(layout.get_tile(12), offset_coordinate(f2))
        self.assertEqual(layout.make_signal(11), f1)
        self.assertEqual(layout.make_signal(12), f2)

        # gates
        gates = layout.gates()
        self.assertEqual(len(gates), 7)
        self.assertIn(offset_coordinate(a1), gates)
        self.assertIn(offset_coordinate(a2), gates)
        self.assertIn(offset_coordinate(b1), gates)
        self.assertIn(offset_coordinate(b2), gates)
        self.assertIn(offset_coordinate(c), gates)
        self.assertIn(offset_coordinate(f1), gates)
        self.assertIn(offset_coordinate(f2), gates)
        self.assertEqual(layout.get_node(offset_coordinate(a1)), 6)
        self.assertEqual(layout.get_node(offset_coordinate(b1)), 7)
        self.assertEqual(layout.get_node(offset_coordinate(b2)), 8)
        self.assertEqual(layout.get_node(offset_coordinate(a2)), 9)
        self.assertEqual(layout.get_node(offset_coordinate(c)), 10)
        self.assertEqual(layout.get_tile(6), offset_coordinate(a1))
        self.assertEqual(layout.get_tile(7), offset_coordinate(b1))
        self.assertEqual(layout.get_tile(8), offset_coordinate(b2))
        self.assertEqual(layout.get_tile(9), offset_coordinate(a2))
        self.assertEqual(layout.get_tile(10), offset_coordinate(c))
        self.assertEqual(layout.make_signal(6), a1)
        self.assertEqual(layout.make_signal(7), b1)
        self.assertEqual(layout.make_signal(8), b2)
        self.assertEqual(layout.make_signal(9), a2)
        self.assertEqual(layout.make_signal(10), c)

        # wires
        wires = layout.wires()
        self.assertEqual(len(wires), 9)
        self.assertIn(offset_coordinate(x1), wires)
        self.assertIn(offset_coordinate(x2), wires)
        self.assertIn(offset_coordinate(x3), wires)
        self.assertIn(offset_coordinate(x4), wires)
        self.assertIn(offset_coordinate(b1), wires)
        self.assertIn(offset_coordinate(b2), wires)
        self.assertIn(offset_coordinate(c), wires)
        self.assertIn(offset_coordinate(f1), wires)
        self.assertIn(offset_coordinate(f2), wires)

        # incoming data flow
        inx1 = layout.fanins(offset_coordinate(x1))
        self.assertEqual(len(inx1), 0)

        inf1 = layout.fanins(offset_coordinate(f1))
        self.assertEqual(len(inf1), 1)
        self.assertIn(offset_coordinate(c), inf1)

        ina2 = layout.fanins(offset_coordinate(a2))
        self.assertEqual(len(ina2), 2)
        self.assertIn(offset_coordinate(b1), ina2)
        self.assertIn(offset_coordinate(b2), ina2)

        # outgoing data flow
        outx1 = layout.fanouts(offset_coordinate(x1))
        self.assertEqual(len(outx1), 1)
        self.assertIn(offset_coordinate(a1), outx1)

        outf1 = layout.fanouts(offset_coordinate(f1))
        self.assertEqual(len(outf1), 0)

        outa2 = layout.fanouts(offset_coordinate(a2))
        self.assertEqual(len(outa2), 1)
        self.assertIn(offset_coordinate(f2), outa2)

        cp, tp = critical_path_length_and_throughput(layout)
        self.assertEqual(cp, 4)
        self.assertEqual(tp, 1)

        drv_params = gate_level_drv_params()
        self.assertEqual(gate_level_drvs(layout, drv_params, False), (0, 0))


if __name__ == "__main__":
    unittest.main()
