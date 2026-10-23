# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Check Python exception names in extracted binding documentation."""

from __future__ import annotations

import importlib.util
from pathlib import Path
from types import SimpleNamespace

import pytest


@pytest.mark.parametrize(
    ("cpp_name", "python_name"),
    [
        ("invalid_argument", "ValueError"),
        ("out_of_range", "IndexError"),
        ("overflow_error", "OverflowError"),
        ("length_error", "ValueError"),
        ("bad_alloc", "MemoryError"),
        ("ifstream::failure", "RuntimeError"),
        ("ofstream::failure", "RuntimeError"),
        ("ios_base::failure", "RuntimeError"),
    ],
)
def test_python_exception_docstrings(cpp_name: str, python_name: str, monkeypatch: pytest.MonkeyPatch) -> None:
    """Translate native exception names without changing unrelated names or prose."""
    script = Path(__file__).resolve().parents[2] / ".github/scripts/generate_docstrings.py"
    spec = importlib.util.spec_from_file_location("generate_exception_docstrings", script)
    assert spec is not None
    assert spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    replacements = []
    generator = SimpleNamespace(mkdoc_lib=SimpleNamespace(INLINE_DOXYGEN_REPLACEMENTS=replacements), main=lambda: 0)
    imports = {
        "clang.cindex": SimpleNamespace(CursorKind=SimpleNamespace(from_id=lambda _kind: None)),
        "pybind11_mkdoc": generator,
    }
    monkeypatch.setattr(module, "import_module", imports.__getitem__)
    assert module.main() == 0
    comment = (
        "/**\n"
        f" * Rejects invalid input with `std::{cpp_name}`.\n"
        " * @return The unchanged `std::string` result.\n"
        f" * @throws std::{cpp_name} If the input is invalid.\n"
        f" * custom::std::{cpp_name} and std::{cpp_name}_suffix stay unchanged.\n"
        " * std::runtime_error and invalid_argument stay unchanged.\n"
        " */"
    )
    translated = comment
    for pattern, replacement in replacements:
        translated = pattern.sub(replacement, translated)
    expected = comment.replace(f"`std::{cpp_name}`", f"`{python_name}`").replace(
        f"@throws std::{cpp_name} ", f"@throws {python_name} "
    )
    assert translated == expected
