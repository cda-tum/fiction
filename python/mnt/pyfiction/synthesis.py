# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Truth tables and logic network transformations."""

from __future__ import annotations

from mnt.pyfiction._native.synthesis import (
    TruthTable,
    all_standard_2_input_functions,
    all_standard_3_input_functions,
    all_supported_standard_functions,
    and_or_not,
    and_or_not_maj,
    convert_network,
    fanout_substitution,
    fanout_substitution_params,
    is_balanced,
    is_fanout_substituted,
    mapper_stats,
    missing_required_gates_exception,
    network_balancing,
    network_balancing_params,
    network_target,
    standard_functions,
    substitution_strategy,
    technology_mapping,
    technology_mapping_params,
    technology_mapping_stats,
)

__all__ = [
    "TruthTable",
    "all_standard_2_input_functions",
    "all_standard_3_input_functions",
    "all_supported_standard_functions",
    "and_or_not",
    "and_or_not_maj",
    "convert_network",
    "fanout_substitution",
    "fanout_substitution_params",
    "is_balanced",
    "is_fanout_substituted",
    "mapper_stats",
    "missing_required_gates_exception",
    "network_balancing",
    "network_balancing_params",
    "network_target",
    "standard_functions",
    "substitution_strategy",
    "technology_mapping",
    "technology_mapping_params",
    "technology_mapping_stats",
]
