# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Check the compile flags supplied to the docstring generator."""

from __future__ import annotations

import importlib.util
from pathlib import Path
from types import SimpleNamespace

import pytest


@pytest.mark.parametrize(
    ("platform", "entry", "expected"),
    [
        pytest.param(
            "nt",
            {"command": r'cl /nologo -I"C:\Project Files\include" -external:I"C:\Vendor Files" /DWIN32'},
            [r"-IC:\Project Files\include", r"-isystemC:\Vendor Files", "-DWIN32"],
            id="windows-joined-paths",
        ),
        pytest.param(
            "nt",
            {
                "command": r'cl /I "C:\Project Files\include" /external:I"C:\Vendor Files" '
                r'/external:I "C:\Other Files" -DNAME=\"v\" -DLABEL="\"two words\""'
            },
            [
                r"-IC:\Project Files\include",
                r"-isystemC:\Vendor Files",
                r"-isystemC:\Other Files",
                '-DNAME="v"',
                '-DLABEL="two words"',
            ],
            id="windows-separated-paths-and-quotes",
        ),
        pytest.param(
            "posix",
            {"command": 'c++ -I "project includes" -isystem /usr/include -DVALUE=1 -DVALUE=1'},
            ["-Iproject includes", "-isystem/usr/include", "-DVALUE=1"],
            id="posix-deduplicate-flags",
        ),
        pytest.param(
            "posix",
            {"command": "c++ /Docs/input.cpp /Include/input.cpp -I/include"},
            ["-I/include"],
            id="posix-source-paths",
        ),
        pytest.param(
            "posix",
            {"arguments": ["c++", "/Docs/input.cpp", "/Include/input.cpp", "-I/include"]},
            ["-I/include"],
            id="argument-array",
        ),
    ],
)
def test_mkdoc_compile_flags(
    monkeypatch: pytest.MonkeyPatch, platform: str, entry: dict[str, str | list[str]], expected: list[str]
) -> None:
    """Preserve platform-specific include paths, definitions, and their order.

    Args:
        monkeypatch: Restore the simulated platform after each case.
        platform: Platform whose command-line syntax is exercised.
        entry: Compile database entry supplied to the extractor.
        expected: Complete flags, including quote handling and deduplication.
    """
    source = Path(__file__).resolve().parents[2] / ".github/scripts/mkdoc_compile_flags.py"
    if not source.is_file():
        pytest.skip("The docstring generator scripts are not part of the installed test tree.")
    spec = importlib.util.spec_from_file_location("mkdoc_compile_flags", source)
    assert spec is not None
    assert spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    monkeypatch.setattr(module, "os", SimpleNamespace(name=platform))
    assert module.extract_flags(entry) == expected
