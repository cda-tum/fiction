# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The random command."""

from __future__ import annotations

import secrets
from typing import TYPE_CHECKING

from mnt import pyfiction
from mnt.pyfiction import (
    network_target,
)
from mnt.pyfiction.cli.registry import Category, command
from mnt.pyfiction.cli.stores import describe

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.registry import Parser, Result
    from mnt.pyfiction.cli.session import Session


NETWORK_TARGETS = {
    "aig": None,
    "xag": network_target.XAG,
    "mig": network_target.MIG,
    "tec": network_target.TEC,
}
"""The network types available to the native random generators."""


def _random_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument("-n", "--inputs", type=int, required=True, help="number of primary inputs")
    parser.add_argument("-g", "--gates", type=int, required=True, help="number of gates")
    parser.add_argument(
        "--type",
        type=str.lower,
        choices=list(NETWORK_TARGETS),
        default="aig",
        help="the network type to produce",
    )
    parser.add_argument("--seed", type=int, help="random seed; a fresh one is drawn when omitted")


@command("random", Category.LOGIC, _random_arguments)
def random_command(session: Session, args: argparse.Namespace) -> Result:
    """Generate a random network; the seed becomes its name.

    Each type uses its native gate distribution; technology networks use the mixed generator.
    """
    seed = args.seed if args.seed is not None else secrets.randbelow(2**32)
    network = getattr(pyfiction, f"random_{args.type}_network")(args.inputs, args.gates, seed)
    session.networks.add(network)
    return {"network": describe(network), "seed": seed}
