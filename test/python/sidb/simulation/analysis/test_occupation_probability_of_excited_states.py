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

from mnt.pyfiction.sidb.simulation.analysis import (
    energy_distribution,
    energy_state,
    occupation_probability_gate_based,
    occupation_probability_non_gate_based,
    state_type,
)


@pytest.mark.parametrize(
    ("states", "expected"),
    [
        ([], 0.0),
        ([(1.0, state_type.ACCEPTED)], 0.0),
        ([(1.0, state_type.REJECTED)], 1.0),
        ([(0.1, state_type.ACCEPTED), (0.1, state_type.REJECTED)], 0.5),
    ],
)
def test_gate_probability_degeneracy(states: list, expected: float) -> None:
    """Degenerate ground states keep their multiplicity even at low temperature."""
    for temperature in (0.01, 10.0, 1000.0):
        assert occupation_probability_gate_based(states, temperature) == expected


@pytest.mark.parametrize("reverse", [False, True])
def test_boltzmann_probability(*, reverse: bool) -> None:
    """Both APIs sum independent Boltzmann weights, regardless of input order."""
    states = [
        (0.1, state_type.ACCEPTED),
        (0.1, state_type.REJECTED),
        (0.2, state_type.ACCEPTED),
        (0.2, state_type.ACCEPTED),
    ]
    if reverse:
        states.reverse()
    distribution = energy_distribution()
    distribution.add_energy_state(energy_state(0.1, 2))
    distribution.add_energy_state(energy_state(0.2, 2))
    for temperature in (0.001, 300.0, 1e11):
        weight = math.exp(-0.1 * 1.6021766e-19 / (1.38064852e-23 * temperature))
        assert occupation_probability_gate_based(states, temperature) == pytest.approx(1 / (2 + 2 * weight))
        assert occupation_probability_non_gate_based(distribution, temperature) == pytest.approx(weight / (1 + weight))


@pytest.mark.parametrize("degeneracy", [1, 2])
def test_no_excited_states(degeneracy: int) -> None:
    """A single energy level has zero excited-state occupation."""
    distribution = energy_distribution()
    distribution.add_energy_state(energy_state(0.1, degeneracy))
    for temperature in (0.01, 10.0, 1000.0):
        assert occupation_probability_non_gate_based(distribution, temperature) == 0.0
