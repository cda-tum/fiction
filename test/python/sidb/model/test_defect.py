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
    defect_extent,
    is_charged_defect_type,
    is_negatively_charged_defect,
    is_neutral_defect_type,
    is_neutrally_charged_defect,
    is_positively_charged_defect,
    sidb_defect,
    sidb_defect_type,
)


def test_default_arguments():
    defect = sidb_defect(sidb_defect_type.DB)

    assert defect.type == sidb_defect_type.DB
    assert defect.charge == 0
    assert defect.epsilon_r == 0.0
    assert defect.lambda_tf == 0.0


@pytest.mark.parametrize("kind", list(sidb_defect_type))
def test_defect_type_classification(kind: sidb_defect_type) -> None:
    """Every bound defect type selects its documented default extent."""
    defect = sidb_defect(kind)
    charged = kind in (sidb_defect_type.DB, sidb_defect_type.SI_VACANCY)
    neutral = not charged and kind != sidb_defect_type.NONE
    assert is_charged_defect_type(defect) is charged
    assert is_neutral_defect_type(defect) is neutral
    assert defect_extent(defect, None, None) == ((26, 13) if charged else (1, 0) if neutral else (0, 0))


@pytest.mark.parametrize("charge", [-2, 0, 2])
def test_explicit_defect_charge(charge: int) -> None:
    """Electric charge overrides a neutral defect type and selects the matching extent."""
    defect = sidb_defect(sidb_defect_type.UNKNOWN, charge, 5.0, 4.0)
    assert (defect.charge, defect.epsilon_r, defect.lambda_tf) == (charge, 5.0, 4.0)
    assert is_negatively_charged_defect(defect) is (charge < 0)
    assert is_positively_charged_defect(defect) is (charge > 0)
    assert is_neutrally_charged_defect(defect) is (charge == 0)
    assert defect_extent(defect, (1, 1), (1, 0)) == ((1, 1) if charge else (1, 0))


@pytest.mark.parametrize(
    "arguments",
    [
        (sidb_defect_type.DB, -1, 2.0, 4.0),
        (sidb_defect_type.UNKNOWN, -5, 2.0, 4.0),
        (sidb_defect_type.UNKNOWN, -1, 5.0, 4.0),
        (sidb_defect_type.UNKNOWN, -1, 2.0, 5.0),
        (sidb_defect_type.UNKNOWN, -1, 2.0, 4.0),
    ],
)
def test_defect_equality(arguments: tuple) -> None:
    """Equality compares type, charge, permittivity, and screening distance."""
    defect = sidb_defect(sidb_defect_type.UNKNOWN, -1, 2.0, 4.0)
    other = sidb_defect(*arguments)
    expected = arguments == (sidb_defect_type.UNKNOWN, -1, 2.0, 4.0)
    assert (defect == other) is expected
    assert (defect != other) is not expected
