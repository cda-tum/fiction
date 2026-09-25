# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The map command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.errors import CommandError
from mnt.fiction.cli.registry import Category, command
from mnt.fiction.cli.render import table as render_table
from mnt.fiction.cli.statistics import stats_to_dict
from mnt.fiction.cli.stores import describe
from mnt.pyfiction.networks import get_name, set_name
from mnt.pyfiction.synthesis import (
    all_standard_2_input_functions,
    all_standard_3_input_functions,
    all_supported_standard_functions,
    technology_mapping,
    technology_mapping_params,
    technology_mapping_stats,
)

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


GATE_FLAGS = (
    ("inv", "inv"),
    ("and", "and2"),
    ("or", "or2"),
    ("xor", "xor2"),
    ("nand", "nand2"),
    ("nor", "nor2"),
    ("xnor", "xnor2"),
    ("lt", "lt2"),
    ("gt", "gt2"),
    ("le", "le2"),
    ("ge", "ge2"),
    ("maj", "maj3"),
    ("dot", "dot"),
    ("mux", "mux"),
    ("and3", "and3"),
    ("xor-and", "xor_and"),
    ("or-and", "or_and"),
    ("and-xor", "and_xor"),
    ("onehot", "onehot"),
    ("gamble", "gamble"),
)
"""The ``map`` flags and the ``technology_mapping_params`` fields they set, in gate-complexity order."""


GATE_SHORT_FLAGS = {"and": "a", "or": "o", "xor": "x", "inv": "i", "maj": "m", "dot": "d"}
"""The six gate flags that also have the short form the C++ shell offered."""


def _map_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    gates = parser.add_argument_group("gate types")
    for flag, _ in GATE_FLAGS:
        names = (f"-{GATE_SHORT_FLAGS[flag]}", f"--{flag}") if flag in GATE_SHORT_FLAGS else (f"--{flag}",)
        gates.add_argument(*names, action="store_true", help=f"allow {flag.upper().replace('-', ' and ')}")
    every = parser.add_mutually_exclusive_group()
    every.add_argument("--all2", action="store_true", help="every 2-input function")
    every.add_argument("--all3", action="store_true", help="every 3-input function")
    every.add_argument("--all", action="store_true", help="every supported function")
    parser.add_argument(
        "--decay",
        action="store_true",
        help="enforce at least one constant input on three-input gates",
    )
    parser.add_argument("-v", "--verbose", action="store_true", help="print the statistics")


@command(
    "map",
    Category.LOGIC,
    _map_arguments,
    inputs="Active network.",
    example="generate mux -b 1; map --and --inv",
    progress=True,
)
def map_command(session: Session, args: argparse.Namespace) -> Result:
    """Map the active network onto a set of gate types, producing a technology network.

    Select at least one gate type, e.g. 'map --and --or --inv' for AND-OR-inverter networks.
    """
    params = technology_mapping_params()
    if args.all:
        params = all_supported_standard_functions()
    elif args.all3:
        params = all_standard_3_input_functions()
    elif args.all2:
        params = all_standard_2_input_functions()
    selected = [field for flag, field in GATE_FLAGS if getattr(args, flag.replace("-", "_"))]
    for field in selected:
        setattr(params, field, True)
    if not selected and not (args.all or args.all2 or args.all3):
        msg = "select at least one gate type, e.g. 'map --and --or --inv'"
        raise CommandError(msg)
    params.decay = args.decay

    network = session.networks.current()
    stats = technology_mapping_stats()
    mapped = technology_mapping(network, params, stats)
    if stats.mapper_stats.mapping_error:
        msg_0 = "mapping failed: the selected gate library cannot cover this network; the store is unchanged"
        error = CommandError(msg_0)
        error.stats = stats_to_dict(stats)
        raise error
    if not get_name(mapped):
        set_name(mapped, get_name(network))
    session.networks.add(mapped)
    statistics = stats_to_dict(stats)
    if args.verbose:
        session.console.print(render_table(statistics))
    return {"network": describe(mapped), "stats": statistics}
