# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Emit the include and define flags that ``pybind11_mkdoc`` needs to parse *fiction*.

``pybind11_mkdoc`` invokes libclang directly, so it needs the same include paths and
preprocessor defines the real build uses -- CMake-generated headers such as
``fiction/utils/version_info.hpp``, the FetchContent'd dependencies, Z3, and ALGLIB.
Reading them out of a configured build's compile database keeps this in step with the
build automatically instead of duplicating the list here.

The flags are emitted in their *joined* form (``-isystem/path``, not ``-isystem /path``).
``read_args()`` in ``pybind11_mkdoc/mkdoc_lib.py`` treats every argument that does not
start with a dash as a header to parse, so a separated path would be swallowed as an
input file and libclang would fail the whole translation unit.

Third-party paths keep ``-isystem`` so their diagnostics stay suppressed. Demoting them
to ``-I`` makes libclang report warnings inside dependency headers, and since
``pybind11_mkdoc`` 3.0 aborts on any error diagnostic, mockturtle's narrowing warnings
alone are enough to fail the run.

Writes through ``sys.stdout`` rather than ``print`` because the lint configuration bans
``print`` outside tests and deletes the call outright when fixing.
"""

from __future__ import annotations

import argparse
import json
import os
import shlex
import sys
from pathlib import Path
from typing import Any

# flags that take their value as a separate following argument
_VALUE_FLAGS = ("-I", "-isystem", "-D", "-include")


def extract_flags(entry: dict[str, Any]) -> list[str]:
    """Return the include and define flags of a single compile database entry.

    Returns:
        The entry's ``-I``, ``-isystem``, ``-D``, and ``-include`` flags, in joined form
        and with duplicates removed.
    """
    command = entry.get("command")
    if command:
        if os.name == "nt":
            # NUL cannot occur in a command line; protect CMake's literal escaped quotes.
            command = str(command).replace(r"\"", "\0")
        lexer = shlex.shlex(str(command), posix=True)
        lexer.whitespace_split = True
        lexer.commenters = ""
        if os.name == "nt":
            # Windows paths use backslashes literally, including inside quoted paths.
            lexer.escape = ""
        tokens = [token.replace("\0", '"') for token in lexer] if os.name == "nt" else list(lexer)
    else:
        tokens = [str(argument) for argument in entry["arguments"]]

    flags: list[str] = []
    index = 0
    while index < len(tokens):
        token = tokens[index]
        if os.name == "nt" and token.startswith(("/I", "/D", "/external:I")):
            token = "-" + token[1:]
        if token.startswith("-external:I"):
            token = "-isystem" + token.removeprefix("-external:I")
        if token in _VALUE_FLAGS:
            flags.append(token + tokens[index + 1])
            index += 2
            continue
        if token.startswith(_VALUE_FLAGS):
            flags.append(token)
        index += 1

    return list(dict.fromkeys(flags))


def main() -> int:
    """Write the flags of one translation unit from the given build directory to stdout.

    Returns:
        ``0`` on success, ``1`` if the build directory holds no usable compile database.
    """
    parser = argparse.ArgumentParser(description="Emit pybind11_mkdoc include and define flags.")
    parser.add_argument("build_dir", type=Path, help="configured CMake build directory")
    args = parser.parse_args()

    database = args.build_dir / "compile_commands.json"
    if not database.is_file():
        sys.stderr.write(f"{database} not found; configure with CMAKE_EXPORT_COMPILE_COMMANDS=ON\n")
        return 1

    entries = json.loads(database.read_text())
    if not entries:
        sys.stderr.write(f"{database} contains no translation units\n")
        return 1

    # any binding translation unit links against libfiction and carries the full include set
    fiction = [entry for entry in entries if "/bindings/" in Path(entry["file"]).as_posix()] or entries
    sys.stdout.write(" ".join(extract_flags(fiction[0])) + "\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
