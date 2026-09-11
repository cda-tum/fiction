# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Fixtures that drive the ``fiction`` shell in-process."""

from __future__ import annotations

import io
from pathlib import Path
from typing import TYPE_CHECKING

import pytest
from rich.console import Console

from mnt.pyfiction.cli import Session

if TYPE_CHECKING:
    from collections.abc import Callable, Iterator

RESOURCES_DIR = Path(__file__).resolve().parent.parent / "resources"


class Shell:
    """A session with a recording console.

    Attributes:
        session: The session under test.
    """

    def __init__(self, log_path: Path | None = None) -> None:
        """Create the session.

        Args:
            log_path: Where the session writes its JSON log on close.
        """
        self.buffer = io.StringIO()
        self.error_buffer = io.StringIO()
        console = Console(file=self.buffer, width=200, force_terminal=False, color_system=None)
        errors = Console(file=self.error_buffer, width=200, force_terminal=False, color_system=None)
        self.session = Session(console=console, log_path=log_path, errors=errors)

    def run(self, line: str) -> bool:
        """Run a command line and forget the output so far.

        Args:
            line: The command line.

        Returns:
            Whether every command on the line succeeded.
        """
        for buffer in (self.buffer, self.error_buffer):
            buffer.seek(0)
            buffer.truncate()
        return self.session.execute(line)

    @property
    def stdout(self) -> str:
        """What went to standard output since the last :meth:`run`.

        Returns:
            The recorded informational output.
        """
        return self.buffer.getvalue()

    @property
    def stderr(self) -> str:
        """What went to standard error since the last :meth:`run`.

        Returns:
            The recorded error messages.
        """
        return self.error_buffer.getvalue()

    @property
    def output(self) -> str:
        """Everything printed since the last :meth:`run`, errors included.

        Returns:
            The recorded output of both consoles.
        """
        return self.stdout + self.stderr

    def ok(self, line: str) -> str:
        """Run a line that must succeed and return its output.

        Args:
            line: The command line.

        Returns:
            The output.
        """
        assert self.run(line), self.output
        return self.output

    def fails(self, line: str) -> str:
        """Run a line that must fail and return its output.

        Args:
            line: The command line.

        Returns:
            The output, which contains the error message.
        """
        assert not self.run(line), self.output
        assert "error:" in self.stderr, f"the error went to standard output: {self.stdout}"
        return self.output


@pytest.fixture
def shell() -> Iterator[Shell]:
    """An empty shell, closed after the test.

    Yields:
        A session with a recording console.
    """
    current = Shell()
    yield current
    current.session.close()


@pytest.fixture
def make_shell(tmp_path: Path) -> Iterator[Callable[[], Shell]]:
    """A factory for shells whose log goes to a fresh file under ``tmp_path``; all are closed after the test.

    Yields:
        The factory.
    """
    shells: list[Shell] = []

    def factory() -> Shell:
        shells.append(Shell(log_path=tmp_path / "log.json"))
        return shells[-1]

    yield factory
    for current in shells:
        current.session.close()


@pytest.fixture
def resource() -> Callable[[str], str]:
    """Resolve a file under the shared test resources.

    Returns:
        A function from a file name to its absolute path.
    """

    def path(name: str) -> str:
        return str(RESOURCES_DIR / name)

    return path


@pytest.fixture
def mux21_shell(shell: Shell, resource: Callable[[str], str]) -> Shell:
    """A shell holding the ``mux21`` technology network.

    Returns:
        The shell after ``read mux21.v``.
    """
    shell.ok(f"read {resource('mux21.v')}")
    return shell
