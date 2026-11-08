# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.sidb import site_at_row
from mnt.pyfiction.sidb.io import (
    missing_position_exception,
    read_surface_defects,
    unsupported_defect_index_exception,
)
from mnt.pyfiction.sidb.model import sidb_defect_type

if TYPE_CHECKING:
    from pathlib import Path


@pytest.mark.parametrize("surface", ["[]", "[[]]", "[[][][]]", "[[0 0] [0 0]]"])
def test_empty_surface(tmp_path: Path, surface: str) -> None:
    """Empty rows and defect-free indices produce an empty surface."""
    path = tmp_path / "surface.txt"
    path.write_text(surface, encoding="utf-8")
    layout = read_surface_defects(str(path))
    assert layout.is_empty()
    assert layout.num_defects() == 0


def test_all_surface_defect_indices(tmp_path: Path) -> None:
    """Each supported index retains its type, column, and dimer row."""
    path = tmp_path / "surface.txt"
    path.write_text("[[0 1 2 3] [4 5 6 7] [8 9 10 0]]", encoding="utf-8")
    layout = read_surface_defects(str(path), "surface")
    assert layout.get_layout_name() == "surface"
    assert layout.is_empty()
    assert layout.num_defects() == 10
    expected = [
        "NONE",
        "DB",
        "SI_VACANCY",
        "DIHYDRIDE_PAIR",
        "SINGLE_DIHYDRIDE",
        "ONE_BY_ONE",
        "THREE_BY_ONE",
        "SILOXANE",
        "RAISED_SI",
        "ETCH_PIT",
        "MISSING_DIMER",
        "NONE",
    ]
    assert [layout.get_defect(site_at_row(index % 4, index // 4)).type for index in range(12)] == [
        getattr(sidb_defect_type, name) for name in expected
    ]


@pytest.mark.parametrize("ignored", ["0 -5 0 0", "0 Z 0 0", "111111111111111111111111111111x"])
def test_invalid_rows_do_not_advance_the_row(tmp_path: Path, ignored: str) -> None:
    """Rows containing negative or non-numeric indices are skipped completely."""
    path = tmp_path / "surface.txt"
    path.write_text(f"[[0 1 0 0] [{ignored}] [0 1 0 0]]", encoding="utf-8")
    layout = read_surface_defects(str(path))
    assert layout.num_defects() == 2
    assert [layout.get_defect(site_at_row(1, row)).type for row in range(2)] == [sidb_defect_type.DB] * 2


@pytest.mark.parametrize("separator", [b"\x1a", b"\r\n", b"\t"])
def test_file_preserves_all_rows(tmp_path: Path, separator: bytes) -> None:
    """Binary input and whitespace separators preserve every parsed row on Windows too."""
    path = tmp_path / "surface.txt"
    path.write_bytes(b"text[1]" + separator + b"[2]text")
    layout = read_surface_defects(str(path))
    assert layout.num_defects() == 2
    assert [layout.get_defect(site_at_row(0, row)).type for row in range(2)] == [
        sidb_defect_type.DB,
        sidb_defect_type.SI_VACANCY,
    ]


@pytest.mark.parametrize(
    ("surface", "error"),
    [
        ("[[11]]", unsupported_defect_index_exception),
        ("[[999999999999999999999999]]", unsupported_defect_index_exception),
        ("[[0 1] [0]]", missing_position_exception),
        ("[1 2]\n[invalid]\n[1]", missing_position_exception),
    ],
)
def test_malformed_surface(tmp_path: Path, surface: str, error: type[Exception]) -> None:
    """Parser failures retain their distinct Python ValueError subclasses."""
    path = tmp_path / "surface.txt"
    path.write_text(surface, encoding="utf-8")
    with pytest.raises(error):
        read_surface_defects(str(path))


def test_missing_surface_file(tmp_path: Path) -> None:
    """A missing file raises instead of silently returning an empty layout."""
    with pytest.raises(RuntimeError):
        read_surface_defects(str(tmp_path / "missing.txt"))
