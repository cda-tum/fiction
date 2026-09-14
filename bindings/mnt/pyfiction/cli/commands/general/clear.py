# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The clear command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction.cli.registry import (
    Category,
    command,
    selected_stores,
    store_flags,
)

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.parsing import Parser
    from mnt.pyfiction.cli.registry import Result
    from mnt.pyfiction.cli.session import Session
from ._common import stores_of


def _all_stores(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    store_flags(parser)


@command(
    "clear", Category.GENERAL, _all_stores, inputs="Store elements selected by the flags below.", example="clear -g -c"
)
def clear(session: Session, args: argparse.Namespace) -> Result:
    """Remove the elements of the selected stores, or of all stores."""
    stores = stores_of(session)
    for name in selected_stores(args) or list(stores):
        stores[name].clear()
    return None
