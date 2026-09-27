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
    aig_network,
    get_name,
    has_high_degree_fanin_nodes,
    high_degree_fanin_exception,
    mig_network,
    random_aig_network,
    random_mig_network,
    random_tec_network,
    random_xag_network,
    set_name,
    simulate,
    simulate_outputs,
    technology_network,
    xag_network,
)

from . import io

__all__ = [
    "aig_network",
    "get_name",
    "has_high_degree_fanin_nodes",
    "high_degree_fanin_exception",
    "io",
    "mig_network",
    "random_aig_network",
    "random_mig_network",
    "random_tec_network",
    "random_xag_network",
    "set_name",
    "simulate",
    "simulate_outputs",
    "technology_network",
    "xag_network",
]
