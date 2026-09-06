# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Select pull-request sources locally for cpp-linter's whole-file checks."""

from __future__ import annotations

import os
from pathlib import Path, PurePosixPath

EXTENSIONS = {"c", "h", "C", "H", "cpp", "hpp", "cc", "hh", "c++", "h++", "cxx", "hxx"}


def matches(path: str, patterns: list[str]) -> bool:
    """Match paths and parents with cpp-linter's documented filter semantics.

    Args:
        path: Source path relative to the repository root.
        patterns: Glob patterns to match.

    Returns:
        Whether the path or a parent matches a pattern.
    """
    source = PurePosixPath(path)
    return any(part.match(pattern) for part in (source, *source.parents[:-1]) for pattern in patterns)


def make_filter(changed: list[str], existing: list[str], ignored: str) -> str:
    """Build an exact allowlist, failing if filter syntax would change its scope.

    Args:
        changed: Added or modified paths from the pull-request diff.
        existing: Source paths visible to cpp-linter in the current checkout.
        ignored: The workflow's existing pipe-separated ignore patterns.

    Returns:
        A cpp-linter ignore value that selects only the changed eligible sources.

    Raises:
        ValueError: A source is missing, a path cannot be represented safely, or a pattern selects another file.
    """
    exclusions = ignored.split("|")
    selected = sorted({
        path
        for path in changed
        if PurePosixPath(path).suffix.lstrip(".") in EXTENSIONS
        and not any(part.startswith(".") for part in PurePosixPath(path).parts[:-1])
        and not matches(path, exclusions)
    })
    for path in selected:
        if path != path.strip() or any(char in path for char in "|!*?[]'\"`$\\\r\n"):
            msg = f"Cannot represent source path in cpp-linter filter: {path!r}"
            raise ValueError(msg)
    if missing := set(selected).difference(existing):
        msg = f"Changed sources are missing from the checkout: {sorted(missing)!r}"
        raise ValueError(msg)
    if {path for path in existing if matches(path, selected)} != set(selected):
        msg = "cpp-linter path patterns would select files outside the pull-request diff"
        raise ValueError(msg)
    return "|".join(["*", *(f"!{path}" for path in selected)])


def main() -> None:
    """Write the exact local pull-request source filter to the GitHub step output."""
    changed = Path(os.environ["PR_CHANGED_FILES"]).read_bytes().decode().split("\0")[:-1]
    existing = []
    for directory, subdirs, files in os.walk("."):
        subdirs[:] = [name for name in subdirs if not name.startswith(".")]
        existing.extend(
            (Path(directory) / name).as_posix() for name in files if Path(name).suffix.lstrip(".") in EXTENSIONS
        )
    value = make_filter(changed, existing, os.environ["LINT_IGNORE"])
    with Path(os.environ["GITHUB_OUTPUT"]).open("a", encoding="utf-8") as output:
        output.write(f"ignore={value}\n")


if __name__ == "__main__":
    main()
