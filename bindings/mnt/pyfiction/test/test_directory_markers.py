# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Check directory marker selection through pytest's command-line interface."""

from __future__ import annotations

from pathlib import Path

import pytest


@pytest.mark.parametrize(
    ("expression", "passed", "deselected"),
    [("simulation", 1, 3), ("sidb and not simulation", 1, 3), ("io", 2, 2), ("", 4, 0)],
)
def test_directory_markers(
    pytester: pytest.Pytester, pytestconfig: pytest.Config, expression: str, passed: int, deselected: int
) -> None:
    """New nested tests inherit markers, while unfiltered runs keep every test."""
    test_dir = Path(__file__).resolve().parent
    pytester.makeconftest((test_dir / "conftest.py").read_text(encoding="utf-8"))
    pytester.makeini("[pytest]\nmarkers =\n" + "\n".join(f"    {m}" for m in pytestconfig.getini("markers")))
    for name in (
        "technology/sidb/simulation/new_category/test_nested.py",
        "technology/sidb/io/test_sidb_io.py",
        "layouts/io/test_layout_io.py",
        "test_unmarked.py",
    ):
        test_file = pytester.path / name
        test_file.parent.mkdir(parents=True, exist_ok=True)
        test_file.write_text("def test_example():\n    pass\n", encoding="utf-8")

    result = pytester.runpytest_subprocess("--strict-markers", "-m", expression)
    result.assert_outcomes(passed=passed, deselected=deselected)
