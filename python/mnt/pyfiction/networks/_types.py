# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Concrete logic network types used by public workflows."""

from __future__ import annotations

from typing import TypeAlias, TypeVar

from mnt.pyfiction._native.networks import AigNetwork, MigNetwork, TechnologyNetwork, XagNetwork

Network: TypeAlias = TechnologyNetwork | AigNetwork | MigNetwork | XagNetwork
"""The supported concrete logic network types."""

NetworkT = TypeVar("NetworkT", bound=Network)
"""A concrete logic network selected by the caller."""
