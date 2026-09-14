#!/usr/bin/env -S uv run --script --quiet
# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

# Keep the blank line above: `license-tools` reads an unbroken run of `#` lines as its own header and deletes it
# /// script
# dependencies = ["nox"]
# ///

"""Nox sessions."""

from __future__ import annotations

import argparse
import contextlib
import os
import shutil
import sys
import tempfile
from pathlib import Path
from typing import TYPE_CHECKING

import nox

if TYPE_CHECKING:
    from collections.abc import Generator, Sequence

nox.needs_version = ">=2025.10.16"
nox.options.default_venv_backend = "uv"

if os.environ.get("CI", None):
    # Without this a runner missing an interpreter skips that session and reports success.
    nox.options.error_on_missing_interpreters = True

nox.options.sessions = ["lint", "tests"]

PYTHON_ALL_VERSIONS = ["3.10", "3.11", "3.12", "3.13", "3.14"]
CPP_LINT_IGNORED_PATHS = (
    "build-*",
    "libs/*",
    "vendors/*",
    "docs/*",
    "benchmarks/*",
    "bib/*",
    "bindings/mnt/pyfiction/include/pyfiction/pybind11_mkdoc_docstrings.hpp",
    "bindings/mnt/pyfiction/include/pyfiction/documentation.hpp",
)


@contextlib.contextmanager
def preserve_lockfile(session: nox.Session) -> Generator[None]:
    """Move `uv.lock` aside for the duration of the block and restore it afterwards.

    A session that resolves differently would otherwise leave its own resolution in `uv.lock`.
    While the block runs, the real lockfile is absent from the working tree and `uv` writes a
    stand-in in its place. That stand-in is internally consistent, so `uv lock --check` and the
    `uv-lock` hook accept it, and anything that reads or commits `uv.lock` in that window records
    the stand-in instead of the real lockfile. Run one such session at a time per worktree, and do
    not commit from a worktree while one runs.

    Starting on a `uv.lock` that already differs from `HEAD` aborts the session, because restoring
    would overwrite that difference.
    """
    pending = session.run("git", "diff", "--name-only", "--", "uv.lock", external=True, silent=True)
    if pending and pending.strip():
        session.error("`uv.lock` differs from HEAD. Commit or restore it before running this session")

    with tempfile.TemporaryDirectory() as temp_dir_name:
        shutil.move("uv.lock", f"{temp_dir_name}/uv.lock")
        try:
            yield
        finally:
            shutil.move(f"{temp_dir_name}/uv.lock", "uv.lock")


@nox.session(reuse_venv=True)
def lint(session: nox.Session) -> None:
    """Run the linter."""
    if shutil.which("prek") is None:
        session.install("prek")

    session.run("prek", "run", "--all-files", *session.posargs, external=True)


def _install_build_tools(session: nox.Session) -> None:
    """Install CMake and Ninja when they are unavailable on ``PATH``.

    Args:
        session: Nox session that provides the isolated environment.
    """
    if shutil.which("cmake") is None:
        session.install("cmake")
    if shutil.which("ninja") is None:
        session.install("ninja")


def _run_tests(
    session: nox.Session,
    *,
    install_args: Sequence[str] = (),
    extra_command: Sequence[str] = (),
    pytest_run_args: Sequence[str] = (),
) -> None:
    env = {"UV_PROJECT_ENVIRONMENT": session.virtualenv.location}
    _install_build_tools(session)

    # install build and test dependencies on top of the existing environment
    session.run(
        "uv",
        "sync",
        "--inexact",
        "--only-group",
        "build",
        "--only-group",
        "test",
        *install_args,
        env=env,
    )
    session.run(
        "uv",
        "sync",
        "--inexact",
        "--no-dev",  # do not auto-install dev dependencies
        "--no-build-isolation-package",
        "mnt-pyfiction",  # build the project without isolation
        *install_args,
        env=env,
    )
    if extra_command:
        session.run(*extra_command, env=env)
    session.run(
        "uv",
        "run",
        "--no-sync",  # do not sync as everything is already installed
        *install_args,
        "pytest",
        *pytest_run_args,
        *session.posargs,
        env=env,
    )


@nox.session(python=PYTHON_ALL_VERSIONS, reuse_venv=True, default=True)
def tests(session: nox.Session) -> None:
    """Run the test suite."""
    _run_tests(session)


@nox.session(python="3.10", reuse_venv=True)
def minimums(session: nox.Session) -> None:
    """Run the test suite against the lowest declared direct dependencies.

    Every other entry point resolves each dependency to the newest compatible version, so a lower
    bound in `pyproject.toml` is only ever asserted. This session resolves each direct dependency
    to the oldest version its declared bound allows, so the test suite exercises the bounds
    themselves.

    The session pins Python 3.10, the `requires-python` floor, because the oldest interpreter
    combined with the oldest dependencies is the one combination nothing else in the project
    covers. Under `CI` the module sets `error_on_missing_interpreters`, without which a runner
    that lacks Python 3.10 skips the session and reports success.

    `preserve_lockfile` keeps the lockfile out of the resolution; read its caveat before running
    this session next to anything else in the same worktree.
    """
    with preserve_lockfile(session):
        _run_tests(session, install_args=["--resolution", "lowest-direct"])


