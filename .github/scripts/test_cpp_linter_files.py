# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Regression tests for local pull-request source selection."""

from __future__ import annotations

import pytest
from cpp_linter_files import make_filter


def test_changes_and_exclusions() -> None:
    """Select new and renamed sources while dropping non-source and excluded files."""
    assert (
        make_filter(
            ["include/new.hpp", "test/renamed.cpp", "readme.md", "vendors/a.hpp", "build-ci-tidy/a.hpp"],
            ["include/new.hpp", "test/renamed.cpp", "test/unchanged.cpp", "vendors/a.hpp", "build-ci-tidy/a.hpp"],
            "vendors/*|build-*",
        )
        == "*|!include/new.hpp|!test/renamed.cpp"
    )


def test_empty_selection() -> None:
    """An empty source diff cannot select unchanged files."""
    assert make_filter([], ["include/a.hpp"], "vendors/*") == "*"


def test_generated_header_exclusion() -> None:
    """Explicit generated-header exclusions survive local selection."""
    generated = "bindings/mnt/pyfiction/include/pyfiction/pybind11_mkdoc_docstrings.hpp"
    assert make_filter([generated], [generated], generated) == "*"


def test_unsafe_filter_characters() -> None:
    """Unrepresentable paths fail instead of being silently ignored."""
    for char in "|!*?[]'\"`$\\\r\n":
        source = f"include/a{char}.hpp"
        with pytest.raises(ValueError, match="Cannot represent source path"):
            make_filter([source], [source], "vendors/*")


def test_suffix_ambiguity() -> None:
    """Relative glob suffix matching cannot expand the requested scope."""
    with pytest.raises(ValueError, match="outside the pull-request diff"):
        make_filter(["include/a.hpp"], ["include/a.hpp", "other/include/a.hpp"], "vendors/*")


def test_path_whitespace() -> None:
    """Filter entry normalization cannot silently remove a selected path."""
    for path in (" include/a.hpp", "\tinclude/a.hpp"):
        with pytest.raises(ValueError, match="Cannot represent source path"):
            make_filter([path], [path], "vendors/*")


def test_missing_source() -> None:
    """An unexpectedly missing changed source cannot produce a successful empty analysis."""
    with pytest.raises(ValueError, match="missing from the checkout"):
        make_filter(["include/missing.hpp"], [], "vendors/*")


def test_hidden_directories() -> None:
    """Hidden parent directories follow cpp-linter's source discovery rules."""
    assert make_filter([".hidden/a.hpp"], [], "vendors/*") == "*"
