# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The write_dot command."""

from __future__ import annotations

from typing import TYPE_CHECKING

from mnt.fiction.cli.drawing import validate_drawing_options, write_dot
from mnt.fiction.cli.errors import CommandError
from mnt.fiction.cli.registry import Category, command, store_flags

from ._write import output_argument, output_path, written

if TYPE_CHECKING:
    import argparse

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session


def _write_dot_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    output_argument(parser)
    store_flags(parser, "network", "gate_layout")
    parser.add_argument("--indexes", action="store_true", help="label nodes with their indices")
    parser.add_argument("--clock-colors", action="store_true", help="color tiles by clock number instead of gate type")


@command(
    "write_dot",
    Category.IO,
    _write_dot_arguments,
    inputs="Active gate-level layout by default; -n selects the network.",
    example="generate mux -b 1; ortho; write_dot output.dot",
    progress=True,
)
def write_dot_command(session: Session, args: argparse.Namespace) -> Result:
    """Write the active gate-level layout or network as Graphviz DOT.

    Without a filename, use the active element's name and ``.dot``.
    """
    if args.network and args.gate_layout:
        msg = "select either the network or gate-level layout store"
        raise CommandError(msg)
    validate_drawing_options(args, dot=True, gate_layout=not args.network, qca_svg=False)
    element = session.networks.current() if args.network else session.gate_layouts.current()
    path = output_path(element, args.file, ".dot")
    write_dot(
        element,
        path,
        indexes=args.indexes,
        clock_colors=args.clock_colors,
        on_progress=session.report_progress,
    )
    return written(session, path)
