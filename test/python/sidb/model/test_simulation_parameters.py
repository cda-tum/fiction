# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction.sidb.model import sidb_simulation_parameters


def test_initialization():
    params = sidb_simulation_parameters()

    # Check if it is initialized correctly.
    assert params.epsilon_r == 5.6
    assert params.lambda_tf == 5
    assert params.mu_minus == -0.32
    assert params.base == 3


def test_custom_initialization():
    params = sidb_simulation_parameters(
        base_number=2, mu_minus=-0.4, relative_permittivity=7.1, screening_distance=10.0
    )

    # Check if it is initialized correctly.
    assert params.epsilon_r == 7.1
    assert params.lambda_tf == 10.0
    assert params.mu_minus == -0.4
    assert params.base == 2


@pytest.mark.parametrize("base", [0, 1, 4, 255])
def test_base_assignment_preserves_valid_model(base: int) -> None:
    """Invalid base assignments raise before changing the physical model."""
    params = sidb_simulation_parameters()
    with pytest.raises(ValueError, match="base must be 2 or 3"):
        params.base = base
    assert params.base == 3
    params.base = 2
    assert params.base == 2


@pytest.mark.parametrize("name", ["epsilon_r", "lambda_tf"])
@pytest.mark.parametrize("value", [0.0, -1.0, float("nan")])
def test_screening_assignment_preserves_valid_model(name: str, value: float) -> None:
    """Physical-model setters preserve the constructor's screening constraints."""
    params = sidb_simulation_parameters()
    previous = getattr(params, name)
    with pytest.raises(ValueError, match=name):
        setattr(params, name, value)
    assert getattr(params, name) == previous


def test_unscreened_model_assignment() -> None:
    """Positive infinite screening distance represents the unscreened model."""
    params = sidb_simulation_parameters()
    params.lambda_tf = float("inf")
    assert params.lambda_tf == float("inf")
    with pytest.raises(ValueError, match="epsilon_r"):
        params.epsilon_r = float("inf")


@pytest.mark.parametrize("value", [float("nan"), float("inf"), -float("inf")])
def test_transition_energy_preserves_finite_model(value: float) -> None:
    """The model rejects non-finite transition energies during construction and assignment."""
    with pytest.raises(ValueError, match="mu_minus"):
        sidb_simulation_parameters(mu_minus=value)
    params = sidb_simulation_parameters()
    with pytest.raises(ValueError, match="mu_minus"):
        params.mu_minus = value
    assert params.mu_minus == -0.32
