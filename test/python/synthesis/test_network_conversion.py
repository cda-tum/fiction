# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of network conversion and technology mapping."""

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.networks import AigNetwork, MigNetwork, TechnologyNetwork, XagNetwork, simulate_outputs
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.synthesis import (
    all_supported_standard_functions,
    convert_network,
    technology_mapping,
)
from mnt.pyfiction.verification import EquivalenceType, equivalence_checking

if TYPE_CHECKING:
    from pathlib import Path


@pytest.mark.parametrize("cls", [TechnologyNetwork, AigNetwork, XagNetwork, MigNetwork])
def test_convert_network_is_equivalent(
    resources_dir: Path, cls: type[TechnologyNetwork | AigNetwork | XagNetwork | MigNetwork]
) -> None:
    network = read_network(resources_dir / "mux21.v", network_type=cls)
    converted = convert_network(network)
    assert isinstance(converted, TechnologyNetwork)
    assert equivalence_checking(read_network(str(resources_dir / "mux21.v")), converted).eq == EquivalenceType.STRONG


@pytest.mark.parametrize("cls", [AigNetwork, XagNetwork, MigNetwork])
def test_technology_mapping_accepts_every_network_type(
    resources_dir: Path, cls: type[TechnologyNetwork | AigNetwork | XagNetwork | MigNetwork]
) -> None:
    network = read_network(resources_dir / "mux21.v", network_type=cls)
    params = all_supported_standard_functions()
    params.lt2 = True
    mapped = technology_mapping(network, params=params).network
    assert isinstance(mapped, TechnologyNetwork)
    assert equivalence_checking(convert_network(network), mapped).eq == EquivalenceType.STRONG


@pytest.mark.parametrize("target", [TechnologyNetwork, AigNetwork, XagNetwork, MigNetwork])
def test_conversion_preserves_interfaces(
    interface_network: TechnologyNetwork, target: type[TechnologyNetwork | AigNetwork | XagNetwork | MigNetwork]
) -> None:
    """Conversion retains all inputs, output order, labels, and output functions."""
    network = convert_network(interface_network, network_type=target)
    assert [network.get_name(pi) for pi in network.pis()] == ["apple", "banana", "cherry", "unused"]
    assert simulate_outputs(network) == simulate_outputs(interface_network)
