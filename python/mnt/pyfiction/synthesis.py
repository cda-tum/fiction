# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Truth tables and logic network transformations."""

from __future__ import annotations

from dataclasses import dataclass
from typing import TYPE_CHECKING, overload

from mnt.pyfiction._native import synthesis as _native
from mnt.pyfiction.networks import AigNetwork, MigNetwork, TechnologyNetwork, XagNetwork

if TYPE_CHECKING:
    from mnt.pyfiction.networks._types import Network, NetworkT

from mnt.pyfiction._native.synthesis import (
    FanoutSubstitutionParams,
    MapperStats,
    MissingRequiredGatesError,
    NetworkBalancingParams,
    SubstitutionStrategy,
    TechnologyMappingParams,
    TechnologyMappingStats,
    TruthTable,
    all_standard_2_input_functions,
    all_standard_3_input_functions,
    all_supported_standard_functions,
    and_or_not,
    and_or_not_maj,
    standard_functions,
)

__all__ = [
    "FanoutSubstitutionParams",
    "MapperStats",
    "MappingResult",
    "MissingRequiredGatesError",
    "NetworkBalancingParams",
    "SubstitutionStrategy",
    "TechnologyMappingParams",
    "TechnologyMappingStats",
    "TruthTable",
    "all_standard_2_input_functions",
    "all_standard_3_input_functions",
    "all_supported_standard_functions",
    "and_or_not",
    "and_or_not_maj",
    "convert_network",
    "fanout_substitution",
    "is_balanced",
    "is_fanout_substituted",
    "network_balancing",
    "standard_functions",
    "technology_mapping",
]


@dataclass(frozen=True)
class MappingResult:
    """A mapped technology network and its mapping statistics."""

    network: TechnologyNetwork
    stats: TechnologyMappingStats


def technology_mapping(network: Network, *, params: TechnologyMappingParams | None = None) -> MappingResult:
    """Map a logic network to the gate library selected by the parameters.

    Args:
        network: Network to map.
        params: Gate library and mapping options. None uses the native defaults.

    Returns:
        The mapped network and statistics. Inspect stats.mapper_stats.mapping_error
        before using the network when the selected library cannot cover the input.
    """
    stats = TechnologyMappingStats()
    mapped = _native.technology_mapping(network, params if params is not None else TechnologyMappingParams(), stats)
    return MappingResult(mapped, stats)


@overload
def convert_network(network: Network) -> TechnologyNetwork: ...


@overload
def convert_network(network: Network, *, network_type: type[NetworkT]) -> NetworkT: ...


def convert_network(network: Network, *, network_type: type[object] = TechnologyNetwork) -> Network:
    """Return an equivalent network of the selected concrete type.

    Args:
        network: Source network.
        network_type: TechnologyNetwork, AigNetwork, MigNetwork, or XagNetwork.

    Returns:
        An independent network with preserved interface names and functions.

    Raises:
        ValueError: The selected network type is unsupported.
    """
    targets: dict[type[object], _native.NetworkTarget] = {
        TechnologyNetwork: _native.NetworkTarget.TEC,
        AigNetwork: _native.NetworkTarget.AIG,
        XagNetwork: _native.NetworkTarget.XAG,
        MigNetwork: _native.NetworkTarget.MIG,
    }
    if network_type not in targets:
        msg = f"unsupported network type: {network_type.__name__}"
        raise ValueError(msg)
    return _native.convert_network(network, targets[network_type])


def fanout_substitution(
    network: TechnologyNetwork, *, params: FanoutSubstitutionParams | None = None
) -> TechnologyNetwork:
    """Return a network with fanout trees bounded by the selected degree.

    Args:
        network: The technology network.
        params: Algorithm options. None uses the native defaults.

    Returns:
        The independent transformed network.
    """
    return _native.fanout_substitution(network, params if params is not None else FanoutSubstitutionParams())


def is_fanout_substituted(network: TechnologyNetwork, *, params: FanoutSubstitutionParams | None = None) -> bool:
    """Check whether the network satisfies the selected fanout bounds.

    Args:
        network: The technology network.
        params: Algorithm options. None uses the native defaults.

    Returns:
        Whether the network satisfies the selected constraints.
    """
    return _native.is_fanout_substituted(network, params if params is not None else FanoutSubstitutionParams())


def network_balancing(network: TechnologyNetwork, *, params: NetworkBalancingParams | None = None) -> TechnologyNetwork:
    """Return a network whose paths are balanced by buffers.

    Args:
        network: The technology network.
        params: Algorithm options. None uses the native defaults.

    Returns:
        The independent transformed network.
    """
    return _native.network_balancing(network, params if params is not None else NetworkBalancingParams())


def is_balanced(network: TechnologyNetwork, *, params: NetworkBalancingParams | None = None) -> bool:
    """Check whether the network paths satisfy the balancing options.

    Args:
        network: The technology network.
        params: Algorithm options. None uses the native defaults.

    Returns:
        Whether the network satisfies the selected constraints.
    """
    return _native.is_balanced(network, params if params is not None else NetworkBalancingParams())
