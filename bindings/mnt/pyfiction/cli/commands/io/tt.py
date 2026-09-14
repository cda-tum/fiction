# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The tt command."""

from __future__ import annotations

import math
from typing import TYPE_CHECKING

from mnt.pyfiction import (
    dynamic_truth_table,
)
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import Category, command
from mnt.pyfiction.cli.stores import describe

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.parsing import Parser
    from mnt.pyfiction.cli.registry import Result
    from mnt.pyfiction.cli.session import Session


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
    source.add_argument("-r", "--random", type=int, metavar="VARS", help="a random function of VARS variables")


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
        table = _table_from_string(args.table)
    session.truth_tables.add(table)
    return {"truth_table": describe(table)}


def _table_from_string(text: str) -> dynamic_truth_table:
    """Create a truth table from binary bits or hexadecimal digits."""
    if text.lower().startswith("0x"):
        digits = text[2:]
        bits = len(digits) * 4
        num_vars = _log2(bits, what="hex digits times four")
        table = dynamic_truth_table(num_vars)
        table.create_from_hex_string(digits)
        return table
    table = dynamic_truth_table(_log2(len(text), what="bits"))
    table.create_from_binary_string(text)
    return table


SMALLEST_TRUTH_TABLE_BITS = 1
"""A truth table of one variable has two bits; that is the smallest string ``tt`` accepts."""


def _log2(count: int, *, what: str) -> int:
    """Return the variable count for a nonempty power-of-two bit count."""
    if count < SMALLEST_TRUTH_TABLE_BITS or count & (count - 1):
        msg = f"the number of {what} must be a power of two, got {count}"
        raise CommandError(msg)
    return int(math.log2(count))
