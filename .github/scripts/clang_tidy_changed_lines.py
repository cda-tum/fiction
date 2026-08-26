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
LOCATED = re.compile(r"^\S.*?:\d+:\d+: (?:warning|error|note):")
UNLOCATED = re.compile(r"^(?:warning|error): ")


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


def located_only(output: str) -> str:
    """Drop diagnostics that carry no source location.

    Clang-Tidy also reports on the macros the compile database defines on the command line --
    `-DABC_NAMESPACE=pabc` draws a `bugprone-macro-parentheses` -- and those diagnostics carry
    no file or line, so `--line-filter` cannot act on them and 66 of them survived into a run
    that is supposed to be about changed lines only. A diagnostic with no location is, by
    definition, not on a changed line.

    Returns:
        The output with every unlocated diagnostic block removed.
    """
    kept: list[str] = []
    keeping = False
    for line in output.splitlines(keepends=True):
        if LOCATED.match(line):
            keeping = True
        elif UNLOCATED.match(line):
            keeping = False
        if keeping:
            kept.append(line)
    return "".join(kept)


def run_one(
    clang_tidy: str,
    build_dir: str,
    resource_dir: str | None,
    disabled: list[str],
    path: str,
    ranges: list[tuple[int, int]],
) -> tuple[str, str]:
    """Lint one file, reporting only what falls inside the ranges the diff added.

    Returns:
        The path and whatever Clang-Tidy wrote to stdout for it.
    """
    line_filter = json.dumps([{"name": Path(path).name, "lines": [[a, b] for a, b in ranges]}])
    argv = [clang_tidy, "-p", build_dir, f"--line-filter={line_filter}", "--quiet"]
    if disabled:
        # `--checks` layers on top of the config file rather than replacing it
        argv.append("--checks=" + ",".join(f"-{check}" for check in disabled))
    if resource_dir:
        argv.append(f"--extra-arg=-resource-dir={resource_dir}")
    completed = subprocess.run(  # ruff: ignore[subprocess-without-shell-equals-true]
        [*argv, path],
        capture_output=True,
        text=True,
        check=False,
    )
    return path, located_only(completed.stdout)


def main() -> int:
    """Lint the diff on stdin.

    Returns:
        ``0`` when the changed lines are clean, ``1`` when any diagnostic survives.
    """
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build-dir", required=True, help="directory holding compile_commands.json")
    parser.add_argument("--clang-tidy", default="clang-tidy", help="the clang-tidy executable to run")
    parser.add_argument(
        "--resource-dir",
        help="clang resource directory holding the builtin headers; the standalone clang-tidy has none",
    )
    parser.add_argument(
        "--disable-check",
        action="append",
        default=[],
        help="a check to switch off on top of .clang-tidy, for a run whose diff makes it meaningless",
    )
    parser.add_argument("--jobs", type=int, default=os.cpu_count() or 1)
    args = parser.parse_args()

    added = {path: ranges for path, ranges in parse_diff(sys.stdin.read()).items() if is_linted(path)}
    files = [path for path in added if Path(path).exists()]
    sys.stderr.write(f"Clang-Tidy over the changed lines of {len(files)} files\n")

    findings: dict[str, str] = {}
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        futures = [
            pool.submit(
                run_one, args.clang_tidy, args.build_dir, args.resource_dir, args.disable_check, path, added[path]
            )
            for path in files
        ]
        for future in concurrent.futures.as_completed(futures):
            path, output = future.result()
            if "warning:" in output or "error:" in output:
                findings[path] = output

    # A clang-tidy that cannot find its builtin headers still runs and still reports, it just
    # reports against libc and the intrinsics headers instead of against the code. That has
    # happened twice here -- once from a standalone binary with no resource directory at all,
    # once from pairing a 21 binary with clang-18's headers, whose MMX builtins 21 has dropped --
    # and both times the log looked like findings. Say so plainly instead.
    broken = [path for path, text in findings.items() if "clang-diagnostic-error" in text]
    if files and len(broken) > len(files) // 2:
        sys.stderr.write(
            f"\nclang-tidy failed to parse {len(broken)} of {len(files)} files. That is a broken\n"
            f"toolchain, not {len(findings)} findings: check that the clang-tidy binary and the\n"
            "resource directory holding its builtin headers come from the same release.\n"
        )

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
