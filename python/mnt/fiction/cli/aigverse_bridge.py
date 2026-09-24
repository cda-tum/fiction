# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The bridge between ``mnt.pyfiction``'s AIGs and ``aigverse``'s.

The two packages bind ``mockturtle``'s ``aig_network`` in separate nanobind domains, so an AIG of
one is not an AIG of the other and no cast between them exists. The functions here hand a network
across through files in the session's temporary directory: AIGER on the way out, gate-level Verilog
on the way back, because binary AIGER written through a text stream is corrupt on Windows.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

from aigverse.io import read_aiger_into_aig
from aigverse.io import write_verilog as aigverse_write_verilog
from aigverse.networks import NamedAig

from mnt.pyfiction import read_aig_network, set_name, write_aiger

if TYPE_CHECKING:
    from aigverse.networks import Aig

    from mnt.pyfiction import aig_network

    from .session import Session


def to_aigverse(session: Session, network: aig_network) -> Aig:
    """Hand an AIG to ``aigverse``.

    Args:
        session: The session, for a temporary file.
        network: The AIG.

    Returns:
        The same network as an ``aigverse`` AIG with its names.
    """
    path = session.temp_file(".aig")
    try:
        write_aiger(network, str(path))
        return read_aiger_into_aig(str(path))
    finally:
        path.unlink(missing_ok=True)


def from_aigverse(session: Session, aig: Aig, name: str, like: aig_network | None = None) -> aig_network:
    """Take an AIG back from ``aigverse``.

    Verilog carries no input or output names, so they are copied from ``like`` when given.

    Args:
        session: The session, for a temporary file.
        aig: The ``aigverse`` AIG.
        name: The network name to assign.
        like: A network with the same inputs and outputs whose names the result takes over.

    Returns:
        The same network as an ``mnt.pyfiction`` AIG.
    """
    path = session.temp_file(".v")
    try:
        aigverse_write_verilog(aig, str(path))
        network = read_aig_network(str(path))
    finally:
        path.unlink(missing_ok=True)
    set_name(network, name)
    if like is None and hasattr(aig, "has_name"):
        like = aig
    if like is not None:
        for source, target in zip(like.pis(), network.pis(), strict=True):
            source_signal = like.make_signal(source) if isinstance(like, NamedAig) else source
            if like.has_name(source_signal):
                network.set_name(target, like.get_name(source_signal))
        for index in range(network.num_pos()):
            if like.has_output_name(index):
                network.set_output_name(index, like.get_output_name(index))
    return network
