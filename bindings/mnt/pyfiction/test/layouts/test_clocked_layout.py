import unittest

from mnt.pyfiction import (
    clocked_cartesian_layout,
    clocked_hexagonal_layout,
    clocked_shifted_cartesian_layout,
)


class TestClockedLayout(unittest.TestCase):
    def test_clocked_layout_inheritance(self):
        for layout in [
            clocked_cartesian_layout((2, 2, 0), "2DDWave"),
            clocked_cartesian_layout((2, 2, 0), "2DDWave", coordinate_type="cube"),
            clocked_shifted_cartesian_layout((2, 2, 0), "2DDWave"),
            clocked_shifted_cartesian_layout((2, 2, 0), "2DDWave", coordinate_type="cube"),
            clocked_hexagonal_layout((2, 2, 0), "2DDWave"),
        ]:
            for t in layout.coordinates():
                self.assertTrue(t <= (9, 9, 1))
                self.assertTrue(layout.is_within_bounds(t))

            for t in layout.ground_coordinates():
                self.assertTrue(t.z == 0)
                self.assertTrue(t <= (9, 9, 0))
                self.assertTrue(layout.is_within_bounds(t))

            for t in layout.adjacent_coordinates((2, 2)):
                self.assertIn(t, [(1, 1), (1, 2), (2, 1), (3, 2), (2, 3)])

    def test_clock_zone_iteration(self):
        for layout in [
            clocked_cartesian_layout((2, 2, 0), "2DDWave"),
            clocked_shifted_cartesian_layout((2, 2, 0), "2DDWave"),
            clocked_hexagonal_layout((2, 2, 0), "2DDWave"),
        ]:
            self.assertEqual(layout.incoming_clocked_zones((0, 0)), [])
            self.assertEqual(layout.outgoing_clocked_zones((2, 2)), [])

            for icz in layout.incoming_clocked_zones((1, 1)):
                self.assertIn(icz, [layout.coord(1, 0), layout.coord(0, 1)])

            for icz in layout.outgoing_clocked_zones((1, 1)):
                self.assertIn(icz, [layout.coord(1, 2), layout.coord(2, 1)])

        for layout in [
            clocked_cartesian_layout((2, 2, 0), "2DDWave", coordinate_type="cube"),
            clocked_shifted_cartesian_layout((2, 2, 0), "2DDWave", coordinate_type="cube"),
        ]:
            for icz in layout.incoming_clocked_zones((0, 0)):
                self.assertIn(icz, [layout.coord(-1, 0), layout.coord(0, -1)])
            self.assertEqual(layout.outgoing_clocked_zones((2, 2)), [])

            for icz in layout.incoming_clocked_zones((1, 1)):
                self.assertIn(icz, [layout.coord(1, 0), layout.coord(0, 1)])

            for icz in layout.outgoing_clocked_zones((1, 1)):
                self.assertIn(icz, [layout.coord(1, 2), layout.coord(2, 1)])

    def test_fetch_clocking_scheme(self):
        for layout in [
            clocked_cartesian_layout,
            clocked_shifted_cartesian_layout,
            clocked_hexagonal_layout,
        ]:
            layout((1, 1), "USE")
            layout((2, 2), "2DDWave")
            layout((3, 3), "RES")
            layout((4, 4), "ESR")
            layout((5, 5), "BANCS")

            with self.assertRaises(RuntimeError):
                layout((1, 2), "3DDWave")
            with self.assertRaises(RuntimeError):
                layout((1, 2), "SUE")
            with self.assertRaises(RuntimeError):
                layout((1, 2), "PES")


if __name__ == "__main__":
    unittest.main()
