# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Generate docstrings with the project's pinned Clang bindings."""

from __future__ import annotations

from importlib import import_module


def main() -> int:
    """Register the missing Clang 18 concept cursor and run the original generator.

    Returns:
        ``0`` after successful docstring extraction.
    """
    cursor_kind = import_module("clang.cindex").CursorKind
    try:
        cursor_kind.from_id(153)
    except ValueError:
        # Clang 18 emits concept expressions that its Python bindings do not name.
        cursor_kind.CONCEPT_SPECIALIZATION_EXPR = cursor_kind(153)
    return import_module("pybind11_mkdoc").main()


if __name__ == "__main__":
    raise SystemExit(main())
