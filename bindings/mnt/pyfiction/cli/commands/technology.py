# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Technology commands: gate libraries and physical area."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction import (
    apply_bestagon_library,
    apply_qca_one_library,
    apply_sim7_mol_library,
    apply_topolinano_library,
    area,
    cartesian_gate_layout,
    hexagonal_gate_layout,
    shifted_cartesian_gate_layout,
)
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import Category, command
from mnt.pyfiction.cli.stores import TOPOLOGIES, CellEntry, describe

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.registry import Parser, Result
    from mnt.pyfiction.cli.session import Session

GATE_LIBRARIES = {
    "qca-one": (cartesian_gate_layout, apply_qca_one_library),
    "sim7-mol": (cartesian_gate_layout, apply_sim7_mol_library),
    "topolinano": (shifted_cartesian_gate_layout, apply_topolinano_library),
    "bestagon": (hexagonal_gate_layout, apply_bestagon_library),
}
"""The gate libraries, each with the gate-level layout topology it maps."""

AREA_OVERRIDES = {"width": "-x", "height": "-y", "hspace": "--hspace", "vspace": "--vspace"}
"""The keyword arguments of the ``area`` binding and the flags that set them."""


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
    parser.add_argument(
        "-l",
        "--library",
        default="qca-one",
        metavar="LIBRARY",
        type=lambda name: LIBRARY_ALIASES.get(_library_key(name), name),
        choices=list(GATE_LIBRARIES),
        help=f"the gate library: {', '.join(GATE_LIBRARIES)} (default: qca-one); "
        "hyphens, underscores, spaces, and case are ignored",
    )


@command("cell", Category.TECHNOLOGY, _cell_arguments)
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


def _area_arguments(parser: Parser) -> None:
    parser.add_argument("-x", "--width", type=float, metavar="NM", help="cell width")
    parser.add_argument("-y", "--height", type=float, metavar="NM", help="cell height")
    parser.add_argument("--hspace", type=float, metavar="NM", help="horizontal spacing between cells")
    parser.add_argument("--vspace", type=float, metavar="NM", help="vertical spacing between cells")


@command("area", Category.TECHNOLOGY, _area_arguments)
def area_command(session: Session, args: argparse.Namespace) -> Result:
    """Compute the physical area of the active cell-level layout in nm².

    Every dimension left unset is the technology's own, so 'area' alone is the right answer for QCA,
    molQCA, iNML, and SiDB alike. SiDB area is measured over the layout's bounding box rather than
    over its aspect ratio, but the cell dimensions apply there too.
    """
    layout = session.cell_layouts.current().layout
    # the binding defaults every dimension to the layout's own technology, so only pass overrides
    overrides = {name: getattr(args, name) for name in AREA_OVERRIDES if getattr(args, name) is not None}
    result = area(layout, **overrides)
    session.output(f"Area: {result:.2f} nm²")
    return {"area_nm2": result, **{f"{name}_nm": value for name, value in overrides.items()}}
