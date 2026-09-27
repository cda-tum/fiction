# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Read, write, and draw logic networks using filesystem paths."""

from __future__ import annotations

from os import PathLike, fspath
from typing import TYPE_CHECKING, overload

from mnt.pyfiction._native.networks import io as _native
from mnt.pyfiction.networks import AigNetwork, MigNetwork, TechnologyNetwork, XagNetwork

if TYPE_CHECKING:
    from collections.abc import Callable

    from mnt.pyfiction.networks._types import Network, NetworkT

_READERS: dict[type[object], Callable[[str, str], Network]] = {
    TechnologyNetwork: _native.read_technology_network,
    AigNetwork: _native.read_aig_network,
    MigNetwork: _native.read_mig_network,
    XagNetwork: _native.read_xag_network,
}


@overload
def read_network(path: str | PathLike[str], *, file_format: str = "") -> TechnologyNetwork: ...


@overload
def read_network(path: str | PathLike[str], *, network_type: type[NetworkT], file_format: str = "") -> NetworkT: ...


def read_network(
    path: str | PathLike[str], *, network_type: type[object] = TechnologyNetwork, file_format: str = ""
) -> Network:
    """Read a logic network with the selected representation.

    Args:
        path: Input file.
        network_type: TechnologyNetwork, AigNetwork, MigNetwork, or XagNetwork.
        file_format: Optional format override; the suffix selects the format when empty.
            Technology networks support Verilog, AIGER, and BLIF. AIG, MIG, and XAG readers
            support Verilog and AIGER.

    Unreadable or malformed input raises RuntimeError with parser diagnostics.

    Returns:
        The parsed network, including input and output names.

    Raises:
        ValueError: The network type is unsupported.
    """
    if network_type not in _READERS:
        msg = f"unsupported network type: {network_type.__name__}"
        raise ValueError(msg)
    return _READERS[network_type](fspath(path), file_format)


def write_verilog(network: Network, path: str | PathLike[str]) -> None:
    """Write a logic network as Verilog.

    Args:
        network: The logic network to serialize.
        path: Output file.
    """
    _native.write_verilog(network, fspath(path))


def write_blif(network: Network, path: str | PathLike[str]) -> None:
    """Write a logic network as BLIF.

    Args:
        network: The logic network to serialize.
        path: Output file.
    """
    _native.write_blif(network, fspath(path))


def write_aiger(network: AigNetwork, path: str | PathLike[str]) -> None:
    """Write a logic network as binary AIGER.

    Args:
        network: The logic network to serialize.
        path: Output file.
    """
    _native.write_aiger(network, fspath(path))


def write_dot_network(network: Network, path: str | PathLike[str], *, indexes: bool = True) -> None:
    """Write a logic network as Graphviz DOT.

    Args:
        network: The logic network to serialize.
        path: Output file.
        indexes: Label nodes with their identifiers.
    """
    _native.write_dot_network(network, fspath(path), indexes)


__all__ = ["read_network", "write_aiger", "write_blif", "write_dot_network", "write_verilog"]
