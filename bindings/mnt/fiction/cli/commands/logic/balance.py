# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The balance command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.registry import Category, command
from mnt.fiction.cli.stores import describe
from mnt.pyfiction import network_balancing, network_balancing_params

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


def _balance_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument("-u", "--unify-outputs", action="store_true", help="balance the outputs against each other")


@command(
    "balance",
    Category.LOGIC,
    _balance_arguments,
    inputs="Active network.",
    example="generate mux -b 1; balance",
    progress=True,
)
def balance(session: Session, args: argparse.Namespace) -> Result:
    """Balance the paths of the active network with buffers, so every path to a gate has the same length.

    Physical design does not need this and balanced networks produce much larger layouts.
    """
    params = network_balancing_params()
    params.on_progress = session.report_progress
    params.unify_outputs = args.unify_outputs
    network = network_balancing(session.as_technology_network(session.networks.current()), params)
    session.networks.add(network)
    return {"network": describe(network)}
