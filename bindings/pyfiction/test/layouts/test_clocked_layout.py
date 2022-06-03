from fiction import pyfiction
import unittest


class TestClockedLayout(unittest.TestCase):

    def test_clocked_layout_inheritance(self):
        layout = pyfiction.clocked_cartesian_layout((2, 2, 0), "2DDWave")

        for t in layout.coordinates():
            self.assertTrue(t <= (9, 9, 1))
            self.assertTrue(layout.is_within_bounds(t))

        for t in layout.ground_coordinates():
            self.assertTrue(t.z == 0)
            self.assertTrue(t <= (9, 9, 0))
            self.assertTrue(layout.is_within_bounds(t))

        for t in layout.adjacent_coordinates((2, 2)):
            self.assertIn(t, [(1, 2), (2, 1), (3, 2), (2, 3)])

    def test_clock_zone_iteration(self):
        layout = pyfiction.clocked_cartesian_layout((2, 2, 0), "2DDWave")

        self.assertEqual(layout.incoming_clocked_zones((0, 0)), set())
        self.assertEqual(layout.outgoing_clocked_zones((2, 2)), set())

        for icz in layout.incoming_clocked_zones((1, 1)):
            self.assertIn(icz, [pyfiction.coordinate(1, 0), pyfiction.coordinate(0, 1)])

        for icz in layout.outgoing_clocked_zones((1, 1)):
            self.assertIn(icz, [pyfiction.coordinate(1, 2), pyfiction.coordinate(2, 1)])

    def test_fetch_clocking_scheme(self):
        pyfiction.clocked_cartesian_layout((1, 1), "USE")
        pyfiction.clocked_cartesian_layout((2, 2), "2DDWave")
        pyfiction.clocked_cartesian_layout((3, 3), "RES")
        pyfiction.clocked_cartesian_layout((4, 4), "ESP")
        pyfiction.clocked_cartesian_layout((5, 5), "BANCS")

        with self.assertRaises(RuntimeError):
            pyfiction.clocked_cartesian_layout((1, 2), "3DDWave")
        with self.assertRaises(RuntimeError):
            pyfiction.clocked_cartesian_layout((1, 2), "SUE")
        with self.assertRaises(RuntimeError):
            pyfiction.clocked_cartesian_layout((1, 2), "PES")


if __name__ == '__main__':
    unittest.main()
