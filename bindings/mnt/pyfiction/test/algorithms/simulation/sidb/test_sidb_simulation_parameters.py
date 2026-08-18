from mnt.pyfiction import sidb_simulation_parameters


def test_initialization():
    params = sidb_simulation_parameters()

    # Check if it is initialized correctly.
    assert params.epsilon_r == 5.6
    assert params.lambda_tf == 5
    assert params.mu_minus == -0.32
    assert params.base == 3


def test_custom_initialization():
    params = sidb_simulation_parameters(2, -0.4, 7.1, 10.0)

    # Check if it is initialized correctly.
    assert params.epsilon_r == 7.1
    assert params.lambda_tf == 10.0
    assert params.mu_minus == -0.4
    assert params.base == 2
