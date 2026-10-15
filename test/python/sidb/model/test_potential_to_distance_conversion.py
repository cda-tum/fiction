# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import math

import pytest

from mnt.pyfiction.sidb.model import potential_to_distance_conversion, sidb_simulation_parameters


def test_potential_distance_limits() -> None:
    """Check the inverse limits and decimal rounding through the Python binding."""
    params = sidb_simulation_parameters()
    assert potential_to_distance_conversion(0.0, params, 2) == math.inf
    assert potential_to_distance_conversion(math.inf, params, 2) == pytest.approx(0.01)
    params.epsilon_r = 2.0
    params.lambda_tf = 1.0
    assert potential_to_distance_conversion(0.01, params, 3) == pytest.approx(3.135)
    params.lambda_tf = math.inf
    assert potential_to_distance_conversion(0.1, params, 2) == pytest.approx(7.2)


@pytest.mark.parametrize("potential", [-1.0, -math.inf, math.nan])
def test_potential_distance_rejects_invalid_potential(potential: float) -> None:
    """Translate invalid C++ potential arguments to Python ValueError."""
    with pytest.raises(ValueError, match="potential-to-distance conversion"):
        potential_to_distance_conversion(potential, sidb_simulation_parameters(), 2)


def test_potential_distance_rejects_invalid_parameters() -> None:
    """Reject unsupported precision and invalid physical parameters."""
    params = sidb_simulation_parameters()
    with pytest.raises(ValueError, match="potential-to-distance conversion"):
        potential_to_distance_conversion(0.1, params, 16)
    params.epsilon_r = 0.0
    with pytest.raises(ValueError, match="potential-to-distance conversion"):
        potential_to_distance_conversion(0.1, params, 2)


def test_potential_distance_documents_python_exception() -> None:
    """Document the exception callers can catch in Python."""
    documentation = potential_to_distance_conversion.__doc__
    assert documentation is not None
    assert "ValueError:" in documentation
    assert "std::invalid_argument" not in documentation
