# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""General commands: help, version, quitting, scripts, and the store commands."""

from __future__ import annotations

import webbrowser
from pathlib import Path
from typing import TYPE_CHECKING

from rich.markup import escape
from rich.table import Table

from mnt.pyfiction import (
    __compiled_date__,
    __compiled_time__,
    __repo__,
    __version__,
    mol_qca_layout,
    qca_layout,
    sidb_layout,
    write_dot_layout,
    write_dot_network,
    write_mol_qca_layout_svg,
    write_qca_layout_svg,
    write_qca_layout_svg_params,
    write_sidb_layout_svg,
)
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import (
    REGISTRY,
    STORE_FLAGS,
    Category,
    command,
    one_store,
    selected_stores,
    store_flags,
)
from mnt.pyfiction.cli.stores import CellEntry, Store, describe, ground_state, one_line

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.registry import Parser, Result
    from mnt.pyfiction.cli.session import Session


def stores_of(session: Session) -> dict[str, Store]:  # type: ignore[type-arg]
    """Map the store flag names onto the session's stores.

    Args:
        session: The session.

    Returns:
        The stores by their :data:`~mnt.pyfiction.cli.registry.STORE_FLAGS` key.
    """
    return {
        "truth_table": session.truth_tables,
        "network": session.networks,
        "gate_layout": session.gate_layouts,
        "cell_layout": session.cell_layouts,
    }


def _help_arguments(parser: Parser) -> None:
    parser.add_argument("name", nargs="?", help="a command to show the full help of")


@command("help", Category.GENERAL, _help_arguments)
def help_command(session: Session, args: argparse.Namespace) -> Result:
    """List the commands, or show one command's options.

    Every command accepts -h for its full help.
    """
    if args.name is not None:
        cmd = REGISTRY.get(args.name)
        if cmd is None:
            msg = f"unknown command '{args.name}'"
            raise CommandError(msg)
        session.console.print(cmd.parser.format_help(), markup=False, highlight=False)
        return None

    table = Table(box=None, show_header=False, padding=(0, 2))
    for category in Category:
        commands = [cmd for cmd in REGISTRY.values() if cmd.category is category]
        if not commands:
            continue
        table.add_row(f"[bold]{category.value}[/]", "")
        for cmd in commands:
            table.add_row(f"  {cmd.name}", escape(cmd.summary))
    session.console.print(table)
    return None


@command("version", Category.GENERAL)
def version(session: Session, args: argparse.Namespace) -> Result:
    """Print the fiction version and build date."""
    del args
    session.info(f"{__version__} ({__repo__}), compiled {__compiled_date__} {__compiled_time__}")
    return {"version": __version__, "compiled": f"{__compiled_date__} {__compiled_time__}"}


@command("quit", Category.GENERAL)
@command("exit", Category.GENERAL)
def quit_command(session: Session, args: argparse.Namespace) -> Result:
    """Leave the shell."""
    del args
    session.running = False
    return None


def _source_arguments(parser: Parser) -> None:
    parser.add_argument("file", type=Path, help="the script file, one command line per line")


@command("source", Category.GENERAL, _source_arguments)
def source(session: Session, args: argparse.Namespace) -> Result:
    """Run the commands of a script file.

    Scripts may source other scripts. The first failing command stops the script.
    """
    if not session.run_script(args.file):
        msg = f"script '{args.file}' stopped at a failing command"
        raise CommandError(msg)
    return None


def _all_stores(parser: Parser) -> None:
    store_flags(parser)


@command("clear", Category.GENERAL, _all_stores)
def clear(session: Session, args: argparse.Namespace) -> Result:
    """Remove the elements of the selected stores, or of all stores."""
    stores = stores_of(session)
    for name in selected_stores(args) or list(stores):
        stores[name].clear()
    return None


@command("store", Category.GENERAL, _all_stores)
def store(session: Session, args: argparse.Namespace) -> Result:
    """List the elements of the selected stores, or of all stores.

    The active element, the one commands work on, is marked with '*'.
    """
    stores = stores_of(session)
    listed: dict[str, object] = {}
    for name in selected_stores(args) or list(stores):
        current = stores[name]
        session.console.print(f"[bold]{STORE_FLAGS[name][2]}[/]")
        if not current.items:
            session.info("  (empty)")
        for index, element in enumerate(current):
            marker = "*" if index == current.active else " "
            session.info(f"{marker} {index}: {one_line(describe(element))}")
        listed[name] = [describe(element) for element in current]
    return listed


def _current_arguments(parser: Parser) -> None:
    store_flags(parser)
    parser.add_argument("index", type=int, help="the element's number, as 'store' lists it")


