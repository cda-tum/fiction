import unittest

import pytest

from mnt.pyfiction import sidb_simulation_parameters


class TestSiDBSimulationParameters(unittest.TestCase):
    def test_initialization(self):
        params = sidb_simulation_parameters()

        # Check if it is initialized correctly.
        assert params.epsilon_r == pytest.approx(5.6)
        assert params.lambda_tf == 5
        assert params.mu_minus == pytest.approx(-0.32)
        assert params.base == 3

    def test_custom_initialization(self):
        params = sidb_simulation_parameters(2, -0.4, 7.1, 10.0)

        # Check if it is initialized correctly.
        assert params.epsilon_r == pytest.approx(7.1)
        assert params.lambda_tf == pytest.approx(10.0)
        assert params.mu_minus == pytest.approx(-0.4)
        assert params.base == 2


if __name__ == "__main__":
    unittest.main()
