# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of network conversion and technology mapping."""

from __future__ import annotations

from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction.networks import simulate_outputs, technology_network
from mnt.pyfiction.networks.io import read_aig_network, read_mig_network, read_technology_network, read_xag_network
from mnt.pyfiction.synthesis import (
    all_supported_standard_functions,
    convert_network,
    network_target,
    technology_mapping,
)
from mnt.pyfiction.verification import eq_type, equivalence_checking

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path


@pytest.mark.parametrize("reader", [read_technology_network, read_aig_network, read_xag_network, read_mig_network])
def test_convert_network_is_equivalent(resources_dir: Path, reader: Callable[[str], Any]) -> None:
    network = reader(str(resources_dir / "mux21.v"))
    converted = convert_network(network)
    assert isinstance(converted, technology_network)
    assert equivalence_checking(read_technology_network(str(resources_dir / "mux21.v")), converted) == eq_type.STRONG


@pytest.mark.parametrize("reader", [read_aig_network, read_xag_network, read_mig_network])
def test_technology_mapping_accepts_every_network_type(resources_dir: Path, reader: Callable[[str], Any]) -> None:
    network = reader(str(resources_dir / "mux21.v"))
    params = all_supported_standard_functions()
    params.lt2 = True
    mapped = technology_mapping(network, params)
    assert isinstance(mapped, technology_network)
    assert equivalence_checking(convert_network(network), mapped) == eq_type.STRONG


@pytest.mark.parametrize("target", ["TEC", "AIG", "XAG", "MIG"])
def test_conversion_preserves_interfaces(interface_network: technology_network, target: str) -> None:
    """Conversion retains all inputs, output order, labels, and output functions."""
    network = convert_network(interface_network, getattr(network_target, target))
    assert [network.get_name(pi) for pi in network.pis()] == ["apple", "banana", "cherry", "unused"]
    assert simulate_outputs(network) == simulate_outputs(interface_network)
