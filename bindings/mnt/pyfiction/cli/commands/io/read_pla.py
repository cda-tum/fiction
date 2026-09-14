# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The read_pla command."""

from __future__ import annotations

from pathlib import Path
from typing import TYPE_CHECKING

from mnt.pyfiction.cli.registry import Category, command
from mnt.pyfiction.cli.stores import describe

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.registry import Parser, Result
    from mnt.pyfiction.cli.session import Session
from ._common import _existing_file, _read_network, _type_argument


def _arguments_read_pla(parser: Parser) -> None:
    """Configure the read_pla arguments.

    Args:
        parser: The command parser.
    """
    parser.add_argument("path", type=Path, help="the file to read")
    _type_argument(parser)


@command("read_pla", Category.IO, _arguments_read_pla)
def read_pla(session: Session, args: argparse.Namespace) -> Result:
    """Read a logic network from a PLA file.

    The file is read through aigverse as an AIG and converted to --type from there; .ilb and .ob
    interface labels are preserved.
    """
    path = _existing_file(args.path, (".pla",))
    network = _read_network(session, path, args.type, path.suffix.lower()[1:])
    session.networks.add(network)
    return {"network": describe(network)}
