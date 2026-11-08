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


@pytest.mark.parametrize(
    ("potential", "epsilon_r", "lambda_tf", "precision", "expected"),
    [
        (5.0, 5.6, 5.0, 1, 0.1),
        (0.01, 2.0, 1.0, 1, 3.2),
        (0.01, 2.0, 1.0, 2, 3.14),  # ruff: ignore[math-constant] -- exact decimal rounding result
        (0.03, 5.6, 5.0, 0, 4.0),
        (math.inf, 5.6, 5.0, 3, 0.001),
        (1e-300, 1e300, 1e300, 2, 1.44),
        (0.1, 5.6, math.inf, 2, 2.58),
    ],
)
def test_conversion_precision_and_extremes(
    potential: float, epsilon_r: float, lambda_tf: float, precision: int, expected: float
) -> None:
    """Conversion rounds upward across precisions, extreme material scales, and the unscreened limit."""
    assert potential_to_distance_conversion(
        potential, precision, epsilon_r=epsilon_r, lambda_tf=lambda_tf
    ) == pytest.approx(expected, abs=1e-14, rel=0)


@pytest.mark.parametrize(
    "distance",
    [0.01, 0.07, 0.1, 1.0, 2.0, 3.14, 10.0],  # ruff: ignore[math-constant] -- exact decimal rounding boundary
)
def test_decimal_boundaries_round_upward(distance: float) -> None:
    """A decimal boundary remains fixed while a slightly smaller potential advances one distance unit."""
    potential = 8.987552e9 * 1.6021766e-19 / (5.6 * distance * 1e-9) * math.exp(-distance / 5.0)
    assert potential_to_distance_conversion(potential, 2) == pytest.approx(distance, abs=1e-14, rel=0)
    assert potential_to_distance_conversion(potential / 1.001, 2) == pytest.approx(distance + 0.01, abs=1e-14, rel=0)


def test_conversion_avoids_underflow_and_preserves_precision() -> None:
    """Log-scaled inversion retains subnormal potentials and respects the requested rounding direction."""
    target_distance = 743.305
    potential = math.exp(
        math.log(8.987552e9 * 1.6021766e-19 * 1e9) - math.log(1e-290) - math.log(target_distance) - target_distance
    )
    assert potential_to_distance_conversion(potential, 2, epsilon_r=1e-290, lambda_tf=1.0) == pytest.approx(
        743.31, abs=1e-10, rel=0
    )
    for potential, precision in [(0.5, 15), (0.03, 15), (1.3772094688939445e-8, 13)]:
        distance = potential_to_distance_conversion(potential, precision)
        recovered = 8.987552e9 * 1.6021766e-19 / (5.6 * distance * 1e-9) * math.exp(-distance / 5.0)
        assert recovered == pytest.approx(potential, rel=1e-12)
        assert recovered <= potential
    assert math.isfinite(potential_to_distance_conversion(5e-324, 2))
    assert potential_to_distance_conversion(5e-324, 2, epsilon_r=5e-324, lambda_tf=1.7976931348623157e308) == math.inf
