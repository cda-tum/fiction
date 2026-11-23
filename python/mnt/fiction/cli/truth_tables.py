# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Parse binary and hexadecimal truth-table specifications."""

from __future__ import annotations

from mnt.pyfiction.synthesis import dynamic_truth_table

from .errors import CommandError


def table_from_string(text: str) -> dynamic_truth_table:
    """Create a truth table from binary bits or hexadecimal digits.

    Returns:
        The parsed truth table.
    """
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


def _log2(count: int, *, what: str) -> int:
    """Return the variable count for a nonempty power-of-two bit count.

    Raises:
        CommandError: The bit count is not a nonempty power of two.
    """
    if count == 0 or count & (count - 1):
        msg = f"the number of {what} must be a power of two, got {count}"
        raise CommandError(msg)
    return count.bit_length() - 1
