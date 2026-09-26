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

def read_technology_network(filename: str, format: str = "") -> mnt.pyfiction.networks.technology_network:
    """
    Reads a logic network from a Verilog (`.v`), AIGER (`.aig`), or BLIF (`.blif`) file, or the first network from a directory of such files. Preserves output signals without adding output buffers. Raises `RuntimeError` with the parser's diagnostics when no network can be read.
    """

def read_aig_network(filename: str, format: str = "") -> mnt.pyfiction.networks.aig_network:
    """
    Reads a logic network from a Verilog (`.v`), AIGER (`.aig`), or BLIF (`.blif`) file, or the first network from a directory of such files. Preserves output signals without adding output buffers. Raises `RuntimeError` with the parser's diagnostics when no network can be read.
    """

def read_xag_network(filename: str, format: str = "") -> mnt.pyfiction.networks.xag_network:
    """
    Reads a logic network from a Verilog (`.v`), AIGER (`.aig`), or BLIF (`.blif`) file, or the first network from a directory of such files. Preserves output signals without adding output buffers. Raises `RuntimeError` with the parser's diagnostics when no network can be read.
    """

def read_mig_network(filename: str, format: str = "") -> mnt.pyfiction.networks.mig_network:
    """
    Reads a logic network from a Verilog (`.v`), AIGER (`.aig`), or BLIF (`.blif`) file, or the first network from a directory of such files. Preserves output signals without adding output buffers. Raises `RuntimeError` with the parser's diagnostics when no network can be read.
    """

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
    Writes a logic network in DOT format into an output stream, using one
    of the drawers above.

    Args:
        ntk: Network to write.
        os: Output stream.
        drawer: Drawer that decides the node labels, colors, and styles.

    Template Args:
        Ntk: Logic network type.
        Drawer: DOT drawer type.
    """
