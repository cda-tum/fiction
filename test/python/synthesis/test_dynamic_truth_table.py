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

from mnt.pyfiction.synthesis import (
    TruthTable,
    standard_functions,
)


def test_binary_and_hex_strings() -> None:
    tt = TruthTable(3)
    tt.create_from_binary_string("11101000")
    assert tt.to_hex() == "e8"
    assert tt.to_binary() == "11101000"
    other = TruthTable(3)
    other.create_from_hex_string("e8")
    assert other.to_binary() == tt.to_binary()
    one = TruthTable(1)
    one.create_from_hex_string("2")
    assert one.to_binary() == "10"


def test_expression_matches_the_majority_factory() -> None:
    tt = TruthTable(3)
    tt.create_from_expression("<abc>")
    assert tt.to_hex() == standard_functions("maj")[0].to_hex()


def test_random_has_the_requested_size() -> None:
    tt = TruthTable(4)
    tt.create_random()
    assert len(tt.to_binary()) == 16


def test_named_constructors_and_standard_specifications() -> None:
    table = TruthTable.from_binary("11101000")
    assert table.to_hex() == TruthTable.from_expression("<abc>", num_vars=3).to_hex()
    assert TruthTable.from_hex("e8", num_vars=3).to_binary() == table.to_binary()
    assert [output.to_hex() for output in standard_functions("half_adder")] == ["6", "8"]
    catalog = standard_functions()
    catalog["maj"][0].create_from_binary_string("00000000")
    assert standard_functions("maj")[0].to_binary() == "11101000"
    with pytest.raises(ValueError, match="unknown standard function"):
        standard_functions("missing")


@pytest.mark.parametrize("binary", ["", "101", "10x0"])
def test_binary_constructor_rejects_invalid_input(binary: str) -> None:
    with pytest.raises(ValueError, match=r"power of two|binary character"):
        TruthTable.from_binary(binary)


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
    tt = TruthTable(3)
    with pytest.raises(ValueError, match=message):
        getattr(tt, method)(argument)


@pytest.mark.parametrize("expression", ["p", "[ap]", "(a"])
def test_rejected_expression_preserves_contents(expression: str) -> None:
    table = TruthTable(1)
    table.create_from_binary_string("10")
    with pytest.raises(ValueError, match=r"expression|variable"):
        table.create_from_expression(expression)
    assert table.to_binary() == "10"


@pytest.mark.parametrize(
    "code",
    [
        "f.TruthTable(0).create_from_expression('p')",
        "f.TruthTable(38)",
        "f.TruthTable(64)",
        "f.TruthTable(4294967295)",
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
    assert TruthTable().to_binary() == "0"
