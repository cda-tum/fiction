# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""General commands: help, version, quitting, scripts, and the store commands."""

from __future__ import annotations

from pathlib import Path
from typing import TYPE_CHECKING

from rich.markup import escape
from rich.table import Table

from mnt.pyfiction import (
    __compiled_date__,
    __compiled_time__,
    __repo__,
    __version__,
    print_sidb_layout,
    sidb_layout,
)
from mnt.pyfiction.cli.drawing import drawing_flags, open_viewer, write_dot, write_svg
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import REGISTRY, STORE_FLAGS, Category, command, one_store, selected_stores, store_flags
from mnt.pyfiction.cli.render import table as render_table
from mnt.pyfiction.cli.stores import Store, describe, ground_state, one_line

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
        # a command registered under an alias appears in the registry twice; list it once
        commands = [cmd for name, cmd in REGISTRY.items() if cmd.category is category and cmd.name == name]
        if not commands:
            continue
        table.add_row(f"[bold]{category.value}[/]", "")
        for cmd in commands:
            names = ", ".join((cmd.name, *cmd.aliases))
            table.add_row(f"  {names}", escape(cmd.summary))
    session.console.print(table)
    return None


@command("version", Category.GENERAL)
def version(session: Session, args: argparse.Namespace) -> Result:
    """Print the fiction version and build date."""
    del args
    session.info(f"{__version__} ({__repo__}), compiled {__compiled_date__} {__compiled_time__}")
    return {"version": __version__, "compiled": f"{__compiled_date__} {__compiled_time__}"}


@command("quit", Category.GENERAL, aliases=("exit",))
def quit_command(session: Session, args: argparse.Namespace) -> Result:
    """Leave the shell; also spelled 'exit'.

    In a script or behind -c, the commands after it are not run.
    """
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


def _store_arguments(parser: Parser) -> None:
    store_flags(parser)
    parser.add_argument("--pop", action="store_true", help="remove the active element of the selected stores")


@command("store", Category.GENERAL, _store_arguments)
def store(session: Session, args: argparse.Namespace) -> Result:
    """List the elements of the selected stores, or of all stores.

    The active element, the one commands work on, is marked with '*'. With --pop, the active
    element is removed instead and the one before it becomes active.
    """
    stores = stores_of(session)
    names = selected_stores(args) or list(stores)
    if args.pop:
        removed: dict[str, object] = {}
        for name in names:
            element = stores[name].pop()
            removed[name] = describe(element)
            session.info(f"removed {one_line(describe(element))}")
        return removed

    listed: dict[str, object] = {}
    for name in names:
        current = stores[name]
        session.console.print(f"[bold]{STORE_FLAGS[name][2]}[/]")
        descriptions = [describe(element) for element in current]
        if not descriptions:
            session.info("  (empty)")
        for index, description in enumerate(descriptions):
            marker = "*" if index == current.active else " "
            session.info(f"{marker} {index}: {one_line(description)}")
        listed[name] = descriptions
    return listed


def _current_arguments(parser: Parser) -> None:
    store_flags(parser)
    parser.add_argument("index", type=int, help="the element's number, as 'store' lists it")


@command("current", Category.GENERAL, _current_arguments)
def current(session: Session, args: argparse.Namespace) -> Result:
    """Make an element of a store the active one."""
    name = one_store(args)
    selected = stores_of(session)[name]
    selected.select(args.index)
    return {name: describe(selected.current())}


def _ps_arguments(parser: Parser) -> None:
    store_flags(parser)
    parser.add_argument("--all", action="store_true", help="describe every element, not only the active one")


@command("ps", Category.GENERAL, _ps_arguments)
def ps(session: Session, args: argparse.Namespace) -> Result:
    """Print the statistics of the active element of a store."""
    stores = stores_of(session)
    if not args.all:
        name = one_store(args)
        description = describe(stores[name].current())
        session.console.print(render_table(description))
        return {name: description}

    listed: dict[str, object] = {}
    for name in selected_stores(args) or list(stores):
        current_store = stores[name]
        descriptions = [describe(element) for element in current_store]
        for index, description in enumerate(descriptions):
            marker = "*" if index == current_store.active else " "
            session.console.print(f"[bold]{STORE_FLAGS[name][2]}[/] {marker}{index}")
            session.console.print(render_table(description))
        listed[name] = descriptions
    return listed


def _print_arguments(parser: Parser) -> None:
    store_flags(parser)


@command("print", Category.GENERAL, _print_arguments)
def print_command(session: Session, args: argparse.Namespace) -> Result:
    """Print the active truth table, gate-level layout, or cell-level layout.

    A simulated SiDB layout is drawn once, with its ground state charges in place of the dots.
    Networks have no textual form; use 'show -n' to draw one.
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
    if not isinstance(entry.layout, sidb_layout):
        session.info(repr(entry.layout))
        return None
    state = ground_state(entry)
    # one picture: the charge symbols replace the dots rather than being drawn a second time. Only
    # the color tells a negative charge from a positive one, so it is kept whenever a terminal reads
    # the output and dropped when it is redirected into a file.
    picture = print_sidb_layout(entry.layout, state, lat_color=session.console.is_terminal)
    session.console.print(picture.rstrip(), markup=False, highlight=False)
    if state is not None:
        session.info(f"ground state energy: {state.energy():.6f} eV")
    return None


def _show_arguments(parser: Parser) -> None:
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

    Networks and gate-level layouts become Graphviz DOT files, cell-level layouts SVG files. A
    simulated SiDB layout is drawn with its ground state charges. The viewer returns at once and
    reads the file afterwards, so a temporary file is kept until the process ends; --delete asks
    for it back when the session closes.
    """
    name = one_store(args, "network", "gate_layout", "cell_layout")
    suffix = ".svg" if name == "cell_layout" else ".dot"
    path: Path = args.output if args.output is not None else session.viewer_file(suffix, delete=args.delete)
    if name == "cell_layout":
        write_svg(session.cell_layouts.current(), path, simple=args.simple)
    else:
        element = session.networks.current() if name == "network" else session.gate_layouts.current()
        write_dot(element, path, network=name == "network", indexes=args.indexes, clock_colors=args.clock_colors)
    session.info(f"wrote {path}")
    if not args.silent:
        open_viewer(path, args.program)
    return {"file": str(path)}
