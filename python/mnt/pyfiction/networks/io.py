# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Readers, writers, and drawers of logic networks."""

from __future__ import annotations

from mnt.pyfiction._native.networks.io import (
    read_aig_network,
    read_mig_network,
    read_technology_network,
    read_xag_network,
    write_aiger,
    write_blif,
    write_dot_network,
    write_verilog,
)

__all__ = [
    "read_aig_network",
    "read_mig_network",
    "read_technology_network",
    "read_xag_network",
    "write_aiger",
    "write_blif",
    "write_dot_network",
    "write_verilog",
]
