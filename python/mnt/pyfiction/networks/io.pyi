# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Readers, writers, and drawers of logic networks."""

from typing import overload

import mnt.pyfiction.networks

@overload
def read_network(
    filename: str, format: str = "", *, network_type: type[mnt.pyfiction.networks.technology_network] = ...
) -> mnt.pyfiction.networks.technology_network:
    """
    Read Verilog, AIGER, or BLIF, or the first network from a directory. Select a registered network class with network_type (technology_network by default). Preserve output signals without adding output buffers. Raise RuntimeError with parser diagnostics when no network can be read. BLIF supports technology_network only.
    """

@overload
def read_network(
    filename: str, format: str = "", *, network_type: type[mnt.pyfiction.networks.aig_network]
) -> mnt.pyfiction.networks.aig_network: ...
@overload
def read_network(
    filename: str, format: str = "", *, network_type: type[mnt.pyfiction.networks.xag_network]
) -> mnt.pyfiction.networks.xag_network: ...
@overload
def read_network(
    filename: str, format: str = "", *, network_type: type[mnt.pyfiction.networks.mig_network]
) -> mnt.pyfiction.networks.mig_network: ...
@overload
def read_network(
    filename: str,
    format: str = "",
    *,
    network_type: type[
        mnt.pyfiction.networks.technology_network
        | mnt.pyfiction.networks.aig_network
        | mnt.pyfiction.networks.xag_network
        | mnt.pyfiction.networks.mig_network
    ],
) -> (
    mnt.pyfiction.networks.technology_network
    | mnt.pyfiction.networks.aig_network
    | mnt.pyfiction.networks.xag_network
    | mnt.pyfiction.networks.mig_network
): ...
@overload
def write_verilog(network: mnt.pyfiction.networks.technology_network, filename: str) -> None: ...
@overload
def write_verilog(network: mnt.pyfiction.networks.aig_network, filename: str) -> None: ...
@overload
def write_verilog(network: mnt.pyfiction.networks.xag_network, filename: str) -> None: ...
@overload
def write_verilog(network: mnt.pyfiction.networks.mig_network, filename: str) -> None:
    """
    Writes the network as a gate-level Verilog file whose module is named `top`, as the readers expect. A technology network is written as an equivalent XAG, because gate-level Verilog has no buffers or fan-out nodes. A file that cannot be written raises a `RuntimeError`.
    """

@overload
def write_blif(network: mnt.pyfiction.networks.technology_network, filename: str) -> None: ...
@overload
def write_blif(network: mnt.pyfiction.networks.aig_network, filename: str) -> None: ...
@overload
def write_blif(network: mnt.pyfiction.networks.xag_network, filename: str) -> None: ...
@overload
def write_blif(network: mnt.pyfiction.networks.mig_network, filename: str) -> None:
    """
    Writes the network as a BLIF file. A file that cannot be written raises a `RuntimeError`.
    """

def write_aiger(network: mnt.pyfiction.networks.aig_network, filename: str) -> None:
    """
    Writes the AIG as a binary AIGER file, including its input, output, and network names. A file that cannot be written raises a `RuntimeError`.
    """

@overload
def write_dot_network(
    network: mnt.pyfiction.networks.technology_network, filename: str, indexes: bool = True
) -> None: ...
@overload
def write_dot_network(network: mnt.pyfiction.networks.aig_network, filename: str, indexes: bool = True) -> None: ...
@overload
def write_dot_network(network: mnt.pyfiction.networks.xag_network, filename: str, indexes: bool = True) -> None: ...
@overload
def write_dot_network(network: mnt.pyfiction.networks.mig_network, filename: str, indexes: bool = True) -> None:
    """
    Write a logic network to a DOT file with gate-type colors.

    Args:
        network: Logic network to write.
        filename: Path of the output file.
        indexes: Include node indices in labels. Defaults to True.

    Raises:
        RuntimeError: The output file cannot be written.
    """
