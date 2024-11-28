from mnt.pyfiction import (sidb_simulation_parameters)
import unittest


class TestSiDBSimulationParameters(unittest.TestCase):

    def test_initialization(self):
        params = sidb_simulation_parameters()

        # Check if it is initialized correctly.
        self.assertEqual(params.epsilon_r, 5.6)
        self.assertEqual(params.lambda_tf, 5)
        self.assertEqual(params.mu_minus, -0.32)
        self.assertEqual(params.base, 3)

    def test_custom_initialization(self):
        params = sidb_simulation_parameters(
            2, -0.4, 7.1, 10.0)

        # Check if it is initialized correctly.
        self.assertEqual(params.epsilon_r, 7.1)
        self.assertEqual(params.lambda_tf, 10.0)
        self.assertEqual(params.mu_minus, -0.4)
        self.assertEqual(params.base, 2)


if __name__ == '__main__':
    unittest.main()
