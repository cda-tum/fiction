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
    tokens = shlex.split(str(command)) if command else [str(a) for a in entry["arguments"]]

    flags: list[str] = []
    index = 0
    while index < len(tokens):
        token = tokens[index]
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

    # any translation unit that links against libfiction carries the full include set
    fiction = [entry for entry in entries if "pyfiction" in str(entry["file"])] or entries
    sys.stdout.write(" ".join(extract_flags(fiction[0])) + "\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
