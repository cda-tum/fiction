import unittest
from mnt import pyfiction


class TestDesignRuleViolations(unittest.TestCase):
    def test_drvs(self):
        # Create empty layout
        layout = pyfiction.cartesian_gate_layout((2, 5, 0), "2DDWave")

        # Create 2:1 MUX

        # Inputs
        layout.create_pi("x1", (0, 3))
        layout.create_pi("x2", (0, 0))
        layout.create_pi("x3", (2, 0))

        # Wires
        layout.create_buf(layout.make_signal(layout.get_node((0, 0, 0))), (0, 1))
        layout.create_buf(layout.make_signal(layout.get_node((0, 1, 0))), (1, 1))

        # NOT
        layout.create_not(layout.make_signal(layout.get_node((0, 1, 0))), (0, 2))

        # Wires
        layout.create_buf(layout.make_signal(layout.get_node((0, 2, 0))), (1, 2))
        # AND
        layout.create_and(
            layout.make_signal(layout.get_node((0, 3, 0))), layout.make_signal(layout.get_node((1, 2, 0))), (1, 3)
        )

        # AND
        layout.create_and(
            layout.make_signal(layout.get_node((1, 1, 0))), layout.make_signal(layout.get_node((2, 0, 0))), (2, 1)
        )

        # Wires
        layout.create_buf(layout.make_signal(layout.get_node((2, 1, 0))), (2, 2))

        # OR
        layout.create_or(
            layout.make_signal(layout.get_node((1, 3, 0))), layout.make_signal(layout.get_node((2, 2, 0))), (2, 3)
        )

        # Outputs
        layout.create_po(layout.make_signal(layout.get_node((2, 3, 0))), "f1", (2, 4))

        layout.move_node(layout.get_node((2, 4)), (2, 5))

        pyfiction.color_routing(layout, [((2, 3), (2, 5))])

        warnings, drvs, output = pyfiction.gate_level_drvs(layout)

        self.assertEqual(0, warnings)
        self.assertEqual(0, drvs)

        report = """
        [i] Topology:
        [i] [✓] all nodes are properly placed
        [i] [✓] all placed nodes are alive
        [i] [✓] all tiles are adjacently connected
        [i] [✓] all occupied tiles are properly connected
        [i] [✓] all wire crossings cross over other wires only
        
        [i] Clocking:
        [i] [✓] all connected tiles are properly clocked
        
        [i] I/O ports:
        [i] [✓] all I/O are properly specified
        [i] [✓] all I/O ports are assigned to a non-empty tile
        [i] [✓] all I/O ports are realized by designated pins
        [i] [✓] all I/O ports are located at the layout's borders
        
        [i] DRVs: 0, Warnings: 0
        """

        self.assertEqual(output, report)


if __name__ == "__main__":
    unittest.main()
