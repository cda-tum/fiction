# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The area command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.pyfiction import (
    area,
)
from mnt.pyfiction.cli.registry import Category, command

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.registry import Parser, Result
    from mnt.pyfiction.cli.session import Session


AREA_OVERRIDES = {"width": "-x", "height": "-y", "hspace": "--hspace", "vspace": "--vspace"}
"""The keyword arguments of the ``area`` binding and the flags that set them."""


def _area_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
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
