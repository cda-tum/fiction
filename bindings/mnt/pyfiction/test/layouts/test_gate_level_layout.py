import unittest

from mnt.pyfiction import (
    cartesian_gate_layout,
    critical_path_length_and_throughput,
    cube_coordinate,
    gate_level_drv_params,
    gate_level_drvs,
    hexagonal_gate_layout,
    offset_coordinate,
    shifted_cartesian_gate_layout,
)


class TestCartesianGateLevelLayout(unittest.TestCase):
    def test_gate_level_layout_inheritance(self):
        for layout in [
            cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout"),
            shifted_cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout"),
            hexagonal_gate_layout((2, 2, 0), "2DDWave", "Layout"),
        ]:
            self.assertEqual(layout.incoming_clocked_zones((0, 0)), [])
            self.assertEqual(layout.outgoing_clocked_zones((2, 2)), [])

            for icz in layout.incoming_clocked_zones((1, 1)):
                self.assertIn(icz, [layout.coord(1, 0), layout.coord(0, 1)])

            for icz in layout.outgoing_clocked_zones((1, 1)):
                self.assertIn(icz, [layout.coord(1, 2), layout.coord(2, 1)])

        layout = cartesian_gate_layout((2, 2, 0), "2DDWave", "Layout", "cube")
        for icz in layout.incoming_clocked_zones((0, 0)):
            self.assertIn(icz, [layout.coord(-1, 0), layout.coord(0, -1)])
        self.assertEqual(layout.outgoing_clocked_zones((2, 2)), [])

        for icz in layout.incoming_clocked_zones((1, 1)):
            self.assertIn(icz, [layout.coord(1, 0), layout.coord(0, 1)])

        for icz in layout.outgoing_clocked_zones((1, 1)):
            self.assertIn(icz, [layout.coord(1, 2), layout.coord(2, 1)])

    def test_gate_level_layout_iteration(self):
        for layout in [
            cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout"),
            shifted_cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout"),
            hexagonal_gate_layout((3, 3, 1), "2DDWave", "Layout"),
        ]:
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
            self.assertEqual(layout.get_name(x1), "x1")
            self.assertEqual(layout.get_name(x2), "x2")
            self.assertEqual(layout.get_name(x3), "x3")
            self.assertEqual(layout.get_name(x4), "x4")

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
            self.assertEqual(layout.get_name(f1), "f1")
            self.assertEqual(layout.get_name(f2), "f2")

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

    def test_gate_level_layout_iteration_cube_coordinates(self):
        layout = cartesian_gate_layout((3, 3, 1), "2DDWave", "Layout", "cube")
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
        self.assertIn(cube_coordinate(x1), pis)
        self.assertIn(cube_coordinate(x2), pis)
        self.assertIn(cube_coordinate(x3), pis)
        self.assertIn(cube_coordinate(x4), pis)
        self.assertEqual(layout.get_node(cube_coordinate(x1)), 2)
        self.assertEqual(layout.get_node(cube_coordinate(x2)), 3)
        self.assertEqual(layout.get_node(cube_coordinate(x3)), 4)
        self.assertEqual(layout.get_node(cube_coordinate(x4)), 5)
        self.assertEqual(layout.get_tile(2), cube_coordinate(x1))
        self.assertEqual(layout.get_tile(3), cube_coordinate(x2))
        self.assertEqual(layout.get_tile(4), cube_coordinate(x3))
        self.assertEqual(layout.get_tile(5), cube_coordinate(x4))
        self.assertEqual(layout.make_signal(2), x1)
        self.assertEqual(layout.make_signal(3), x2)
        self.assertEqual(layout.make_signal(4), x3)
        self.assertEqual(layout.make_signal(5), x4)
        self.assertEqual(layout.get_name(x1), "x1")
        self.assertEqual(layout.get_name(x2), "x2")
        self.assertEqual(layout.get_name(x3), "x3")
        self.assertEqual(layout.get_name(x4), "x4")

        # POs
        pos = layout.pos()
        self.assertEqual(len(pos), 2)
        self.assertIn(cube_coordinate(f1), pos)
        self.assertIn(cube_coordinate(f2), pos)
        self.assertEqual(layout.get_node(cube_coordinate(f1)), 11)
        self.assertEqual(layout.get_node(cube_coordinate(f2)), 12)
        self.assertEqual(layout.get_tile(11), cube_coordinate(f1))
        self.assertEqual(layout.get_tile(12), cube_coordinate(f2))
        self.assertEqual(layout.make_signal(11), f1)
        self.assertEqual(layout.make_signal(12), f2)
        self.assertEqual(layout.get_name(f1), "f1")
        self.assertEqual(layout.get_name(f2), "f2")

        # gates
        gates = layout.gates()
        self.assertEqual(len(gates), 7)
        self.assertIn(cube_coordinate(a1), gates)
        self.assertIn(cube_coordinate(a2), gates)
        self.assertIn(cube_coordinate(b1), gates)
        self.assertIn(cube_coordinate(b2), gates)
        self.assertIn(cube_coordinate(c), gates)
        self.assertIn(cube_coordinate(f1), gates)
        self.assertIn(cube_coordinate(f2), gates)
        self.assertEqual(layout.get_node(cube_coordinate(a1)), 6)
        self.assertEqual(layout.get_node(cube_coordinate(b1)), 7)
        self.assertEqual(layout.get_node(cube_coordinate(b2)), 8)
        self.assertEqual(layout.get_node(cube_coordinate(a2)), 9)
        self.assertEqual(layout.get_node(cube_coordinate(c)), 10)
        self.assertEqual(layout.get_tile(6), cube_coordinate(a1))
        self.assertEqual(layout.get_tile(7), cube_coordinate(b1))
        self.assertEqual(layout.get_tile(8), cube_coordinate(b2))
        self.assertEqual(layout.get_tile(9), cube_coordinate(a2))
        self.assertEqual(layout.get_tile(10), cube_coordinate(c))
        self.assertEqual(layout.make_signal(6), a1)
        self.assertEqual(layout.make_signal(7), b1)
        self.assertEqual(layout.make_signal(8), b2)
        self.assertEqual(layout.make_signal(9), a2)
        self.assertEqual(layout.make_signal(10), c)

        # wires
        wires = layout.wires()
        self.assertEqual(len(wires), 9)
        self.assertIn(cube_coordinate(x1), wires)
        self.assertIn(cube_coordinate(x2), wires)
        self.assertIn(cube_coordinate(x3), wires)
        self.assertIn(cube_coordinate(x4), wires)
        self.assertIn(cube_coordinate(b1), wires)
        self.assertIn(cube_coordinate(b2), wires)
        self.assertIn(cube_coordinate(c), wires)
        self.assertIn(cube_coordinate(f1), wires)
        self.assertIn(cube_coordinate(f2), wires)

        # incoming data flow
        inx1 = layout.fanins(cube_coordinate(x1))
        self.assertEqual(len(inx1), 0)

        inf1 = layout.fanins(cube_coordinate(f1))
        self.assertEqual(len(inf1), 1)
        self.assertIn(cube_coordinate(c), inf1)

        ina2 = layout.fanins(cube_coordinate(a2))
        self.assertEqual(len(ina2), 2)
        self.assertIn(cube_coordinate(b1), ina2)
        self.assertIn(cube_coordinate(b2), ina2)

        # outgoing data flow
        outx1 = layout.fanouts(cube_coordinate(x1))
        self.assertEqual(len(outx1), 1)
        self.assertIn(cube_coordinate(a1), outx1)

        outf1 = layout.fanouts(cube_coordinate(f1))
        self.assertEqual(len(outf1), 0)

        outa2 = layout.fanouts(cube_coordinate(a2))
        self.assertEqual(len(outa2), 1)
        self.assertIn(cube_coordinate(f2), outa2)

        cp, tp = critical_path_length_and_throughput(layout)
        self.assertEqual(cp, 4)
        self.assertEqual(tp, 1)

        drv_params = gate_level_drv_params()
        self.assertEqual(gate_level_drvs(layout, drv_params, False), (0, 0))

    def test_gate_level_layout_gate_types(self):
        for layout in [
            cartesian_gate_layout((2, 8, 0), "2DDWave", "Layout"),
            cartesian_gate_layout((2, 8, 0), "2DDWave", "Layout", "cube"),
            shifted_cartesian_gate_layout((2, 8, 0), "2DDWave", "Layout"),
            hexagonal_gate_layout((2, 8, 0), "2DDWave", "Layout"),
        ]:
            self.assertTrue(layout.is_empty())

            # layout creation
            # pis
            x1 = layout.create_pi("x1", (0, 0))
            x2 = layout.create_pi("x2", (0, 1))
            x3 = layout.create_pi("x3", (0, 2))
            x4 = layout.create_pi("x4", (0, 3))
            x5 = layout.create_pi("x5", (0, 4))
            x6 = layout.create_pi("x6", (0, 5))
            x7 = layout.create_pi("x7", (0, 6))

            # gates
            inv = layout.create_not(x1, (1, 0))
            and_gate = layout.create_and(x2, inv, (1, 1))
            nand_gate = layout.create_nand(x3, and_gate, (1, 2))
            or_gate = layout.create_or(x4, nand_gate, (1, 3))
            nor_gate = layout.create_nor(x5, or_gate, (1, 4))
            xor_gate = layout.create_xor(x6, nor_gate, (1, 5))
            xnor_gate = layout.create_xnor(x7, xor_gate, (1, 6))
            fanout = layout.create_buf(xnor_gate, (1, 7))
            buf = layout.create_buf(fanout, (2, 7))

            # pos
            layout.create_po(fanout, "f1", (1, 8))
            layout.create_po(buf, "f2", (2, 8))

            # check gate type
            # pis
            self.assertTrue(layout.is_pi(layout.get_node((0, 0))))
            self.assertTrue(layout.is_pi(layout.get_node((0, 1))))
            self.assertTrue(layout.is_pi(layout.get_node((0, 2))))
            self.assertTrue(layout.is_pi(layout.get_node((0, 3))))
            self.assertTrue(layout.is_pi(layout.get_node((0, 4))))
            self.assertTrue(layout.is_pi(layout.get_node((0, 5))))
            self.assertTrue(layout.is_pi(layout.get_node((0, 6))))

            # gates
            self.assertTrue(layout.is_inv(layout.get_node((1, 0))))
            self.assertTrue(layout.is_and(layout.get_node((1, 1))))
            self.assertTrue(layout.is_nand(layout.get_node((1, 2))))
            self.assertTrue(layout.is_or(layout.get_node((1, 3))))
            self.assertTrue(layout.is_nor(layout.get_node((1, 4))))
            self.assertTrue(layout.is_xor(layout.get_node((1, 5))))
            self.assertTrue(layout.is_xnor(layout.get_node((1, 6))))
            self.assertTrue(layout.is_fanout(layout.get_node((1, 7))))
            self.assertTrue(layout.is_wire(layout.get_node((2, 7))))

            # pos
            self.assertTrue(layout.is_po(layout.get_node((1, 8))))
            self.assertTrue(layout.is_po(layout.get_node((2, 8))))

        for layout in [
            cartesian_gate_layout((2, 2, 0), "RES", "Layout"),
            cartesian_gate_layout((2, 2, 0), "RES", "Layout", "cube"),
            shifted_cartesian_gate_layout((2, 2, 0), "RES", "Layout"),
            hexagonal_gate_layout((2, 2, 0), "RES", "Layout"),
        ]:
            self.assertTrue(layout.is_empty())

            # pis
            x1 = layout.create_pi("x1", (0, 1))
            x2 = layout.create_pi("x2", (1, 0))
            x3 = layout.create_pi("x3", (2, 1))

            # maj
            maj = layout.create_maj(x1, x2, x3, (1, 1))

            # po
            layout.create_po(maj, "f1", (1, 2))

            # check gate type
            # pis
            self.assertTrue(layout.is_pi(layout.get_node((0, 1))))
            self.assertTrue(layout.is_pi(layout.get_node((1, 0))))
            self.assertTrue(layout.is_pi(layout.get_node((2, 1))))

            # maj
            self.assertTrue(layout.is_maj(layout.get_node((1, 1))))

            # po
            self.assertTrue(layout.is_po(layout.get_node((1, 2))))


if __name__ == "__main__":
    unittest.main()
