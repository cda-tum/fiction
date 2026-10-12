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
from typing import TYPE_CHECKING

if TYPE_CHECKING:
    import pytest


def test_mkdoc_compile_flags(monkeypatch: pytest.MonkeyPatch) -> None:
    """Preserve quoted Windows paths and system includes without changing POSIX flags."""
    source = Path(__file__).resolve().parents[2] / ".github/scripts/mkdoc_compile_flags.py"
    spec = importlib.util.spec_from_file_location("mkdoc_compile_flags", source)
    assert spec is not None
    assert spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    monkeypatch.setattr(module, "os", SimpleNamespace(name="nt"))
    assert module.extract_flags({
        "command": r'cl /nologo -I"C:\Project Files\include" -external:I"C:\Vendor Files" /DWIN32'
    }) == [r"-IC:\Project Files\include", r"-isystemC:\Vendor Files", "-DWIN32"]
    monkeypatch.setattr(module, "os", SimpleNamespace(name="posix"))
    assert module.extract_flags({"command": 'c++ -I "project includes" -isystem /usr/include -DVALUE=1 -DVALUE=1'}) == [
        "-Iproject includes",
        "-isystem/usr/include",
        "-DVALUE=1",
    ]
