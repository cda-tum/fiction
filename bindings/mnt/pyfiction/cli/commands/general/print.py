# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The print command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction import (
    print_sidb_layout,
    sidb_layout,
)
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import (
    Category,
    command,
    one_store,
    store_flags,
)
from mnt.pyfiction.cli.stores import ground_state

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.registry import Parser, Result
    from mnt.pyfiction.cli.session import Session


def _print_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    store_flags(parser)


@command("print", Category.GENERAL, _print_arguments)
def print_command(session: Session, args: argparse.Namespace) -> Result:
    """Print the active truth table, gate-level layout, or cell-level layout.

    A simulated SiDB layout is drawn once, with its ground state charges in place of the dots.
    Networks have no textual form; use 'show -n' to draw one.
    """
    if args.network:
        msg = "networks have no textual form; use 'show -n' to draw the active network"
        raise CommandError(msg)
    name = one_store(args, "truth_table", "gate_layout", "cell_layout")
    if name == "truth_table":
        tt = session.truth_tables.current()
        session.output(f"hex: {tt.to_hex()}\nbin: {tt.to_binary()}")
        return None
    if name == "gate_layout":
        session.output(repr(session.gate_layouts.current()))
        return None
    entry = session.cell_layouts.current()
    if not isinstance(entry.layout, sidb_layout):
        session.output(repr(entry.layout))
        return None
    state = ground_state(entry)
    picture = print_sidb_layout(
        entry.layout, state, lat_color=session.console.is_terminal and not session.console.no_color
    )
    session.output(picture.rstrip())
    if state is not None:
        session.output(f"Ground state energy: {state.energy():.6f} eV", style="result")
    return None
