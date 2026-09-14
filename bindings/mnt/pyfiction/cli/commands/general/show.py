# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The show command."""

from __future__ import annotations

import shutil
from pathlib import Path
from typing import TYPE_CHECKING

from mnt.pyfiction.cli.drawing import drawing_flags, open_viewer, render_dot, write_dot, write_svg
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import (
    Category,
    command,
    one_store,
    store_flags,
)

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.registry import Parser, Result
    from mnt.pyfiction.cli.session import Session


def _show_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    store_flags(parser, "network", "gate_layout", "cell_layout")
    parser.add_argument("-o", "--output", type=Path, metavar="FILE", help="write here instead of a temporary file")
    parser.add_argument("--silent", action="store_true", help="only write the file, do not open a viewer")
    parser.add_argument(
        "-p",
        "--program",
        metavar="COMMAND",
        help="open the file with this command instead of the platform's viewer; '{}' becomes the file",
    )
    parser.add_argument("--delete", action="store_true", help="remove the temporary file when the session ends")
    drawing_flags(parser)


@command("show", Category.GENERAL, _show_arguments)
def show(session: Session, args: argparse.Namespace) -> Result:
    """Draw the active network or layout and open it in the platform's viewer.

    Networks and gate-level layouts become SVG images through optional Graphviz; .dot keeps raw DOT.
    Cell-level layouts become SVG images. A
    simulated SiDB layout is drawn with its ground state charges. The viewer returns at once and
    reads the file afterwards, so a temporary file is retained on disk; --delete removes it when the session closes.
    """
    name = one_store(args, "network", "gate_layout", "cell_layout")
    if args.output is not None:
        path: Path = args.output
    else:
        suffix = ".svg" if name == "cell_layout" or shutil.which("dot") else ".dot"
        path = session.viewer_file(suffix, delete=args.delete)
    suffix = path.suffix.lower()
    if name == "cell_layout":
        if suffix != ".svg":
            msg = "cell drawings require an .svg output filename"
            raise CommandError(msg)
        write_svg(session.cell_layouts.current(), path, simple=args.simple)
    else:
        if suffix not in {".svg", ".dot"}:
            msg = "network and gate-layout drawings require .svg or .dot"
            raise CommandError(msg)
        element = session.networks.current() if name == "network" else session.gate_layouts.current()
        dot_path = path
        if suffix == ".svg":
            dot_path = (
                session.viewer_file(".dot", delete=args.delete) if args.output is None else path.with_suffix(".dot")
            )
        write_dot(element, dot_path, network=name == "network", indexes=args.indexes, clock_colors=args.clock_colors)
        if suffix == ".svg":
            render_dot(dot_path, path)
        elif args.output is None and not shutil.which("dot"):
            session.output(f"Install Graphviz for SVG viewing; DOT retained at '{path}'")
    session.output(f"wrote {path}")
    if not args.silent and (suffix != ".dot" or args.program or args.output is not None):
        open_viewer(path, args.program)
    return {"file": str(path)}
