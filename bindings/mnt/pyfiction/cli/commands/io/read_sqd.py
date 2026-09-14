# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The read_sqd command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction.cli.registry import Category, command

from ._common import read_file

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.registry import Result
    from mnt.pyfiction.cli.session import Session
from ._common import _read_path_argument


@command("read_sqd", Category.IO, _read_path_argument, inputs="One file.", example='read_sqd "layout.sqd"')
def read_sqd(session: Session, args: argparse.Namespace) -> Result:
    """Read an SiDB cell-level layout from a SiQAD file."""
    return read_file(session, args.path, suffixes=(".sqd",))
