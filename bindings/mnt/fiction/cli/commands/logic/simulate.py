# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The simulate command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.registry import Category, command, one_store, store_flags
from mnt.fiction.cli.stores import describe
from mnt.pyfiction import dynamic_truth_table, simulate_outputs

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


def _simulate_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    store_flags(parser, "network", "gate_layout")
    parser.add_argument("--store", action="store_true", help="add the output functions to the truth table store")
    parser.add_argument("--silent", action="store_true", help="do not print the truth tables")


@command(
    "simulate",
    Category.LOGIC,
    _simulate_arguments,
    inputs="Store elements selected by the flags below.",
    example="generate mux -b 1; simulate -n --store",
    progress=True,
)
def simulate_command(session: Session, args: argparse.Namespace) -> Result:
    """Compute the truth table of every output of the active network or gate-level layout.

    Layouts are simulated on the logic level, following the clocking; timing is not considered.
    """
    store = one_store(args, "network", "gate_layout")
    element = session.networks.current() if store == "network" else session.gate_layouts.current()
    tables: list[dict[str, object]] = []
    for output, bits in simulate_outputs(element):
        binary = "".join("1" if bit else "0" for bit in bits)
        table = dynamic_truth_table(len(bits).bit_length() - 1)
        table.create_from_binary_string(binary)
        if args.store:
            session.truth_tables.add(table)
        if not args.silent:
            session.output(f"{output}: {binary} (0x{table.to_hex()})")
        tables.append({"output": output, **describe(table)})
    return {"tables": tables}