@nox.session(reuse_venv=True)
def check_sdist(session: nox.Session) -> None:
    """Diff the built source distribution against the files git tracks.

    Not a prek hook: it builds an sdist, which needs a build environment that the pre-commit
    sandbox does not have. The packaging workflow's sdist job runs `check-sdist` directly, so this
    session is the local entry point.
    """
    session.install("check-sdist")
    session.run("check-sdist", "--inject-junk", *session.posargs)


@nox.session(python="3.12", reuse_venv=True)
def cpp_lint(session: nox.Session) -> None:
    """Run the CI Clang-Tidy configuration on changed or all C++ files.

    Args:
        session: Nox session that supplies the lint scope and isolated environment.
    """
    parser = argparse.ArgumentParser(prog="nox -s cpp_lint --")
    scope = parser.add_mutually_exclusive_group()
    scope.add_argument("--all", action="store_true", dest="all_files", help="lint every eligible C++ file")
    scope.add_argument("--diff-base", help="lint files changed from this Git revision")
    args = parser.parse_args(session.posargs)

    _install_build_tools(session)
    session.install(
        "clang-tools==1.2.0",
        "cpp-linter==1.13.0",
        "nanobind~=3.0.0",
        "z3-solver==4.14.1",
    )
    session.run("clang-tools", "install", "clang-tidy", "--version", "21")
    installed_z3_output = session.run(
        "python",
        "-c",
        "import pathlib, z3; print(pathlib.Path(z3.__file__).parent)",
        silent=True,
    )
    if not installed_z3_output:
        session.error("Could not locate the installed Z3 package")
    installed_z3_root = Path(installed_z3_output.strip())

    with tempfile.TemporaryDirectory(prefix="fiction-z3-") as temp_dir_name:
        z3_root = Path(temp_dir_name, "z3")
        shutil.copytree(installed_z3_root, z3_root)
        z3_library_name = "libz3.dll" if os.name == "nt" else "libz3.dylib" if sys.platform == "darwin" else "libz3.so"

        cmake_args = [
            "-S",
            ".",
            "--preset",
            "ci-tidy",
            "-DSKBUILD_SABI_COMPONENT=Development.SABIModule",
            f"-DZ3_ROOT={z3_root}",
            f"-DZ3_CXX_INCLUDE_DIRS={z3_root / 'include'}",
            f"-DZ3_LIBRARIES={z3_root / 'lib' / z3_library_name}",
        ]
        session.run("cmake", *cmake_args, external=True)

        linter_args = [
            "--style=",
            "--tidy-checks=",
            "--database=build-ci-tidy",
            "--version=21",
            "--lines-changed-only=false",
            f"--files-changed-only={'false' if args.all_files else 'true'}",
            f"--ignore={'|'.join(CPP_LINT_IGNORED_PATHS)}",
            "--file-annotations=false",
            "--jobs=0",
        ]
        if args.diff_base:
            linter_args.append(f"--diff-base={args.diff_base}")

        session.run("cpp-linter", *linter_args)


@nox.session(python="3.12", reuse_venv=True)
def docs(session: nox.Session) -> None:
    """Build documentation, serving interactive HTML builds with live reload.

    Pass ``-b linkcheck`` to check links or ``--non-interactive`` to build and exit.
    Additional arguments are passed to Sphinx.

    Args:
        session: Nox session that supplies arguments and controls interactive mode.
    """
    parser = argparse.ArgumentParser()
    parser.add_argument("-b", dest="builder", default="html")
    args, sphinx_args = parser.parse_known_args(session.posargs)
    if shutil.which("doxygen") is None:
        session.error("Doxygen is required. Install it and add it to PATH")

    env = {
        "UV_PROJECT_ENVIRONMENT": session.virtualenv.location,
        "SKBUILD_BUILD_DIR": "build-pyfiction",
        "SKBUILD_CMAKE_ARGS": "--preset=pyfiction",
    }
    session.run("uv", "sync", "--frozen", "--no-dev", "--group", "build", "--group", "docs", env=env)
    with session.chdir("docs"):
        serve = args.builder == "html" and session.interactive
        command = ["sphinx-autobuild" if serve else "sphinx-build"]
        if serve:
            command.extend(["--ignore", "doxyxml/**", "--watch", "../include/fiction"])
            command.extend(["--pre-build", "doxygen Doxyfile"])
        else:
            session.run("doxygen", "Doxyfile", external=True)
        session.run(
            *command,
            "-T",
            "-b",
            args.builder,
            ".",
            str(Path("_build") / args.builder),
            *sphinx_args,
        )
