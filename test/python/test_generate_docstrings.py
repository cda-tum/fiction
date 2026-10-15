# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Check docstring extraction from concept-constrained declarations."""

from __future__ import annotations

import importlib.util
from pathlib import Path

import pytest


def test_generate_docstrings(tmp_path: Path, monkeypatch: pytest.MonkeyPatch) -> None:
    """Extract the documented declaration with optional real generator dependencies."""
    clang = pytest.importorskip("clang.cindex")
    generator = pytest.importorskip("pybind11_mkdoc")
    header = tmp_path / "concept.hpp"
    header.write_text(
        "template<class T> concept HasValue = requires(T value) { value.x; };\n"
        "/** @brief A documented constrained value. */\n"
        "template<class T> requires HasValue<T> struct Constrained {};\n",
        encoding="utf-8",
    )
    try:
        generator.mkdoc_lib.read_args(["-std=c++20", str(header)])
        clang.Index.create()
    except (clang.LibclangError, OSError):
        pytest.skip("The optional libclang runtime is unavailable")

    script = Path(__file__).resolve().parents[2] / ".github/scripts/generate_docstrings.py"
    spec = importlib.util.spec_from_file_location("generate_docstrings", script)
    assert spec is not None
    assert spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    output = tmp_path / "docstrings.hpp"
    monkeypatch.setattr("sys.argv", [str(script), "-std=c++20", "-o", str(output), str(header)])
    assert module.main() == 0
    generated = output.read_text(encoding="utf-8")
    assert "mkd_doc_Constrained" in generated
    assert "A documented constrained value." in generated
