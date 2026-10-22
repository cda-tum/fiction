# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Check that public Python docstrings describe the arguments callers can pass."""

from __future__ import annotations

import ast
import re
from pathlib import Path


def test_python_parameter_docstrings() -> None:
    """Keep extracted C++ parameter names out of Python keyword documentation."""
    package = Path(__file__).resolve().parents[2] / "python/mnt/pyfiction"
    mismatches = []
    for path in sorted(package.rglob("*.pyi")):
        tree = ast.parse(path.read_text(encoding="utf-8"))
        for node in ast.walk(tree):
            if not isinstance(node, ast.FunctionDef):
                continue
            names = {arg.arg for arg in (*node.args.posonlyargs, *node.args.args, *node.args.kwonlyargs)}
            for arg in (node.args.vararg, node.args.kwarg):
                if arg is not None:
                    names.add(arg.arg)
            # Nanobind gives positional-only operator operands the generic name "arg".
            if node.name.startswith("__") and any(arg.arg == "arg" for arg in node.args.posonlyargs):
                names.add("other")
            doc = ast.get_docstring(node) or ""
            sections = re.findall(r"(?m)^(?:Args|Arguments|Parameters):\n((?:[ \t].*\n|\n)*)", doc + "\n")
            for section in sections:
                mismatches.extend(
                    f"{path.relative_to(package)}:{node.lineno}: {node.name} documents {name!r}"
                    for name in re.findall(r"(?m)^    (\w+)(?:\s*\([^\n]*\))?:", section)
                    if name not in names
                )
    assert not mismatches, "\n".join(mismatches)
