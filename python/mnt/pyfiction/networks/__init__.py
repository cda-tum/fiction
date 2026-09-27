# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Logic networks and their simulation."""

from __future__ import annotations

from mnt.pyfiction._native.networks import (
    AigNetwork,
    MigNetwork,
    Signal,
    TechnologyNetwork,
    XagNetwork,
    get_name,
    has_high_degree_fanin_nodes,
    high_degree_fanin_exception,
    random_aig_network,
    random_mig_network,
    random_tec_network,
    random_xag_network,
    set_name,
    simulate,
    simulate_outputs,
)

from . import io

__all__ = [
    "AigNetwork",
    "MigNetwork",
    "Signal",
    "TechnologyNetwork",
    "XagNetwork",
    "get_name",
    "has_high_degree_fanin_nodes",
    "high_degree_fanin_exception",
    "io",
    "random_aig_network",
    "random_mig_network",
    "random_tec_network",
    "random_xag_network",
    "set_name",
    "simulate",
    "simulate_outputs",
]
