# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The ``fiction`` command-line interface: a shell around the ``mnt.pyfiction`` bindings.

Run it as ``fiction`` or ``python -m mnt.fiction.cli``. :class:`Session` runs commands
in-process, which is how the test suite drives it.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

from .app import main
from .errors import CommandError

if TYPE_CHECKING:
    from .session import Session

__all__ = ["CommandError", "Session", "main"]


def __getattr__(name: str) -> type[Session]:
    """Retain the Session import without loading shell state for other consumers.

    Returns:
        The shell session class.

    Raises:
        AttributeError: The requested export does not exist.
    """
    if name == "Session":
        from .session import Session  # ruff: ignore[import-outside-top-level] -- compatibility export

        return Session
    raise AttributeError(name)
