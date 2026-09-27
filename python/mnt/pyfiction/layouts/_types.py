# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Type annotations shared by layout I/O and physical design."""

from __future__ import annotations

from typing import TypeAlias, TypeVar

from mnt.pyfiction._native.layouts import (
    CartesianGateLayout,
    EvenColumnCartesianGateLayout,
    EvenColumnHexGateLayout,
    EvenRowCartesianGateLayout,
    HexagonalGateLayout,
    OddColumnHexGateLayout,
    OddRowCartesianGateLayout,
    OddRowHexGateLayout,
    ShiftedCartesianGateLayout,
)

GateLayout: TypeAlias = (
    CartesianGateLayout
    | ShiftedCartesianGateLayout
    | HexagonalGateLayout
    | OddRowCartesianGateLayout
    | EvenRowCartesianGateLayout
    | EvenColumnCartesianGateLayout
    | OddRowHexGateLayout
    | OddColumnHexGateLayout
    | EvenColumnHexGateLayout
)
"""The supported concrete gate layout types; this alias is not a constructor."""

GateLayoutT = TypeVar("GateLayoutT", bound=GateLayout)
"""A concrete gate layout selected by the caller."""
