from mnt.pyfiction import *
import unittest
import os


class TestSiDBSimulationParameters(unittest.TestCase):

    def test_initialization(self):
        params = sidb_simulation_parameters()

        # Check if it is initialized correctly.
        self.assertEqual(params.lat_a, 3.84)
        self.assertEqual(params.lat_b, 7.68)
        self.assertEqual(params.lat_c, 2.25)
        self.assertEqual(params.epsilon_r, 5.6)
        self.assertEqual(params.lambda_tf, 5)
        self.assertEqual(params.mu_minus, -0.32)
        self.assertEqual(params.base, 3)

    def test_custom_initialization(self):
        params = sidb_simulation_parameters(
            2, -0.4, 7.1, 10.0, 6.84, 7.68, 2.8)

        # Check if it is initialized correctly.
        self.assertEqual(params.lat_a, 6.84)
        self.assertEqual(params.lat_b, 7.68)
        self.assertEqual(params.lat_c, 2.8)
        self.assertEqual(params.epsilon_r, 7.1)
        self.assertEqual(params.lambda_tf, 10.0)
        self.assertEqual(params.mu_minus, -0.4)
        self.assertEqual(params.base, 2)


if __name__ == '__main__':
    unittest.main()
