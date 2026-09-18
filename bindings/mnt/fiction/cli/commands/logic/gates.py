# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The gates command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from rich.table import Table

from mnt.fiction.cli.registry import Category, command, one_store, store_flags
from mnt.fiction.cli.statistics import stats_to_dict
from mnt.pyfiction import count_gate_types

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


def _gates_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    store_flags(parser, "network", "gate_layout")
    parser.add_argument("--detailed", action="store_true", help="also list the rarer gate types")


@command(
    "gates",
    Category.LOGIC,
    _gates_arguments,
    inputs="Store elements selected by the flags below.",
    example="generate mux -b 1; gates -n",
)
def gates(session: Session, args: argparse.Namespace) -> Result:
    """Count the gate types of the active network or gate-level layout."""
    store = one_store(args, "network", "gate_layout")
    element = session.networks.current() if store == "network" else session.gate_layouts.current()
    gate_types = count_gate_types(element)
    counts = stats_to_dict(gate_types)
    table = Table(box=None, show_header=False, padding=(0, 2))
    common = {"num_fanout", "num_buf", "num_inv", "num_and2", "num_or2", "num_xor2", "num_maj3"}
    for field, count in counts.items():
        if args.detailed or field in common or count:
            table.add_row(field.removeprefix("num_").replace("_", " ").upper(), str(count))
    session.console.print(table)
    return counts
