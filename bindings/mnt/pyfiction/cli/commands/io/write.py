# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""The write command."""

from __future__ import annotations

import argparse
from pathlib import Path
from typing import TYPE_CHECKING

from mnt.pyfiction import (
    aig_network,
    mol_qca_layout,
    qca_layout,
    write_aiger,
    write_blif,
    write_fgl_layout,
    write_fqca_layout,
    write_fqca_layout_params,
    write_qca_layout,
    write_qca_layout_params,
    write_qcc_layout,
    write_qcc_layout_params,
    write_qll_layout,
    write_sqd_layout,
    write_verilog,
)
from mnt.pyfiction.cli.drawing import drawing_flags, validate_drawing_options, write_dot, write_svg
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import Category, command, store_flags
from mnt.pyfiction.cli.stores import element_name

if TYPE_CHECKING:
    from mnt.pyfiction.cli.parsing import Parser
    from mnt.pyfiction.cli.registry import Result
    from mnt.pyfiction.cli.session import Session
from ._common import FORMATS

NETWORK_WRITE_SUFFIXES = {suffix for suffix, spec in FORMATS.items() if spec.stores == ("network",)}
"""Unambiguous network writer formats."""


GATE_LAYOUT_WRITE_SUFFIXES = {suffix for suffix, spec in FORMATS.items() if spec.stores == ("gate_layout",)}
"""Unambiguous gate layout writer formats."""


WRITE_SUFFIXES = {suffix for suffix, spec in FORMATS.items() if spec.stores}
"""Formats with a writer, used in --format help and completion."""


