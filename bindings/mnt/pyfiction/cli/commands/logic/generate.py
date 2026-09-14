# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The generate command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from aigverse.generators import (
    binary_decoder,
    carry_lookahead_adder,
    multiplexer,
    ripple_carry_adder,
    ripple_carry_multiplier,
)

from mnt.pyfiction.cli.aigverse_bridge import from_aigverse
from mnt.pyfiction.cli.registry import Category, command
from mnt.pyfiction.cli.stores import describe

if TYPE_CHECKING:
    import argparse
    from collections.abc import Callable

    from aigverse.networks import Aig

    from mnt.pyfiction.cli.parsing import Parser
    from mnt.pyfiction.cli.registry import Result
    from mnt.pyfiction.cli.session import Session


GENERATORS: dict[str, Callable[[int], Aig]] = {
    "rca": ripple_carry_adder,
    "cla": carry_lookahead_adder,
    "multiplier": ripple_carry_multiplier,
    "mux": multiplexer,
    "decoder": binary_decoder,
}
"""The ``generate`` kinds and the ``aigverse`` generators that build them."""


def _generate_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument("kind", choices=list(GENERATORS), help="rca and cla are adders, multiplier, mux, decoder")
    parser.add_argument("-b", "--bitwidth", type=int, required=True, help="operand width, or select bits for decoder")


@command("generate", Category.LOGIC, _generate_arguments, example="generate rca -b 2")
def generate(session: Session, args: argparse.Namespace) -> Result:
    """Generate an arithmetic or control circuit as an AIG."""
    network = from_aigverse(session, GENERATORS[args.kind](args.bitwidth), f"{args.kind}{args.bitwidth}")
    session.networks.add(network)
    return {"network": describe(network)}
