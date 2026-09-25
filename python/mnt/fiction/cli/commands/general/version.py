# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The version command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.registry import Category, command
from mnt.pyfiction.utils import __compiled_date__, __compiled_time__, __repo__, __version__

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


@command("version", Category.GENERAL)
def version(session: Session, args: argparse.Namespace) -> Result:
    """Print the fiction version and build date."""
    del args
    session.output(f"{__version__} ({__repo__}), compiled {__compiled_date__} {__compiled_time__}", style="result")
    return {"version": __version__, "compiled": f"{__compiled_date__} {__compiled_time__}"}
