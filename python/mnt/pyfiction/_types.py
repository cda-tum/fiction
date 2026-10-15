# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Python types shared by native binding signatures."""

from __future__ import annotations

from typing import TypedDict


class TechnologyMappingOptions(TypedDict):
    """Complete boolean keyword dictionary returned by gate-set factories."""

    decay: bool
    inv: bool
    and2: bool
    nand2: bool
    or2: bool
    nor2: bool
    xor2: bool
    xnor2: bool
    lt2: bool
    gt2: bool
    le2: bool
    ge2: bool
    and3: bool
    xor_and: bool
    or_and: bool
    onehot: bool
    maj3: bool
    gamble: bool
    dot: bool
    mux: bool
    and_xor: bool