def _write_arguments(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument(
        "file",
        type=Path,
        nargs="?",
        help="the output file; its suffix selects the format. Defaults to the active element's name plus --format",
    )
    parser.add_argument(
        "-F",
        "--format",
        choices=sorted(suffix.lstrip(".") for suffix in WRITE_SUFFIXES),
        help="the format, when no file is given or its suffix is to be overridden",
    )
    store_flags(parser, "network", "gate_layout", "cell_layout")
    parser.add_argument(
        "--via-layers",
        action=argparse.BooleanOptionalAction,
        default=None,
        help="add inter-layer via cells to .qca and .fqca files (.qca default: on, .fqca default: off)",
    )
    parser.add_argument(
        "--component-name", action="store_true", help="use the .qcc file name as the component identifier"
    )
    drawing_flags(parser)


@command(
    "write",
    Category.IO,
    _write_arguments,
    inputs="Store elements selected by the flags below.",
    example="generate mux -b 1; write mux.v",
)
def write(session: Session, args: argparse.Namespace) -> Result:
    """Write the active network or layout to a file, in the format its suffix names.

    Networks: .v, .blif, .aig (AIGs only), .dot (with -n). Gate-level layouts: .fgl, .dot (with -g,
    the default). Cell-level layouts: .qca (QCADesigner), .fqca (QCA-STACK), .qcc and .qll
    (ToPoliNano and MagCAD), .sqd (SiQAD), .svg. Without a file, the active element's name and
    --format make one in the current directory.
    """
    suffix = f".{args.format}" if args.format is not None else (args.file.suffix.lower() if args.file else "")
    if suffix not in WRITE_SUFFIXES:
        msg = f"cannot write '{suffix}' files" if suffix else "give a file with a known suffix, or --format"
        raise CommandError(msg)
    _validate_writer(session, args, suffix)
    path = _output_path(session, args, suffix)

    if suffix in NETWORK_WRITE_SUFFIXES or (suffix == ".dot" and args.network):
        network = session.networks.current()
        if suffix == ".v":
            write_verilog(network, str(path))
        elif suffix == ".blif":
            write_blif(network, str(path))
        elif suffix == ".aig":
            if not isinstance(network, aig_network):
                msg = "only an AIG can be written as AIGER; read the network with --type aig"
                raise CommandError(msg)
            write_aiger(network, str(path))
        else:
            write_dot(network, path, network=True, indexes=args.indexes, clock_colors=args.clock_colors)
    elif suffix in GATE_LAYOUT_WRITE_SUFFIXES or suffix == ".dot":
        layout = session.gate_layouts.current()
        if suffix == ".fgl":
            write_fgl_layout(layout, str(path))
        else:
            write_dot(layout, path, network=False, indexes=args.indexes, clock_colors=args.clock_colors)
    else:
        _write_cell_layout(session, path, suffix, args)
    session.info(f"wrote {path}")
    return {"file": str(path)}


def _validate_writer(session: Session, args: argparse.Namespace, suffix: str) -> None:
    """Reject store conflicts and options the selected writer cannot honor.

    Args:
        session: The stores containing the output element.
        args: Writer options.
        suffix: Validated format suffix.

    Raises:
        CommandError: Store selection or a format-specific option conflicts.
    """
    selected = [name for name in ("network", "gate_layout", "cell_layout") if getattr(args, name)]
    allowed = FORMATS[suffix].stores
    expected = selected[0] if len(selected) == 1 else allowed[0]
    if len(selected) > 1 or expected not in allowed:
        msg = f"{suffix} output conflicts with the selected store"
        raise CommandError(msg)
    if args.via_layers is not None and suffix not in {".qca", ".fqca"}:
        msg_0 = "--via-layers applies only to QCA and FQCA output"
        raise CommandError(msg_0)
    if args.component_name and suffix != ".qcc":
        msg_0 = "--component-name applies only to QCC output"
        raise CommandError(msg_0)
    validate_drawing_options(
        args,
        dot=suffix == ".dot",
        gate_layout=expected == "gate_layout",
        qca_svg=suffix == ".svg" and isinstance(session.cell_layouts.current().layout, qca_layout | mol_qca_layout),
    )


def _output_path(session: Session, args: argparse.Namespace, suffix: str) -> Path:
    """Return where to write, defaulting to the active element's name in the current directory."""
    if args.file is None:
        name = element_name(_target_element(session, suffix, args))
        if not name:
            msg = "the active element has no name; give a file to write to"
            raise CommandError(msg)
        if any(char in name for char in ("/", "\\", ":")) or name in {".", ".."}:
            msg_0 = "the element name contains path components; give an explicit output path"
            raise CommandError(msg_0)
        return Path(f"{name}{suffix}")
    path: Path = args.file
    if path.is_dir():
        msg = f"'{path}' is a directory; give a file to write to"
        raise CommandError(msg)
    return path


def _target_element(session: Session, suffix: str, args: argparse.Namespace) -> object:
    """Return the store element the given format writes, so its name can become the file name."""
    if suffix in NETWORK_WRITE_SUFFIXES or (suffix == ".dot" and args.network):
        return session.networks.current()
    if suffix in GATE_LAYOUT_WRITE_SUFFIXES or suffix == ".dot":
        return session.gate_layouts.current()
    return session.cell_layouts.current()


def _write_cell_layout(session: Session, path: Path, suffix: str, args: argparse.Namespace) -> None:
    """Write the active cell layout with the selected format options."""
    entry = session.cell_layouts.current()
    layout = entry.layout
    _require(layout, FORMATS[suffix].cell_types, suffix)
    if suffix == ".svg":
        write_svg(entry, path, simple=args.simple)
        return
    if suffix == ".sqd":
        write_sqd_layout(layout, str(path))
        return
    if suffix == ".qcc":
        qcc_params = write_qcc_layout_params()
        qcc_params.use_filename_as_component_name = args.component_name
        write_qcc_layout(layout, str(path), qcc_params)
        return
    if suffix == ".qll":
        write_qll_layout(layout, str(path))
        return
    if suffix == ".qca":
        qca_params = write_qca_layout_params()
        qca_params.create_inter_layer_via_cells = args.via_layers if args.via_layers is not None else True
        write_qca_layout(layout, str(path), qca_params)
    else:
        fqca_params = write_fqca_layout_params()
        fqca_params.create_inter_layer_via_cells = args.via_layers if args.via_layers is not None else False
        write_fqca_layout(layout, str(path), fqca_params)


def _require(layout: object, types: type | tuple[type, ...], suffix: str) -> None:
    """Require a layout type supported by the selected writer."""
    if not isinstance(layout, types):
        names = " or ".join(t.__name__ for t in (types if isinstance(types, tuple) else (types,)))
        msg = f"'{suffix}' files take a {names}, not a {type(layout).__name__}"
        raise CommandError(msg)
