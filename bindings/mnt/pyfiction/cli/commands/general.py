# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""General commands: help, version, quitting, scripts, and the store commands."""

from __future__ import annotations

import shutil
import sys
from contextlib import nullcontext
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
from mnt.pyfiction.cli.drawing import drawing_flags, open_viewer, render_dot, write_dot, write_svg
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import (
    REGISTRY,
    STORE_FLAGS,
    Category,
    command,
    one_store,
    selected_stores,
    store_flags,
    unavailable_reason,
)
from mnt.pyfiction.cli.render import summary as render_summary
from mnt.pyfiction.cli.stores import Store, describe, ground_state, one_line

if TYPE_CHECKING:
    import argparse

    from mnt.pyfiction.cli.registry import Parser, Result
    from mnt.pyfiction.cli.session import Session


MAX_NAME_WIDTH = 32
"""The widest a name column gets before it is shortened; long names must not push the counts away."""


def _size_of(description: dict[str, object]) -> str:
    """Return the size of an element: its extent for a layout, its node count for a network.

    Args:
        description: A description from :func:`~mnt.pyfiction.cli.stores.describe`.

    Returns:
        The size as one short string.
    """
    size = description.get("size")
    if isinstance(size, dict):
        extent = " x ".join(str(size[axis]) for axis in ("x", "y", "z") if axis in size)
        if extent:
            return extent
    return ", ".join(f"{description[key]} {key}" for key in ("gates", "cells", "dots", "vars") if key in description)


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
    parser.add_argument("--all", action="store_true", help="list command descriptions; page in interactive terminals")
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

    pager = (
        session.console.pager() if args.all and session.console.is_terminal and sys.stdin.isatty() else nullcontext()
    )
    with pager:
        session.console.print(_command_grid(described=args.all))
        session.output("\nExample: read circuit.v; ortho; check; cell; write circuit.qca", style="note")
        session.output("Use help COMMAND for options and restrictions; help --all for descriptions.", style="note")
        for name in ("exact", "clustercomplete"):
            if reason := unavailable_reason(name):
                session.output(f"{name}: {reason}", style="note")
    return None


NAME_COLUMNS = 4
"""How many command names one row of the ``help`` grid holds."""


def _command_grid(*, described: bool) -> Table:
    """Lay every category out in one grid, so the columns align across the whole listing.

    Args:
        described: List each command with its summary instead of packing names into columns.

    Returns:
        The grid, ready to print on a console.
    """
    grid = Table(box=None, show_header=False, padding=(0, 1), pad_edge=False)
    grid.add_column("", no_wrap=True)
    if described:
        grid.add_column("", overflow="fold")
    else:
        for _ in range(NAME_COLUMNS - 1):
            grid.add_column("", no_wrap=True)
    blanks = [""] * (len(grid.columns) - 1)
    for category in Category:
        # a command registered under an alias appears in the registry twice; list it once
        commands = [cmd for name, cmd in REGISTRY.items() if cmd.category is category and cmd.name == name]
        if not commands:
            continue
        if grid.row_count:
            grid.add_row("", *blanks)
        grid.add_row(f"[bold]{category.value}[/]", *blanks)
        if described:
            for cmd in commands:
                grid.add_row(f"  {cmd.name}", escape(cmd.summary))
        else:
            _add_name_rows(grid, [name for cmd in commands for name in (cmd.name, *cmd.aliases)])
    return grid


def _add_name_rows(grid: Table, names: list[str]) -> None:
    """Append the command names of one category, wrapped across the grid's columns.

    Args:
        grid: The grid being built.
        names: The command names, aliases included.
    """
    for start in range(0, len(names), NAME_COLUMNS):
        row = names[start : start + NAME_COLUMNS]
        cells = [*row, *[""] * (NAME_COLUMNS - len(row))]
        cells[0] = f"  {cells[0]}"
        grid.add_row(*cells)


@command("version", Category.GENERAL)
def version(session: Session, args: argparse.Namespace) -> Result:
    """Print the fiction version and build date."""
    del args
    session.output(f"{__version__} ({__repo__}), compiled {__compiled_date__} {__compiled_time__}", style="result")
    return {"version": __version__, "compiled": f"{__compiled_date__} {__compiled_time__}"}


