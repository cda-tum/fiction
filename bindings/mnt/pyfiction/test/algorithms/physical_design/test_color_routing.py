import unittest

from mnt.pyfiction import (cartesian_gate_layout, hexagonal_gate_layout, offset_coordinate, color_routing,
                           color_routing_params)


class TestColorRouting(unittest.TestCase):
    def test_routing(self):
        for lyt in [cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
                    hexagonal_gate_layout((4, 4), "2DDWave", "Layout")]:
            x1 = lyt.create_pi("x1", offset_coordinate(0, 0))
            x2 = lyt.create_pi("x2", offset_coordinate(0, 1))

            a = lyt.create_and(x1, x2, offset_coordinate(2, 2))

            lyt.create_po(a, "f1", offset_coordinate(4, 4))

            success = color_routing(lyt, [((0, 0), (2, 2)), ((0, 1), (2, 2)), ((2, 2), (4, 4))])

            self.assertTrue(success)

    def test_crossings(self):
        lyt = cartesian_gate_layout((4, 2, 1), "2DDWave", "Layout")

        x1 = lyt.create_pi("x1", (0, 1))
        x2 = lyt.create_pi("x2", (3, 2))
        x3 = lyt.create_pi("x3", (2, 0))

        buf1 = lyt.create_buf(x3, (2, 1))
        lyt.create_buf(buf1, (2, 2))

        lyt.create_and(x1, x2, (4, 2))
        lyt.move_node(lyt.get_node((4, 2)), (4, 2))

        params = color_routing_params()
        params.crossings = True
        params.path_limit = 1

        success = color_routing(lyt, [((0, 1), (4, 2)), ((3, 2), (4, 2))], params=params)

        self.assertTrue(success)


if __name__ == '__main__':
    unittest.main()
