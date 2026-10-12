# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Generate docstrings with the project's pinned Clang bindings."""

from __future__ import annotations

from clang.cindex import CursorKind
from pybind11_mkdoc import main as mkdoc_main


def main() -> int:
    """Register the missing Clang 18 concept cursor and run the original generator.

    Returns:
        ``0`` after successful docstring extraction.
    """
    try:
        CursorKind.from_id(153)
    except ValueError:
        # Clang 18 emits concept expressions that its Python bindings do not name.
        CursorKind.CONCEPT_SPECIALIZATION_EXPR = CursorKind(153)
    return mkdoc_main()


if __name__ == "__main__":
    raise SystemExit(main())