@command("quit", Category.GENERAL)
def quit_command(session: Session, args: argparse.Namespace) -> Result:
    """Leave the shell.

    In a script or behind -c, the commands after it are not run.
    """
    del args
    session.running = False
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
    element of every selected store is removed instead and the one before it becomes active.
    --pop needs the stores spelled out, so that it cannot empty one the user did not mean.
    """
    stores = stores_of(session)
    if args.pop:
        return _pop(session, stores, selected_stores(args))

    listed: dict[str, object] = {}
    for name in selected_stores(args) or list(stores):
        current = stores[name]
        descriptions = [describe(element) for element in current]
        empty = " (empty)" if not descriptions else ""
        session.console.print(f"[bold]{STORE_FLAGS[name][2]}[/]{empty}")
        table = Table(box=None, padding=(0, 1))
        table.add_column("", width=1)
        table.add_column("Index", justify="right", no_wrap=True)
        table.add_column("Name", overflow="ellipsis", max_width=MAX_NAME_WIDTH, no_wrap=True)
        table.add_column("Type", no_wrap=True)
        table.add_column("I/O", no_wrap=True)
        table.add_column("Size", no_wrap=True)
        for index, description in enumerate(descriptions):
            marker = "*" if index == current.active else ""
            label = " / ".join(
                str(description[key]) for key in ("type", "technology", "topology", "lattice") if description.get(key)
            )
            if description.get("simulation"):
                label += " / simulated"
            io = f"{description['inputs']}/{description['outputs']}" if "inputs" in description else ""
            table.add_row(
                marker,
                str(index + 1),
                escape(str(description.get("name", "—"))),
                escape(label),
                io,
                escape(_size_of(description)),
            )
        if descriptions:
            session.console.print(table)
        listed[name] = descriptions
    return listed


def _pop(session: Session, stores: dict[str, Store], names: list[str]) -> Result:  # type: ignore[type-arg]
    """Remove the active element of every named store.

    Every store is checked before any is touched, so a command that cannot run leaves the session
    exactly as it found it.

    Args:
        session: The session, for the message.
        stores: The stores by their flag name.
        names: The stores the flags selected.

    Returns:
        The removed elements by store.

    Raises:
        CommandError: When no store was selected, or one of them is empty.
    """
    if not names:
        flags = ", ".join(short for short, _, _ in STORE_FLAGS.values())
        msg = f"select the stores to remove from: {flags}"
        raise CommandError(msg)
    empty = [stores[name].kind for name in names if stores[name].active is None]
    if empty:
        msg = f"no {' and no '.join(empty)} in store"
        raise CommandError(msg)
    removed: dict[str, object] = {}
    for name in names:
        description = describe(stores[name].pop())
        removed[name] = description
        session.output(f"removed {one_line(description)}")
    return removed


def _current_arguments(parser: Parser) -> None:
    store_flags(parser)
    parser.add_argument("position", type=int, help="the element's position, counting from 1 as 'store' lists it")


@command("current", Category.GENERAL, _current_arguments)
def current(session: Session, args: argparse.Namespace) -> Result:
    """Make an element of a store the active one."""
    name = one_store(args)
    selected = stores_of(session)[name]
    selected.select(args.position)
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
        session.console.print(render_summary(description))
        return {name: description}

    listed: dict[str, object] = {}
    for name in selected_stores(args) or list(stores):
        current_store = stores[name]
        descriptions = [describe(element) for element in current_store]
        for index, description in enumerate(descriptions):
            marker = "*" if index == current_store.active else " "
            session.console.print(f"[bold]{STORE_FLAGS[name][2]}[/] {marker}{index + 1}")
            session.console.print(render_summary(description))
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
        session.output(f"hex: {tt.to_hex()}\nbin: {tt.to_binary()}")
        return None
    if name == "gate_layout":
        session.output(repr(session.gate_layouts.current()))
        return None
    entry = session.cell_layouts.current()
    if not isinstance(entry.layout, sidb_layout):
        session.output(repr(entry.layout))
        return None
    state = ground_state(entry)
    picture = print_sidb_layout(
        entry.layout, state, lat_color=session.console.is_terminal and not session.console.no_color
    )
    session.output(picture.rstrip())
    if state is not None:
        session.output(f"Ground state energy: {state.energy():.6f} eV", style="result")
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
