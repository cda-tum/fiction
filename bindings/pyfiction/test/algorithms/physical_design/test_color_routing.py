import unittest
from fiction.pyfiction import *


class ColorRouting(unittest.TestCase):
    def test_routing(self):
        for lyt in [cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
                    hexagonal_gate_layout((4, 4), "2DDWave", "Layout")]:
            x1 = lyt.create_pi("x1", coordinate(0, 0))
            x2 = lyt.create_pi("x2", coordinate(0, 1))

            a = lyt.create_and(x1, x2, coordinate(2, 2))

            lyt.create_po(a, "f1", coordinate(4, 4))

            success = color_routing(lyt, [((0, 0), (2, 2)), ((0, 1), (2, 2)), ((2, 2), (4, 4))])

            self.assertTrue(success)


if __name__ == '__main__':
    unittest.main()
