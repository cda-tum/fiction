# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The cell command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction import (
    apply_bestagon_library,
    apply_qca_one_library,
    apply_sim7_mol_library,
    apply_topolinano_library,
    cartesian_gate_layout,
    hexagonal_gate_layout,
    shifted_cartesian_gate_layout,
)
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import Category, command
from mnt.pyfiction.cli.stores import CellEntry, describe
from mnt.pyfiction.cli.topologies import TOPOLOGIES

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.parsing import Parser
    from mnt.pyfiction.cli.registry import Result
    from mnt.pyfiction.cli.session import Session


GATE_LIBRARIES = {
    "qca-one": (cartesian_gate_layout, apply_qca_one_library),
    "sim7-mol": (cartesian_gate_layout, apply_sim7_mol_library),
    "topolinano": (shifted_cartesian_gate_layout, apply_topolinano_library),
    "bestagon": (hexagonal_gate_layout, apply_bestagon_library),
}
"""The gate libraries, each with the gate-level layout topology it maps."""


def _library_key(name: str) -> str:
    """Normalize a gate library name, so that 'QCA ONE', 'qca_one', and 'qcaone' all find qca-one.

    Args:
        name: The name the user typed.

    Returns:
        The name without separators, in lower case.
    """
    return name.lower().replace("-", "").replace("_", "").replace(" ", "")


LIBRARY_ALIASES = {_library_key(name): name for name in GATE_LIBRARIES}
"""Every gate library under its normalized name."""


def _cell_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument(
        "-l",
        "--library",
        default="qca-one",
        metavar="LIBRARY",
        type=lambda name: LIBRARY_ALIASES.get(_library_key(name), name),
        choices=list(GATE_LIBRARIES),
        help=f"the gate library: {', '.join(GATE_LIBRARIES)}; hyphens, underscores, spaces, and case are ignored",
    )


@command(
    "cell",
    Category.TECHNOLOGY,
    _cell_arguments,
    inputs="Active gate-level layout.",
    example="generate mux -b 1; ortho; cell --library qca-one",
)
def cell(session: Session, args: argparse.Namespace) -> Result:
    """Compile the active gate-level layout into a cell-level layout with a gate library.

    qca-one and sim7-mol take Cartesian layouts, topolinano takes shifted Cartesian ones (exact
    --topolinano), and bestagon takes hexagonal ones (hex, or exact --topology hexagonal -s row).
    """
    library = LIBRARY_ALIASES.get(_library_key(args.library))
    if library is None:
        msg = f"'{args.library}' is not a gate library; choose from {', '.join(GATE_LIBRARIES)}"
        raise CommandError(msg)
    layout = session.gate_layouts.current()
    needed, apply = GATE_LIBRARIES[library]
    if not isinstance(layout, needed):
        msg = f"{library} needs a {TOPOLOGIES[needed]} layout; the active layout is {TOPOLOGIES[type(layout)]}"
        raise CommandError(msg)
    entry = CellEntry(apply(layout))
    session.cell_layouts.add(entry)
    return {"cell_layout": describe(entry)}
