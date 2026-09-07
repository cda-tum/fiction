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
    inml_layout,
    qca_layout,
    shifted_cartesian_gate_layout,
    sidb_layout,
)
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import Category, command
from mnt.pyfiction.cli.stores import TECHNOLOGIES, TOPOLOGIES, CellEntry, describe

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

CELL_DIMENSIONS_NM = {
    qca_layout: (18.0, 18.0, 2.0, 2.0),
    inml_layout: (50.0, 100.0, 10.0, 25.0),
    sidb_layout: (0.0, 0.0, 0.384, 0.384),
}
"""Default cell width, height, horizontal spacing, and vertical spacing in nm per technology."""


def _cell_arguments(parser: Parser) -> None:
    parser.add_argument(
        "-l",
        "--library",
        type=str.lower,
        choices=list(GATE_LIBRARIES),
        default="qca-one",
        help="the gate library (default: qca-one)",
    )


@command("cell", Category.TECHNOLOGY, _cell_arguments)
def cell(session: Session, args: argparse.Namespace) -> Result:
    """Compile the active gate-level layout into a cell-level layout with a gate library.

    qca-one and sim7-mol take Cartesian layouts, topolinano takes shifted Cartesian ones (exact
    --topolinano), and bestagon takes hexagonal ones (hex, or exact --topology hexagonal -s row).
    """
    layout = session.gate_layouts.current()
    needed, apply = GATE_LIBRARIES[args.library]
    if not isinstance(layout, needed):
        msg = f"{args.library} needs a {TOPOLOGIES[needed]} layout; the active layout is {TOPOLOGIES[type(layout)]}"
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

    SiDB area uses the layout lattice. Unset cell dimensions use QCADesigner's (QCA) or NMLSim's (iNML) values.
    """
    layout = session.cell_layouts.current().layout
    if isinstance(layout, sidb_layout):
        if any(value is not None for value in (args.width, args.height, args.hspace, args.vspace)):
            msg = "SiDB area uses the layout lattice; cell dimension overrides do not apply"
            raise CommandError(msg)
        result = area(layout)
        session.info(f"area: {result:.2f} nm²")
        return {"area_nm2": result}
    defaults = CELL_DIMENSIONS_NM.get(type(layout))
    if defaults is None:
        msg = f"no area model for {TECHNOLOGIES[type(layout)]} layouts"
        raise CommandError(msg)
    width, height, hspace, vspace = (
        given if given is not None else default
        for given, default in zip((args.width, args.height, args.hspace, args.vspace), defaults, strict=True)
    )
    result = area(layout, width, height, hspace, vspace)
    session.info(f"area: {result:.2f} nm²")
    return {
        "area_nm2": result,
        "cell_width_nm": width,
        "cell_height_nm": height,
        "hspace_nm": hspace,
        "vspace_nm": vspace,
    }