@command("current", Category.GENERAL, _current_arguments)
def current(session: Session, args: argparse.Namespace) -> Result:
    """Make an element of a store the active one."""
    name = one_store(args)
    stores_of(session)[name].select(args.index)
    return {name: describe(stores_of(session)[name].current())}


@command("ps", Category.GENERAL, _all_stores)
def ps(session: Session, args: argparse.Namespace) -> Result:
    """Print the statistics of the active element of a store."""
    name = one_store(args)
    description = describe(stores_of(session)[name].current())
    table = Table(box=None, show_header=False, padding=(0, 2))
    for key, value in _flatten(description):
        table.add_row(f"[bold]{key}[/]", escape(str(value)))
    session.console.print(table)
    return {name: description}


def _flatten(description: dict[str, object], prefix: str = "") -> list[tuple[str, object]]:
    rows: list[tuple[str, object]] = []
    for key, value in description.items():
        if isinstance(value, dict):
            rows.extend(_flatten(value, f"{prefix}{key} "))
        else:
            rows.append((f"{prefix}{key}", value))
    return rows


def _print_arguments(parser: Parser) -> None:
    store_flags(parser)


@command("print", Category.GENERAL, _print_arguments)
def print_command(session: Session, args: argparse.Namespace) -> Result:
    """Print the active truth table, gate-level layout, or cell-level layout.

    A simulated SiDB layout also prints its ground state charge configuration. Networks have no
    textual form; use 'show -n' to draw one.
    """
    if args.network:
        msg = "networks have no textual form; use 'show -n' to draw the active network"
        raise CommandError(msg)
    name = one_store(args, "truth_table", "gate_layout", "cell_layout")
    if name == "truth_table":
        tt = session.truth_tables.current()
        session.info(f"hex: {tt.to_hex()}\nbin: {tt.to_binary()}")
        return None
    if name == "gate_layout":
        session.info(repr(session.gate_layouts.current()))
        return None
    entry = session.cell_layouts.current()
    session.info(repr(entry.layout))
    state = ground_state(entry)
    if state is not None:
        session.info(f"ground state: {state!r} ({state.energy():.6f} eV)")
    return None


def _show_arguments(parser: Parser) -> None:
    store_flags(parser, "network", "gate_layout", "cell_layout")
    parser.add_argument("-o", "--output", type=Path, metavar="FILE", help="write here instead of a temporary file")
    parser.add_argument("--silent", action="store_true", help="only write the file, do not open a viewer")
    parser.add_argument("--simple", action="store_true", help="draw QCA cells without dots and clock numbers")


@command("show", Category.GENERAL, _show_arguments)
def show(session: Session, args: argparse.Namespace) -> Result:
    """Draw the active network or layout and open it in the default viewer.

    Networks and gate-level layouts become Graphviz DOT files, cell-level layouts SVG files. A
    simulated SiDB layout is drawn with its ground state charges.
    """
    name = one_store(args, "network", "gate_layout", "cell_layout")
    suffix = ".svg" if name == "cell_layout" else ".dot"
    path: Path = args.output if args.output is not None else session.temp_file(suffix)
    if name == "network":
        write_dot_network(session.networks.current(), str(path))
    elif name == "gate_layout":
        write_dot_layout(session.gate_layouts.current(), str(path))
    else:
        write_svg(session.cell_layouts.current(), path, simple=args.simple)
    session.info(f"wrote {path}")
    if not args.silent:
        webbrowser.open(path.resolve().as_uri())
    return {"file": str(path)}


def write_svg(entry: CellEntry, path: Path, *, simple: bool) -> None:
    """Draw a cell-level layout as an SVG file.

    Args:
        entry: The store element; a simulated SiDB layout is drawn with its ground state charges.
        path: The output file.
        simple: Draw QCA cells without dots and clock numbers.

    Raises:
        CommandError: For iNML layouts, which have no SVG drawer.
    """
    layout = entry.layout
    if isinstance(layout, qca_layout | mol_qca_layout):
        params = write_qca_layout_svg_params()
        params.simple = simple
        if isinstance(layout, qca_layout):
            write_qca_layout_svg(layout, str(path), params)
        else:
            write_mol_qca_layout_svg(layout, str(path), params)
    elif isinstance(layout, sidb_layout):
        state = ground_state(entry)
        if state is not None:
            write_sidb_layout_svg(layout, state, str(path))
        else:
            write_sidb_layout_svg(layout, str(path))
    else:
        msg = f"no SVG drawer for {type(layout).__name__} layouts"
        raise CommandError(msg)
