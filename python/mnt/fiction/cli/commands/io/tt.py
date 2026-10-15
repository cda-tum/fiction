# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The tt command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.parsing import integer
from mnt.fiction.cli.registry import Category, command
from mnt.fiction.cli.stores import describe
from mnt.fiction.cli.truth_tables import table_from_string
from mnt.pyfiction.synthesis import dynamic_truth_table

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


def _tt_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    source = parser.add_mutually_exclusive_group(required=True)
    source.add_argument("-t", "--table", metavar="BITS", help="a binary string, or a hex string with a 0x prefix")
    source.add_argument(
        "-e",
        "--expression",
        metavar="EXPR",
        help="a Boolean expression over a..p: !E, (E..E) for AND, {E..E} for OR, [E..E] for XOR, <EEE> for MAJ",
    )
    source.add_argument("-r", "--random", type=integer, metavar="VARS", help="a random function of VARS variables")


@command("tt", Category.IO, _tt_arguments, example='tt -e "<abc>"')
def tt(session: Session, args: argparse.Namespace) -> Result:
    """Create a truth table from a bit string, a hex string, an expression, or at random.

    The number of variables follows from the input: a binary string of 2^n bits, a hex string of
    2^n / 4 digits, or the largest variable in an expression. Bit strings are most significant bit
    first, so 'tt -t 1000' is the two-input AND.
    """
    if args.random is not None:
        table = dynamic_truth_table(args.random)
        table.create_random()
    elif args.expression is not None:
        variables = [ord(char) - ord("a") + 1 for char in args.expression if "a" <= char <= "p"]
        table = dynamic_truth_table(max(variables, default=0))
        table.create_from_expression(args.expression)
    else:
        table = table_from_string(args.table)
    session.truth_tables.add(table)
    return {"truth_table": describe(table)}
