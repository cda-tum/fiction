# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of the truth table constructors and string exports."""

from __future__ import annotations

import subprocess  # ruff: ignore[suspicious-subprocess-import] -- bounded native crash regressions
import sys

import pytest

from mnt.pyfiction import synthesis
from mnt.pyfiction.synthesis import dynamic_truth_table


@pytest.mark.parametrize(
    ("gate", "expected"),
    [
        ("id", "10"),
        ("not", "01"),
        ("and", "1000"),
        ("or", "1110"),
        ("nand", "0111"),
        ("nor", "0001"),
        ("xor", "0110"),
        ("xnor", "1001"),
        ("lt", "0010"),
        ("gt", "0100"),
        ("le", "1011"),
        ("ge", "1101"),
        ("and3", "10000000"),
        ("xor_and", "00101000"),
        ("or_and", "10101000"),
        ("onehot", "00010110"),
        ("maj", "11101000"),
        ("gamble", "10000001"),
        ("dot", "01010010"),
        ("ite", "11011000"),
        ("and_xor", "01101010"),
        ("xor3", "10010110"),
    ],
)
def test_gate_truth_tables(gate: str, expected: str) -> None:
    """Every bound gate factory has the documented ordered output bits."""
    table = getattr(synthesis, f"create_{gate}_tt")()
    assert table.to_binary() == expected
    assert table.num_bits() == len(expected)


@pytest.mark.parametrize(
    ("gate", "expected"),
    [
        ("double_wire", ["1100", "1010"]),
        ("crossing_wire", ["1010", "1100"]),
        ("fan_out", ["10", "10"]),
        ("half_adder", ["0110", "1000"]),
    ],
)
def test_multioutput_truth_tables(gate: str, expected: list[str]) -> None:
    assert [table.to_binary() for table in getattr(synthesis, f"create_{gate}_tt")()] == expected


def test_binary_and_hex_strings() -> None:
    tt = dynamic_truth_table(3)
    tt.create_from_binary_string("11101000")
    assert tt.to_hex() == "e8"
    assert tt.to_binary() == "11101000"
    other = dynamic_truth_table(3)
    other.create_from_hex_string("e8")
    assert other.to_binary() == tt.to_binary()
    one = dynamic_truth_table(1)
    one.create_from_hex_string("2")
    assert one.to_binary() == "10"


@pytest.mark.parametrize(
    ("expression", "expected"),
    [
        ("0", "00000000"),
        ("1", "11111111"),
        ("!a", "01010101"),
        ("(abc)", "10000000"),
        ("{abc}", "11111110"),
        ("[abc]", "10010110"),
        ("<abc>", "11101000"),
    ],
)
def test_expression(expression: str, expected: str) -> None:
    tt = dynamic_truth_table(3)
    tt.create_from_expression(expression)
    assert tt.to_binary() == expected


@pytest.mark.parametrize("variables", [0, 1, 2, 6, 7])
def test_size_and_word_boundaries(variables: int) -> None:
    table = dynamic_truth_table(variables)
    bits = 1 << variables
    assert table.num_vars() == variables
    assert table.num_bits() == bits
    assert table.num_blocks() == max(1, bits // 64)
    assert table.to_binary() == "0" * bits
    expected = "1" + "0" * (bits - 1)
    table.create_from_binary_string(expected)
    assert table.to_binary() == expected
    table.create_from_hex_string(table.to_hex().upper())
    assert table.to_binary() == expected


def test_random_has_the_requested_size() -> None:
    tt = dynamic_truth_table(4)
    tt.create_random()
    assert len(tt.to_binary()) == 16


@pytest.mark.parametrize(
    ("method", "argument", "message"),
    [
        ("create_from_binary_string", "101", "needs 8 bits"),
        ("create_from_hex_string", "abc", "needs 2 hex digits"),
        ("create_from_expression", "(a", "could not parse"),
        ("create_from_binary_string", "1110100x", "not a binary character"),
        ("create_from_binary_string", "11101002", "not a binary character"),
        ("create_from_hex_string", "eg", "not a hexadecimal character"),
    ],
)
def test_invalid_input_raises(method: str, argument: str, message: str) -> None:
    """A string of the right length but the wrong alphabet raises instead of producing a wrong table."""
    tt = dynamic_truth_table(3)
    with pytest.raises(ValueError, match=message):
        getattr(tt, method)(argument)


@pytest.mark.parametrize("expression", ["p", "[ap]", "(a"])
def test_rejected_expression_preserves_contents(expression: str) -> None:
    table = dynamic_truth_table(1)
    table.create_from_binary_string("10")
    with pytest.raises(ValueError, match=r"expression|variable"):
        table.create_from_expression(expression)
    assert table.to_binary() == "10"


@pytest.mark.parametrize(
    "code",
    [
        "f.dynamic_truth_table(0).create_from_expression('p')",
        "f.dynamic_truth_table(38)",
        "f.dynamic_truth_table(64)",
        "f.dynamic_truth_table(4294967295)",
    ],
)
def test_invalid_truth_tables_fail_without_native_crash(code: str) -> None:
    result = subprocess.run(  # ruff: ignore[subprocess-without-shell-equals-true] -- fixed Python interpreter and test input
        [
            sys.executable,
            "-c",
            "from mnt.pyfiction import synthesis as f\ntry:\n "
            + code
            + "\nexcept ValueError:\n pass\nelse:\n raise AssertionError('accepted unsafe input')",
        ],
        capture_output=True,
        text=True,
        timeout=20,
        check=False,
    )
    assert result.returncode == 0, result.stderr


def test_default_truth_table_is_zero() -> None:
    assert dynamic_truth_table().to_binary() == "0"
