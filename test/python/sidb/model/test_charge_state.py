# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction.sidb.model import (
    SIDB_CHARGE_STATES_BASE_2,
    SIDB_CHARGE_STATES_BASE_3,
    charge_configuration_to_string,
    charge_state_to_sign,
    sidb_charge_state,
    sidb_charge_states_for_base_number,
    sign_to_charge_state,
)


@pytest.mark.parametrize(
    ("state", "sign"),
    [
        (sidb_charge_state.NEGATIVE, -1),
        (sidb_charge_state.NEUTRAL, 0),
        (sidb_charge_state.POSITIVE, 1),
        (sidb_charge_state.NONE, 0),
    ],
)
def test_charge_sign(state: sidb_charge_state, sign: int) -> None:
    """Each charge state has its documented electric sign."""
    assert charge_state_to_sign(state) == sign


@pytest.mark.parametrize("sign", [-128, -4, -3, -2, -1, 0, 1, 2, 3, 4, 127])
def test_sign_conversion(sign: int) -> None:
    """Only the three electric signs identify a charge state."""
    expected = {-1: sidb_charge_state.NEGATIVE, 0: sidb_charge_state.NEUTRAL, 1: sidb_charge_state.POSITIVE}
    assert sign_to_charge_state(sign) == expected.get(sign, sidb_charge_state.NONE)


@pytest.mark.parametrize("text", ["", "-", "0", "+", "+-0", "N0N-N"])
def test_charge_serialization(text: str) -> None:
    """Serialization preserves charge order and omits unoccupied sites."""
    states = {
        "-": sidb_charge_state.NEGATIVE,
        "0": sidb_charge_state.NEUTRAL,
        "+": sidb_charge_state.POSITIVE,
        "N": sidb_charge_state.NONE,
    }
    assert charge_configuration_to_string([states[character] for character in text]) == text.replace("N", "")


@pytest.mark.parametrize("base", [2, 3])
def test_charge_states_for_base(base: int) -> None:
    """The state tables contain every digit in charge-index order."""
    expected = [sidb_charge_state.NEGATIVE, sidb_charge_state.NEUTRAL, sidb_charge_state.POSITIVE][:base]
    assert list(sidb_charge_states_for_base_number(base)) == expected
    assert list(SIDB_CHARGE_STATES_BASE_2 if base == 2 else SIDB_CHARGE_STATES_BASE_3) == expected
