# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Generate docstrings with the project's pinned Clang bindings."""

from __future__ import annotations

import re
from importlib import import_module


def main() -> int:
    """Register the Clang concept cursor and Python exception names before extraction.

    Returns:
        ``0`` after successful docstring extraction.
    """
    cursor_kind = import_module("clang.cindex").CursorKind
    try:
        cursor_kind.from_id(153)
    except ValueError:
        # Clang 18 emits concept expressions that its Python bindings do not name.
        cursor_kind.CONCEPT_SPECIALIZATION_EXPR = cursor_kind(153)
    generator = import_module("pybind11_mkdoc")
    for cpp_name, python_name in {
        "invalid_argument": "ValueError",
        "out_of_range": "IndexError",
        "overflow_error": "OverflowError",
        "length_error": "ValueError",
        "bad_alloc": "MemoryError",
        "ifstream::failure": "RuntimeError",
        "ofstream::failure": "RuntimeError",
        "ios_base::failure": "RuntimeError",
    }.items():
        generator.mkdoc_lib.INLINE_DOXYGEN_REPLACEMENTS.append((
            re.compile(r"(?<![\w:])std::" + cpp_name + r"(?!\w|::)"),
            python_name,
        ))
    return generator.main()


if __name__ == "__main__":
    raise SystemExit(main())
