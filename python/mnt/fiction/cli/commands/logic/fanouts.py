# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The fanouts command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.parsing import integer, seed
from mnt.fiction.cli.registry import Category, command
from mnt.fiction.cli.stores import describe
from mnt.pyfiction.synthesis import fanout_substitution, fanout_substitution_params, substitution_strategy

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


def _fanouts_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument("-d", "--degree", type=integer, choices=[2, 3], default=2, help="fan-out nodes' output degree")
    parser.add_argument(
        "-s",
        "--strategy",
        choices=["breadth", "depth", "random"],
        default="breadth",
        help="how cascaded fan-outs are arranged",
    )
    parser.add_argument("-t", "--threshold", type=integer, default=1, help="maximum output degree left unsubstituted")
    parser.add_argument("--seed", type=seed, help="seed for the random strategy")


@command(
    "fanouts",
    Category.LOGIC,
    _fanouts_arguments,
    inputs="Active network.",
    example="generate mux -b 1; fanouts --degree 2",
    progress=True,
)
def fanouts(session: Session, args: argparse.Namespace) -> Result:
    """Replace high-degree outputs of the active network by fan-out nodes.

    Physical design does this with default settings; run it beforehand to choose the settings.
    """
    params = fanout_substitution_params()
    params.on_progress = session.report_progress
    params.degree = args.degree
    params.threshold = args.threshold
    params.strategy = getattr(substitution_strategy, args.strategy.upper())
    if args.seed is not None:
        params.seed = args.seed
    network = fanout_substitution(session.as_technology_network(session.networks.current()), params)
    session.networks.add(network)
    return {"network": describe(network)}
