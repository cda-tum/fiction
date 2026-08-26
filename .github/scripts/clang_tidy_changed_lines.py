# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Run Clang-Tidy over the lines a unified diff adds, reading the diff from stdin.

``cpp-linter-action`` does the same thing by asking the REST API for the pull request's
diff, and the API refuses to produce one for more than 300 files. The ``include/fiction``
restructure changes far more than that, so the action fails before Clang-Tidy ever runs.
Computing the diff locally has no such ceiling, and the checkout already runs at
``fetch-depth: 0``, so both sides of the comparison are there.

This is deliberately narrow: it exists to keep the check meaningful while a single
tree-wide change is in flight. Once that has landed, the workflow goes back to
``cpp-linter-action`` linting whole files.

Diagnostics are reported on stderr and summarized for the step summary. Any diagnostic
that survives the line filter fails the run.
"""

from __future__ import annotations

import argparse
import concurrent.futures
import fnmatch
import json
import os
import re
import subprocess  # ruff: ignore[suspicious-subprocess-import] - runs clang-tidy on a fixed argv
import sys
from pathlib import Path

# mirrors the `ignore:` list `cpp-linter-action` was given, so the two agree on scope
IGNORE = (
    "build-*/*",
    "libs/*",
    "vendors/*",
    "docs/*",
    "benchmarks/*",
    "bib/*",
    "bindings/mnt/pyfiction/include/pyfiction/pybind11_mkdoc_docstrings.hpp",
    "bindings/mnt/pyfiction/include/pyfiction/documentation.hpp",
)
SOURCE_SUFFIXES = {".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".hxx"}

FILE_HEADER = re.compile(r"^\+\+\+ b/(.+)$")
HUNK_HEADER = re.compile(r"^@@ -\d+(?:,\d+)? \+(\d+)(?:,(\d+))? @@")


def parse_diff(diff: str) -> dict[str, list[tuple[int, int]]]:
    """Return the added-line ranges per file, one-based and inclusive."""
    added: dict[str, list[tuple[int, int]]] = {}
    current: str | None = None
    line_number = 0
    for line in diff.splitlines():
        header = FILE_HEADER.match(line)
        if header:
            current = header.group(1)
            continue
        hunk = HUNK_HEADER.match(line)
        if hunk:
            line_number = int(hunk.group(1))
            continue
        if current is None:
            continue
        if line.startswith("+") and not line.startswith("+++"):
            ranges = added.setdefault(current, [])
            # extend the open range rather than emitting one entry per line
            if ranges and ranges[-1][1] == line_number - 1:
                ranges[-1] = (ranges[-1][0], line_number)
            else:
                ranges.append((line_number, line_number))
            line_number += 1
        elif not line.startswith("-"):
            line_number += 1
    return added


def is_linted(path: str) -> bool:
    """Say whether Clang-Tidy has anything to say about this path.

    Returns:
        ``True`` for a C or C++ source the ignore list does not cover.
    """
    return Path(path).suffix in SOURCE_SUFFIXES and not any(fnmatch.fnmatch(path, p) for p in IGNORE)


def run_one(clang_tidy: str, build_dir: str, path: str, ranges: list[tuple[int, int]]) -> tuple[str, str]:
    """Lint one file, reporting only what falls inside the ranges the diff added.

    Returns:
        The path and whatever Clang-Tidy wrote to stdout for it.
    """
    line_filter = json.dumps([{"name": Path(path).name, "lines": [[a, b] for a, b in ranges]}])
    completed = subprocess.run(  # ruff: ignore[subprocess-without-shell-equals-true]
        [clang_tidy, "-p", build_dir, f"--line-filter={line_filter}", "--quiet", path],
        capture_output=True,
        text=True,
        check=False,
    )
    return path, completed.stdout


def main() -> int:
    """Lint the diff on stdin.

    Returns:
        ``0`` when the changed lines are clean, ``1`` when any diagnostic survives.
    """
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-dir", required=True, help="directory holding compile_commands.json")
    parser.add_argument("--clang-tidy", default="clang-tidy", help="the clang-tidy executable to run")
    parser.add_argument("--jobs", type=int, default=os.cpu_count() or 1)
    args = parser.parse_args()

    added = {path: ranges for path, ranges in parse_diff(sys.stdin.read()).items() if is_linted(path)}
    files = [path for path in added if Path(path).exists()]
    sys.stderr.write(f"Clang-Tidy over the changed lines of {len(files)} files\n")

    findings: dict[str, str] = {}
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        futures = [pool.submit(run_one, args.clang_tidy, args.build_dir, path, added[path]) for path in files]
        for future in concurrent.futures.as_completed(futures):
            path, output = future.result()
            if "warning:" in output or "error:" in output:
                findings[path] = output

    summary = os.environ.get("GITHUB_STEP_SUMMARY")
    if summary:
        with Path(summary).open("a", encoding="utf-8") as handle:
            if findings:
                total = sum(len(re.findall(r"(?:warning|error):", text)) for text in findings.values())
                handle.write(f"### Clang-Tidy reported {total} diagnostics in {len(findings)} files\n\n")
                for path in sorted(findings):
                    handle.write(f"<details><summary>{path}</summary>\n\n```\n{findings[path]}\n```\n\n</details>\n")
            else:
                handle.write(f"Clang-Tidy found nothing on the changed lines of {len(files)} files.\n")

    for path in sorted(findings):
        sys.stderr.write(findings[path])
    return 1 if findings else 0


if __name__ == "__main__":
    raise SystemExit(main())
