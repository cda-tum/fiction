# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The quit command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction.cli.registry import (
    Category,
    command,
)

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.registry import Result
    from mnt.pyfiction.cli.session import Session


@command("quit", Category.GENERAL)
def quit_command(session: Session, args: argparse.Namespace) -> Result:
    """Leave the shell.

    In a script or behind -c, the commands after it are not run.
    """
    del args
    session.running = False
    return None
