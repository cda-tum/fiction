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

from mnt.pyfiction.sidb.model import potential_to_distance_conversion


def test_potential_distance_limits() -> None:
    """Check the inverse limits and decimal rounding through the Python binding."""
    assert potential_to_distance_conversion(0.0, 2) == math.inf
    assert potential_to_distance_conversion(math.inf, 2) == pytest.approx(0.01)
    assert potential_to_distance_conversion(0.01, 3, epsilon_r=2.0, lambda_tf=1.0) == pytest.approx(3.135)
    assert potential_to_distance_conversion(0.1, 2, epsilon_r=2.0, lambda_tf=math.inf) == pytest.approx(7.2)


@pytest.mark.parametrize("potential", [-1.0, -math.inf, math.nan])
def test_potential_distance_rejects_invalid_potential(potential: float) -> None:
    """Translate invalid C++ potential arguments to Python ValueError."""
    with pytest.raises(ValueError, match="potential-to-distance conversion"):
        potential_to_distance_conversion(potential, 2)


def test_potential_distance_rejects_invalid_parameters() -> None:
    """Reject unsupported precision and invalid physical parameters."""
    with pytest.raises(ValueError, match="potential-to-distance conversion"):
        potential_to_distance_conversion(0.1, 16)
    with pytest.raises(ValueError, match="epsilon_r"):
        potential_to_distance_conversion(0.1, 2, epsilon_r=0.0)


def test_potential_distance_documents_python_exception() -> None:
    """Document the exception callers can catch in Python."""
    documentation = potential_to_distance_conversion.__doc__
    assert documentation is not None
    assert "ValueError:" in documentation
    assert "std::invalid_argument" not in documentation
