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

from mnt import pyfiction as fiction
from mnt.pyfiction import create_maj_tt, dynamic_truth_table


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


def test_expression_matches_the_majority_factory() -> None:
    tt = dynamic_truth_table(3)
    tt.create_from_expression("<abc>")
    assert tt.to_hex() == create_maj_tt().to_hex()


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
    table = fiction.dynamic_truth_table(1)
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
            "from mnt import pyfiction as f\ntry:\n "
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
    assert fiction.dynamic_truth_table().to_binary() == "0"
