# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Drawing a network or a layout, and handing the file to a viewer.

``show``, ``write_dot``, and ``write_svg`` draw the same things, so the dispatch and the flags that steer it live here
once instead of in both command modules.
"""

from __future__ import annotations

import shutil

# the viewer is the platform's own opener or a command the user typed, both started without a shell
import subprocess  # ruff: ignore[suspicious-subprocess-import]
import sys
import tempfile
from pathlib import Path
from typing import TYPE_CHECKING

from mnt.pyfiction.layouts.io import write_dot_layout
from mnt.pyfiction.mol_qca import mol_qca_layout
from mnt.pyfiction.mol_qca.io import write_mol_qca_layout_svg, write_mol_qca_layout_svg_params
from mnt.pyfiction.networks.io import write_dot_network
from mnt.pyfiction.qca import qca_layout
from mnt.pyfiction.qca.io import write_qca_layout_svg, write_qca_layout_svg_params
from mnt.pyfiction.sidb import sidb_layout
from mnt.pyfiction.sidb.io import write_sidb_layout_svg, write_sidb_layout_svg_params

from .errors import CommandError
from .parsing import tokenize
from .session import ProgressCallback, ignore_progress
from .stores import Network, ground_state

if TYPE_CHECKING:
    import argparse

    from .parsing import Parser
    from .stores import CellEntry, GateLayout


def drawing_flags(parser: Parser) -> None:
    """Add the flags that steer a drawing to a parser.

    Args:
        parser: The parser of ``show``.
    """
    drawing = parser.add_argument_group("drawing")
    drawing.add_argument("--simple", action="store_true", help="draw .svg QCA cells without dots and clock numbers")
    drawing.add_argument("--indexes", action="store_true", help="label the .dot nodes with their indices")
    drawing.add_argument(
        "--clock-colors", action="store_true", help="color the .dot tiles by clock number instead of by gate type"
    )


def validate_drawing_options(args: argparse.Namespace, *, dot: bool, gate_layout: bool, qca_svg: bool) -> None:
    """Reject options the selected drawing cannot honor.

    Args:
        args: Drawing options.
        dot: The drawing uses a DOT source, including Graphviz-rendered SVG.
        gate_layout: The source is a gate-level layout.
        qca_svg: The drawing uses a QCA or molecular QCA SVG writer.

    Raises:
        CommandError: A requested drawing option is unsupported.
    """
    for flag, supported, requirement in (
        ("simple", qca_svg, "QCA SVG output"),
        ("indexes", dot, "a DOT drawing"),
        ("clock_colors", dot and gate_layout, "a gate-level DOT drawing"),
    ):
        if getattr(args, flag, False) and not supported:
            msg = f"--{flag.replace('_', '-')} requires {requirement}"
            raise CommandError(msg)


def write_dot(
    element: Network | GateLayout,
    path: Path,
    *,
    indexes: bool,
    clock_colors: bool,
    on_progress: ProgressCallback = ignore_progress,
) -> None:
    """Draw a network or a gate-level layout as a Graphviz DOT file.

    Args:
        element: The network or the gate-level layout.
        path: The output file.
        indexes: Label the nodes with their indices.
        on_progress: Receives completed tile rendering.
        clock_colors: Color the tiles by clock number instead of by gate type; layouts only.
    """
    if isinstance(element, Network):
        write_dot_network(element, str(path), indexes=indexes)
    else:
        write_dot_layout(element, str(path), clock_colors=clock_colors, indexes=indexes, on_progress=on_progress)


def write_svg(entry: CellEntry, path: Path, *, simple: bool, on_progress: ProgressCallback = ignore_progress) -> None:
    """Draw a cell-level layout as an SVG file.

    Args:
        entry: The store element; a simulated SiDB layout is drawn with its ground state charges.
        path: The output file.
        simple: Draw QCA cells without dots and clock numbers.
        on_progress: Receives serialization progress.

    Raises:
        CommandError: For iNML layouts, which have no SVG drawer.
    """
    layout = entry.layout
    if isinstance(layout, qca_layout):
        params = write_qca_layout_svg_params()
        params.simple = simple
        params.on_progress = on_progress
        write_qca_layout_svg(layout, str(path), params)
    elif isinstance(layout, mol_qca_layout):
        mol_params = write_mol_qca_layout_svg_params()
        mol_params.simple = simple
        mol_params.on_progress = on_progress
        write_mol_qca_layout_svg(layout, str(path), mol_params)
    elif isinstance(layout, sidb_layout):
        sidb_params = write_sidb_layout_svg_params()
        sidb_params.on_progress = on_progress
        state = ground_state(entry)
        if state is not None:
            write_sidb_layout_svg(layout, state, str(path), sidb_params)
        else:
            write_sidb_layout_svg(layout, str(path), sidb_params)
    else:
        msg = f"no SVG drawer for {type(layout).__name__} layouts"
        raise CommandError(msg)


def viewer_command(path: Path, program: str | None) -> list[str]:
    """Return the command that opens a file in a viewer.

    Args:
        path: The file to open.
        program: An explicit command; ``{}`` in it is replaced by the file, otherwise the file is
            appended. The platform's own opener is used when this is ``None``.

    Returns:
        The command and its arguments.

    Raises:
        CommandError: An explicit viewer contains multiple executable commands.
    """
    if program is not None:
        commands = tokenize(program)
        if len(commands) != 1:
            msg = "the viewer must be one executable command"
            raise CommandError(msg)
        words = commands[0]
        if any("{}" in word for word in words):
            return [word.replace("{}", str(path)) for word in words]
        return [*words, str(path)]
    if sys.platform == "darwin":
        return ["open", str(path)]
    return ["xdg-open", str(path)]


def open_viewer(path: Path, program: str | None = None) -> None:
    """Open a drawing in a viewer, without waiting for it.

    A Graphviz DOT file is not a web page, so the platform's file association decides what opens it,
    exactly as the C++ shell did.

    Args:
        path: The file to open.
        program: An explicit viewer command; the platform's own opener when ``None``.

    Raises:
        CommandError: When the viewer cannot be started.
    """
    if program is None and sys.platform == "win32":
        # Windows has no opener binary; the shell association is reached through the API
        import os  # ruff: ignore[import-outside-top-level] -- os.startfile exists on Windows alone

        os.startfile(path)  # ruff: ignore[start-process-with-no-shell]
        return
    command = viewer_command(path, program)
    try:
        subprocess.Popen(command, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)  # ruff: ignore[subprocess-without-shell-equals-true]
    except OSError as error:
        msg = f"cannot open '{path}' with '{command[0]}': {error.strerror}"
        raise CommandError(msg) from error


def render_dot(source: Path, destination: Path) -> None:
    """Render DOT into an SVG and replace the destination only after successful rendering.

    Args:
        source: Retained DOT source.
        destination: SVG output path.

    Raises:
        CommandError: Graphviz is unavailable or rejects the drawing.
    """
    executable = shutil.which("dot")
    if executable is None:
        msg = f"Graphviz 'dot' is required for SVG rendering; DOT retained at '{source}'"
        raise CommandError(msg)
    if destination.is_symlink():
        destination = destination.resolve(strict=True)
    if destination.exists() and not destination.is_file():
        msg = f"output path is not a regular file: '{destination}'"
        raise CommandError(msg)
    with tempfile.TemporaryDirectory(prefix=".fiction-", dir=destination.parent) as directory:
        temporary = Path(directory) / destination.name
        result = subprocess.run(  # ruff: ignore[subprocess-without-shell-equals-true] -- pass paths as arguments, never shell code
            [executable, "-Tsvg", str(source), "-o", str(temporary)], capture_output=True, text=True, check=False
        )
        if result.returncode or not temporary.is_file():
            msg = f"Graphviz failed: {result.stderr.strip()}; DOT retained at '{source}'"
            raise CommandError(msg)
        if destination.exists():
            shutil.copymode(destination, temporary)
        temporary.replace(destination)
