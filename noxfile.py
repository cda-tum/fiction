#!/usr/bin/env -S uv run --script --quiet
# /// script
# dependencies = ["nox"]
# ///

"""Nox sessions."""

from __future__ import annotations

import contextlib
import os
import shutil
import tempfile
from typing import TYPE_CHECKING

import nox

if TYPE_CHECKING:
    from collections.abc import Generator, Sequence

nox.needs_version = ">=2025.10.16"
nox.options.default_venv_backend = "uv"

nox.options.sessions = ["lint", "tests"]

PYTHON_ALL_VERSIONS = ["3.10", "3.11", "3.12", "3.13", "3.14"]


if os.environ.get("CI", None):
    nox.options.error_on_missing_interpreters = True


@contextlib.contextmanager
def preserve_lockfile() -> Generator[None]:
    """Preserve the lockfile by moving it to a temporary directory."""
    with tempfile.TemporaryDirectory() as temp_dir_name:
        shutil.move("uv.lock", f"{temp_dir_name}/uv.lock")
        try:
            yield
        finally:
            shutil.move(f"{temp_dir_name}/uv.lock", "uv.lock")


@nox.session(reuse_venv=True)
def lint(session: nox.Session) -> None:
    """Run the linter."""
    if shutil.which("pre-commit") is None:
        session.install("pre-commit")

    session.run("pre-commit", "run", "--all-files", *session.posargs, external=True)


def _run_tests(
    session: nox.Session,
    *,
    install_args: Sequence[str] = (),
    extra_command: Sequence[str] = (),
    pytest_run_args: Sequence[str] = (),
) -> None:
    env = {"UV_PROJECT_ENVIRONMENT": session.virtualenv.location}
    if shutil.which("cmake") is None and shutil.which("cmake3") is None:
        session.install("cmake")
    if shutil.which("ninja") is None:
        session.install("ninja")

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
        "--cov-config=pyproject.toml",
        env=env,
    )


@nox.session(python=PYTHON_ALL_VERSIONS, reuse_venv=True, default=True)
def tests(session: nox.Session) -> None:
    """Run the test suite."""
    _run_tests(session)


# @nox.session(reuse_venv=True)
# def docs(session: nox.Session) -> None:
#     """Build the docs. Use "--non-interactive" to avoid serving. Pass "-b linkcheck" to check links."""
#     parser = argparse.ArgumentParser()
#     parser.add_argument("-b", dest="builder", default="html", help="Build target (default: html)")
#     args, posargs = parser.parse_known_args(session.posargs)
#
#     serve = args.builder == "html" and session.interactive
#     extra_installs = ["sphinx-autobuild"] if serve else []
#     session.install(*BUILD_REQUIREMENTS, *extra_installs)
#     session.install("--no-build-isolation", "-ve.[docs]")
#
#     if args.builder == "linkcheck":
#         session.run("sphinx-build", "-b", "linkcheck", "docs", "docs/_build/linkcheck", *posargs)
#         return
#
#     shared_args = (
#         "-n",  # nitpicky mode
#         "-T",  # full tracebacks
#         f"-b={args.builder}",
#         "docs",
#         f"docs/_build/{args.builder}",
#         *posargs,
#     )
#
#     if serve:
#         session.run("sphinx-autobuild", *shared_args)
#     else:
#         session.run("sphinx-build", "--keep-going", *shared_args)
