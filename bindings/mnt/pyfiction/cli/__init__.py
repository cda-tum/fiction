# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The ``fiction`` command-line interface: a shell around the ``mnt.pyfiction`` bindings.

Run it as ``fiction`` or ``python -m mnt.pyfiction.cli``. :class:`Session` runs commands
in-process, which is how the test suite drives it.
"""

from __future__ import annotations

from .app import main
from .errors import CommandError
from .session import Session

__all__ = ["CommandError", "Session", "main"]
