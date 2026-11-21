# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Atomic staging for CLI output files."""

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.fiction.cli.output import atomic_output

if TYPE_CHECKING:
    from pathlib import Path


def _fail_after_partial_write(path: Path) -> None:
    """Stage a partial output, then fail.

    Args:
        path: The destination.

    Raises:
        RuntimeError: Always.
    """
    with atomic_output(path) as temporary:
        temporary.write_text("partial")
        msg = "write failed"
        raise RuntimeError(msg)


def test_atomic_output_keeps_destination_on_failure(tmp_path: Path) -> None:
    """A failed write leaves the existing file untouched and no staging directory behind."""
    path = tmp_path / "domain.csv"
    path.write_text("old")
    with pytest.raises(RuntimeError):
        _fail_after_partial_write(path)
    assert path.read_text() == "old"
    assert list(tmp_path.iterdir()) == [path]
    with atomic_output(path) as temporary:
        temporary.write_text("new")
    assert path.read_text() == "new"
