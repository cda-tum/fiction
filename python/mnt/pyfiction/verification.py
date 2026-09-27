# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Design rule, equivalence, and performance checks of gate-level layouts."""

from __future__ import annotations

from mnt.pyfiction._native.verification import (
    count_gate_types,
    count_gate_types_stats,
    critical_path_length_and_throughput,
    eq_type,
    equivalence_checking,
    equivalence_checking_stats,
    gate_level_drv_params,
    gate_level_drv_stats,
    gate_level_drvs,
)

__all__ = [
    "count_gate_types",
    "count_gate_types_stats",
    "critical_path_length_and_throughput",
    "eq_type",
    "equivalence_checking",
    "equivalence_checking_stats",
    "gate_level_drv_params",
    "gate_level_drv_stats",
    "gate_level_drvs",
]
