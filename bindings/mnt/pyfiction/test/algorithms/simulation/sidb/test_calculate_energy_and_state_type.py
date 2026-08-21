# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from itertools import pairwise

import pytest

from mnt.pyfiction import (
    bdl_input_iterator_100,
    calculate_energy_and_state_type_with_kinks_accepted,
    calculate_energy_distribution,
    create_and_tt,
    detect_bdl_pairs,
    occupation_probability_gate_based,
    quickexact,
    read_sqd_layout_100,
    sidb_technology,
    state_type,
)

# The Bestagon AND gate, evaluated at input pattern 01, has five physically valid charge
# distributions. The two lowest-energy ones encode AND(0, 1) = 0; the three above them do not.
# Energies in eV, taken from the C++ suite, which asserts the same five constants against the same
# layout in `test/algorithms/simulation/sidb/calculate_energy_and_state_type.cpp:71-84`.
EXPECTED_ENERGIES_AND_STATE_TYPES = [
    (0.9915, state_type.ACCEPTED),
    (0.9966, state_type.ACCEPTED),
    (1.2827, state_type.REJECTED),
    (1.2917, state_type.REJECTED),
    (1.2976, state_type.REJECTED),
]


def test_calculate_energy_and_state_type(resources_dir):
    layout = read_sqd_layout_100(str(resources_dir / "21_hex_inputsdbp_and_v19.sqd"))

    # advance the iterator to input pattern 01
    input_iterator = bdl_input_iterator_100(layout)
    input_iterator += 1
    layout_with_inputs = input_iterator.get_layout()

    output_bdl_pairs = detect_bdl_pairs(layout_with_inputs, sidb_technology.cell_type.OUTPUT)
    assert len(output_bdl_pairs) == 1

    charge_distributions = quickexact(layout_with_inputs).charge_distributions
    assert charge_distributions

    distribution = calculate_energy_distribution(charge_distributions)

    energies_and_state_types = calculate_energy_and_state_type_with_kinks_accepted(
        distribution,
        charge_distributions,
        output_bdl_pairs,
        [create_and_tt()],
        1,
    )

    assert len(energies_and_state_types) == len(EXPECTED_ENERGIES_AND_STATE_TYPES)

    # the energies are strictly ascending, so no two states share one energy
    energies = [energy for energy, _ in energies_and_state_types]
    assert all(lower < higher for lower, higher in pairwise(energies))

    for (energy, state), (expected_energy, expected_state) in zip(
        energies_and_state_types, EXPECTED_ENERGIES_AND_STATE_TYPES, strict=True
    ):
        assert energy == pytest.approx(expected_energy, abs=1e-4)
        assert state == expected_state

    # the result is accepted by the one consumer of `state_type` in the library, which sums the
    # Boltzmann factors of the REJECTED states over those of all five
    assert occupation_probability_gate_based(energies_and_state_types, 300) == pytest.approx(1.5974e-05, abs=1e-9)
