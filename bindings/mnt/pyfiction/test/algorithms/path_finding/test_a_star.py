import unittest

from mnt.pyfiction import (
    cartesian_layout,
    shifted_cartesian_layout,
    hexagonal_layout,
    a_star,
    offset_coordinate,
    clocked_cartesian_layout,
    cartesian_gate_layout,
    shifted_cartesian_gate_layout,
    clocked_shifted_cartesian_layout,
    hexagonal_gate_layout,
    clocked_hexagonal_layout,
    cartesian_obstruction_layout,
    shifted_cartesian_obstruction_layout,
    hexagonal_obstruction_layout,
    a_star_params,
    a_star_distance,
)


class TestAStar(unittest.TestCase):
    def test_non_clocked_path_finding(self):
        for lyt in [cartesian_layout((4, 4)), shifted_cartesian_layout((4, 4)), hexagonal_layout((4, 4))]:
            self.assertListEqual(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)), [(0, 0)])

    def test_clocked_path_finding(self):
        for lyt in [
            clocked_cartesian_layout((4, 4), "2DDWave"),
            cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
            clocked_shifted_cartesian_layout((4, 4), "2DDWave"),
            shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
            clocked_hexagonal_layout((4, 4), "2DDWave"),
            hexagonal_gate_layout((4, 4), "2DDWave", "Layout"),
        ]:
            self.assertListEqual(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)), [(0, 0)])
            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1))), 3)
            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2))), 5)
            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3))), 7)
            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4))), 9)
            self.assertEqual(len(a_star(lyt, offset_coordinate(1, 1), offset_coordinate(0, 0))), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(2, 2), offset_coordinate(1, 1))), 0)

    def test_path_finding_with_obstructions(self):
        for lyt in [
            cartesian_obstruction_layout(cartesian_gate_layout((4, 4), "2DDWave", "Layout")),
            shifted_cartesian_obstruction_layout(shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout")),
            hexagonal_obstruction_layout(hexagonal_gate_layout((4, 4), "2DDWave", "Layout")),
        ]:
            # block a vertical line of offset_coordinates
            lyt.obstruct_coordinate(offset_coordinate(1, 0))
            lyt.obstruct_coordinate(offset_coordinate(1, 1))
            lyt.obstruct_coordinate(offset_coordinate(1, 2))
            lyt.obstruct_coordinate(offset_coordinate(1, 3))
            lyt.obstruct_coordinate(offset_coordinate(1, 4))

            self.assertTrue(lyt.is_obstructed_coordinate(offset_coordinate(1, 0)))
            self.assertTrue(lyt.is_obstructed_coordinate(offset_coordinate(1, 1)))
            self.assertTrue(lyt.is_obstructed_coordinate(offset_coordinate(1, 2)))
            self.assertTrue(lyt.is_obstructed_coordinate(offset_coordinate(1, 3)))
            self.assertTrue(lyt.is_obstructed_coordinate(offset_coordinate(1, 4)))

            self.assertListEqual(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)), [(0, 0)])
            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1))), 3)
            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2))), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3))), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4))), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(1, 1), offset_coordinate(0, 0))), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(2, 2), offset_coordinate(1, 1))), 0)

    def test_path_finding_with_obstructions_and_crossings(self):
        for lyt in [
            cartesian_obstruction_layout(cartesian_gate_layout((2, 1, 1), "2DDWave", "Layout")),
            shifted_cartesian_obstruction_layout(shifted_cartesian_gate_layout((2, 1, 1), "2DDWave", "Layout")),
            hexagonal_obstruction_layout(hexagonal_gate_layout((2, 1, 1), "2DDWave", "Layout")),
        ]:
            x1 = lyt.create_pi("x1", (0, 0))
            lyt.obstruct_coordinate((0, 0, 0))
            lyt.obstruct_coordinate((0, 0, 1))

            x2 = lyt.create_pi("x2", (0, 1))
            lyt.obstruct_coordinate((0, 1, 0))
            lyt.obstruct_coordinate((0, 1, 1))

            b = lyt.create_buf(x1, (1, 0))
            lyt.obstruct_coordinate((1, 0, 0))

            lyt.create_and(x2, b, (1, 1))
            lyt.obstruct_coordinate((1, 1, 0))
            lyt.obstruct_coordinate((1, 1, 1))

            params = a_star_params()
            params.crossings = True

            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0), params)), 1)
            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1), params)), 2)
            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(2, 0), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 0), offset_coordinate(2, 1), params)), 0)

            self.assertEqual(len(a_star(lyt, offset_coordinate(1, 0), offset_coordinate(0, 0), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(1, 0), offset_coordinate(1, 0), params)), 1)
            self.assertEqual(len(a_star(lyt, offset_coordinate(1, 0), offset_coordinate(0, 1), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(1, 0), offset_coordinate(1, 1), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(1, 0), offset_coordinate(2, 0), params)), 2)
            self.assertEqual(len(a_star(lyt, offset_coordinate(1, 0), offset_coordinate(2, 1), params)), 3)

            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 1), offset_coordinate(0, 0), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 1), offset_coordinate(1, 0), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 1), offset_coordinate(0, 1), params)), 1)
            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 1), offset_coordinate(1, 1), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 1), offset_coordinate(2, 0), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(0, 1), offset_coordinate(2, 1), params)), 0)

            self.assertEqual(len(a_star(lyt, offset_coordinate(1, 1), offset_coordinate(0, 0), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(1, 1), offset_coordinate(1, 0), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(1, 1), offset_coordinate(0, 1), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(1, 1), offset_coordinate(1, 1), params)), 1)
            self.assertEqual(len(a_star(lyt, offset_coordinate(1, 1), offset_coordinate(2, 0), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(1, 1), offset_coordinate(2, 1), params)), 2)

            self.assertEqual(len(a_star(lyt, offset_coordinate(2, 0), offset_coordinate(0, 0), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(2, 0), offset_coordinate(1, 0), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(2, 0), offset_coordinate(0, 1), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(2, 0), offset_coordinate(1, 1), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(2, 0), offset_coordinate(2, 0), params)), 1)
            self.assertEqual(len(a_star(lyt, offset_coordinate(2, 0), offset_coordinate(2, 1), params)), 2)

            self.assertEqual(len(a_star(lyt, offset_coordinate(2, 1), offset_coordinate(0, 0), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(2, 1), offset_coordinate(1, 0), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(2, 1), offset_coordinate(0, 1), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(2, 1), offset_coordinate(1, 1), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(2, 1), offset_coordinate(2, 0), params)), 0)
            self.assertEqual(len(a_star(lyt, offset_coordinate(2, 1), offset_coordinate(2, 1), params)), 1)

    def test_distance(self):
        for lyt in [
            clocked_cartesian_layout((4, 4), "2DDWave"),
            cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
            clocked_shifted_cartesian_layout((4, 4), "2DDWave"),
            shifted_cartesian_gate_layout((4, 4), "2DDWave", "Layout"),
            clocked_hexagonal_layout((4, 4), "2DDWave"),
            hexagonal_gate_layout((4, 4), "2DDWave", "Layout"),
        ]:
            self.assertEqual(a_star_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 0)), 0)
            self.assertEqual(a_star_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 0)), 1)
            self.assertEqual(a_star_distance(lyt, offset_coordinate(0, 0), offset_coordinate(0, 1)), 1)
            self.assertEqual(a_star_distance(lyt, offset_coordinate(0, 0), offset_coordinate(1, 1)), 2)
            self.assertEqual(a_star_distance(lyt, offset_coordinate(0, 0), offset_coordinate(2, 2)), 4)
            self.assertEqual(a_star_distance(lyt, offset_coordinate(0, 0), offset_coordinate(3, 3)), 6)
            self.assertEqual(a_star_distance(lyt, offset_coordinate(0, 0), offset_coordinate(4, 4)), 8)
            self.assertEqual(a_star_distance(lyt, offset_coordinate(1, 1), offset_coordinate(0, 0)), float("inf"))
            self.assertEqual(a_star_distance(lyt, offset_coordinate(2, 2), offset_coordinate(1, 1)), float("inf"))


if __name__ == "__main__":
    unittest.main()
