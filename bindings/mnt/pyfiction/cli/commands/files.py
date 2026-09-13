# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Input and output commands: reading and writing files, and creating truth tables."""

from __future__ import annotations

import argparse
import math
from dataclasses import dataclass
from pathlib import Path
from typing import TYPE_CHECKING

from aigverse.io import read_ascii_aiger_into_aig, read_pla_into_aig

from mnt.pyfiction import (
    aig_network,
    convert_network,
    dynamic_truth_table,
    inml_layout,
    mol_qca_layout,
    network_target,
    qca_layout,
    read_aig_network,
    read_cartesian_fgl_layout,
    read_even_column_cartesian_fgl_layout,
    read_even_column_hex_fgl_layout,
    read_even_row_cartesian_fgl_layout,
    read_fqca_layout,
    read_hexagonal_fgl_layout,
    read_mig_network,
    read_odd_column_hex_fgl_layout,
    read_odd_row_cartesian_fgl_layout,
    read_odd_row_hex_fgl_layout,
    read_shifted_cartesian_fgl_layout,
    read_sqd_layout,
    read_stacked_fqca_layout,
    read_technology_network,
    read_xag_network,
    set_name,
    sidb_layout,
    stacked_qca_layout,
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
from mnt.pyfiction.cli.aigverse_bridge import from_aigverse
from mnt.pyfiction.cli.drawing import drawing_flags, write_dot, write_svg
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import Category, command, store_flags
from mnt.pyfiction.cli.stores import CellEntry, describe, element_name

if TYPE_CHECKING:
    from aigverse.networks import Aig

    from mnt.pyfiction.cli.registry import Parser, Result
    from mnt.pyfiction.cli.session import Session
    from mnt.pyfiction.cli.stores import Network

NETWORK_READERS = {
    "aig": read_aig_network,
    "xag": read_xag_network,
    "mig": read_mig_network,
    "tec": read_technology_network,
}
"""The ``--type`` names and the readers that produce them."""


@dataclass(frozen=True)
class FileFormat:
    """Reader family, writable stores, and supported cell types for one format."""

    reader: str | None
    stores: tuple[str, ...]
    cell_types: tuple[type, ...] = ()


FORMATS = {
    ".v": FileFormat("native", ("network",)),
    ".aig": FileFormat("native", ("network",)),
    ".blif": FileFormat("native", ("network",)),
    ".aag": FileFormat("aigverse", ()),
    ".pla": FileFormat("aigverse", ()),
    ".fgl": FileFormat("gate", ("gate_layout",)),
    ".fqca": FileFormat("cell", ("cell_layout",), (qca_layout, stacked_qca_layout)),
    ".sqd": FileFormat("cell", ("cell_layout",), (sidb_layout,)),
    ".qca": FileFormat(None, ("cell_layout",), (qca_layout, stacked_qca_layout)),
    ".qcc": FileFormat(None, ("cell_layout",), (inml_layout,)),
    ".qll": FileFormat(None, ("cell_layout",), (qca_layout, stacked_qca_layout, mol_qca_layout, inml_layout)),
    ".svg": FileFormat(None, ("cell_layout",), (qca_layout, mol_qca_layout, sidb_layout)),
    ".dot": FileFormat(None, ("gate_layout", "network")),
}
"""Shared format contract for dispatch, validation, help choices, and completion."""

NATIVE_NETWORK_SUFFIXES = {suffix for suffix, spec in FORMATS.items() if spec.reader == "native"}
"""The native network reader's suffixes."""

AIGVERSE_NETWORK_SUFFIXES = {suffix for suffix, spec in FORMATS.items() if spec.reader == "aigverse"}
"""Formats read as AIGs through aigverse."""

AIGVERSE_TARGETS = {"xag": network_target.XAG, "mig": network_target.MIG, "tec": network_target.TEC}
"""What ``--type`` converts an AIG read through ``aigverse`` into."""

NETWORK_SUFFIXES = NATIVE_NETWORK_SUFFIXES | AIGVERSE_NETWORK_SUFFIXES
"""Every file suffix ``read`` recognizes as a logic network."""

FGL_READERS = {
    "cartesian": read_cartesian_fgl_layout,
    "odd_column_cartesian": read_shifted_cartesian_fgl_layout,
    "even_row_hex": read_hexagonal_fgl_layout,
    "odd_row_cartesian": read_odd_row_cartesian_fgl_layout,
    "even_row_cartesian": read_even_row_cartesian_fgl_layout,
    "even_column_cartesian": read_even_column_cartesian_fgl_layout,
    "odd_row_hex": read_odd_row_hex_fgl_layout,
    "odd_column_hex": read_odd_column_hex_fgl_layout,
    "even_column_hex": read_even_column_hex_fgl_layout,
    "shifted_cartesian": read_shifted_cartesian_fgl_layout,
    "hexagonal": read_hexagonal_fgl_layout,
}
"""The ``--topology`` names and the FGL readers that produce them."""

NETWORK_WRITE_SUFFIXES = {suffix for suffix, spec in FORMATS.items() if spec.stores == ("network",)}
"""Unambiguous network writer formats."""

GATE_LAYOUT_WRITE_SUFFIXES = {suffix for suffix, spec in FORMATS.items() if spec.stores == ("gate_layout",)}
"""Unambiguous gate layout writer formats."""

CELL_LAYOUT_WRITE_SUFFIXES = {suffix for suffix, spec in FORMATS.items() if spec.stores == ("cell_layout",)}
"""Cell layout writer formats."""

WRITE_SUFFIXES = {suffix for suffix, spec in FORMATS.items() if spec.stores}
"""Formats with a writer, used in --format help and completion."""


def _type_argument(parser: Parser) -> None:
    """Add the network type option shared by every network reader.

    Args:
        parser: The command's parser.
    """
    parser.add_argument(
        "--type",
        choices=list(NETWORK_READERS),
        default="tec",
        help="the network type to build",
    )


def _topology_argument(parser: Parser) -> None:
    """Add the FGL topology option.

    Args:
        parser: The command's parser.
    """
    parser.add_argument(
        "--topology",
        choices=list(FGL_READERS),
        metavar="TOPOLOGY",
        default="cartesian",
        help="FGL layout topology; choices: %(choices)s",
    )


def _read_arguments(parser: Parser) -> None:
    parser.add_argument(
        "-F",
        "--format",
        choices=sorted(suffix[1:] for suffix, spec in FORMATS.items() if spec.reader),
        help="select the reader independently of the filename",
    )
    parser.add_argument("path", type=Path, help="the file to read")
    _type_argument(parser)
    _topology_argument(parser)


@command("read", Category.IO, _read_arguments)
def read(session: Session, args: argparse.Namespace) -> Result:
    """Read a logic network or a layout from a file, chosen by its suffix.

    Networks: .v (Verilog), .aig and .aag (AIGER), .blif, and .pla; the type defaults to a
    technology network. Gate-level layouts: .fgl. Cell-level layouts: .sqd (SiDB) and .fqca (QCA).
    The format-specific commands, such as 'read_verilog', take the same file without guessing.
    """
    path: Path = args.path
    if path.is_dir():
        msg = f"'{path}' is a directory; give one file to read"
        raise CommandError(msg)
    if not path.is_file():
        msg = f"no such file: '{path}'"
        raise CommandError(msg)

    suffix = f".{args.format}" if args.format else path.suffix.lower()
    if suffix in NETWORK_SUFFIXES:
        network = _read_network(session, path, args.type, args.format)
        session.networks.add(network)
        return {"network": describe(network)}
    if suffix == ".fgl":
        layout = FGL_READERS[args.topology](str(path), path.stem)
        session.gate_layouts.add(layout)
        return {"gate_layout": describe(layout)}
    if suffix == ".sqd":
        entry = CellEntry(read_sqd_layout(str(path), path.stem))
        session.cell_layouts.add(entry)
        return {"cell_layout": describe(entry)}
    if suffix == ".fqca":
        stacked = read_stacked_fqca_layout(str(path), path.stem)
        # The SVG renderer supports unsigned coordinates, whose layer index is one bit.
        entry = CellEntry(read_fqca_layout(str(path), path.stem) if stacked.z() <= 1 else stacked)
        session.cell_layouts.add(entry)
        return {"cell_layout": describe(entry)}
    msg = f"cannot read '{path.suffix}' files"
    raise CommandError(msg)


def _network_reader(name: str, suffixes: tuple[str, ...], description: str) -> None:
    """Register one format-specific network reader.

    Args:
        name: The command name, such as ``read_verilog``.
        suffixes: The suffixes the reader accepts; the first names the format in messages.
        description: The command's help text.
    """

    def arguments(parser: Parser) -> None:
        parser.add_argument("path", type=Path, help="the file to read")
        _type_argument(parser)

    def run(session: Session, args: argparse.Namespace) -> Result:
        path = _existing_file(args.path, suffixes)
        network = _read_network(session, path, args.type, path.suffix.lower()[1:])
        session.networks.add(network)
        return {"network": describe(network)}

    run.__doc__ = description
    command(name, Category.IO, arguments)(run)


def _existing_file(path: Path, suffixes: tuple[str, ...]) -> Path:
    """Check that a path is a readable file of an accepted format.

    Args:
        path: The path the user gave.
        suffixes: The suffixes the reader accepts.

    Returns:
        The path itself.

    Raises:
        CommandError: The path is missing, a directory, or of another format.
    """
    if path.is_dir():
        msg = f"'{path}' is a directory; give one file to read"
        raise CommandError(msg)
    if not path.is_file():
        msg = f"no such file: '{path}'"
        raise CommandError(msg)
    if path.suffix.lower() not in suffixes:
        accepted = " or ".join(suffixes)
        msg = f"'{path.suffix}' is not {accepted}; use 'read' to choose the reader by suffix"
        raise CommandError(msg)
    return path


_network_reader(
    "read_verilog",
    (".v",),
    """Read a logic network from a Verilog file.

    The network type follows --type and defaults to a technology network.""",
)

_network_reader(
    "read_aiger",
    (".aig", ".aag"),
    """Read a logic network from a binary (.aig) or ASCII (.aag) AIGER file.

    ASCII AIGER is read through aigverse as an AIG and converted to --type from there.""",
)

_network_reader(
    "read_blif",
    (".blif",),
    """Read a logic network from a BLIF file.

    BLIF is read as a technology network only; --type tec is the only accepted value.""",
)

_network_reader(
    "read_pla",
    (".pla",),
    """Read a logic network from a PLA file.

    The file is read through aigverse as an AIG and converted to --type from there; .ilb and .ob
    interface labels are preserved.""",
)


def _read_fgl_arguments(parser: Parser) -> None:
    parser.add_argument("path", type=Path, help="the file to read")
    _topology_argument(parser)


@command("read_fgl", Category.IO, _read_fgl_arguments)
def read_fgl(session: Session, args: argparse.Namespace) -> Result:
    """Read a gate-level layout from an FGL file.

    The file does not record its topology, so --topology selects the reader; it defaults to
    cartesian.
    """
    path = _existing_file(args.path, (".fgl",))
    layout = FGL_READERS[args.topology](str(path), path.stem)
    session.gate_layouts.add(layout)
    return {"gate_layout": describe(layout)}


def _read_path_argument(parser: Parser) -> None:
    parser.add_argument("path", type=Path, help="the file to read")


@command("read_sqd", Category.IO, _read_path_argument)
def read_sqd(session: Session, args: argparse.Namespace) -> Result:
    """Read an SiDB cell-level layout from a SiQAD file."""
    path = _existing_file(args.path, (".sqd",))
    entry = CellEntry(read_sqd_layout(str(path), path.stem))
    session.cell_layouts.add(entry)
    return {"cell_layout": describe(entry)}


@command("read_fqca", Category.IO, _read_path_argument)
def read_fqca(session: Session, args: argparse.Namespace) -> Result:
    """Read a QCA cell-level layout from a QCA-STACK file.

    A file of more than two layers is kept stacked, which the SVG renderer cannot draw.
    """
    path = _existing_file(args.path, (".fqca",))
    stacked = read_stacked_fqca_layout(str(path), path.stem)
    # The SVG renderer supports unsigned coordinates, whose layer index is one bit.
    entry = CellEntry(read_fqca_layout(str(path), path.stem) if stacked.z() <= 1 else stacked)
    session.cell_layouts.add(entry)
    return {"cell_layout": describe(entry)}


def _read_network(session: Session, path: Path, network_type: str, file_format: str | None = None) -> Network:
    suffix = f".{file_format}" if file_format else path.suffix.lower()
    if suffix in AIGVERSE_NETWORK_SUFFIXES:
        # aigverse reads these two formats, and it only produces AIGs; --type converts from there
        aig = read_ascii_aiger_into_aig(str(path)) if suffix == ".aag" else _read_pla(session, path)
        network = from_aigverse(session, aig, path.stem)
        if suffix == ".pla":
            _restore_pla_labels(network, path)
        if network_type == "aig":
            return network
        converted = convert_network(network, AIGVERSE_TARGETS[network_type])
        set_name(converted, path.stem)
        return converted
    if suffix == ".blif" and network_type != "tec":
        msg = "BLIF files are read as technology networks only; drop --type"
        raise CommandError(msg)
    return NETWORK_READERS[network_type](str(path), format=suffix[1:])


def _read_pla(session: Session, path: Path) -> Aig:
    """Read a PLA after separating labels unsupported by the AIG reader.

    Args:
        session: Owns the temporary transfer path.
        path: PLA source, including optional input and output labels.

    Returns:
        The network; interface labels are restored by the caller.
    """
    lines = path.read_text(encoding="utf-8").splitlines()
    temporary = session.temp_file(".pla")
    try:
        temporary.write_text(
            "\n".join(line for line in lines if line.split()[:1] not in ([".ilb"], [".ob"])) + "\n",
            encoding="utf-8",
        )
        return read_pla_into_aig(str(temporary))
    finally:
        temporary.unlink(missing_ok=True)


def _restore_pla_labels(network: Network, path: Path) -> None:
    """Restore PLA interface labels omitted by the AIG reader.

    Args:
        network: Network with the declaration's input and output order.
        path: PLA source.

    Raises:
        CommandError: A label list disagrees with the interface size.
    """
    for line in path.read_text(encoding="utf-8").splitlines():
        labels = line.split()
        if labels and labels[0] == ".ilb":
            if len(labels) - 1 != network.num_pis():
                msg = "PLA input labels do not match the input count"
                raise CommandError(msg)
            for signal, label in zip(network.pis(), labels[1:], strict=True):
                network.set_name(signal, label)
        elif labels and labels[0] == ".ob":
            if len(labels) - 1 != network.num_pos():
                msg = "PLA output labels do not match the output count"
                raise CommandError(msg)
            for index, label in enumerate(labels[1:]):
                network.set_output_name(index, label)


def _write_arguments(parser: Parser) -> None:
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


@command("write", Category.IO, _write_arguments)
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
    _validate_writer(args, suffix)
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


def _validate_writer(args: argparse.Namespace, suffix: str) -> None:
    """Reject store conflicts and options the selected writer cannot honor.

    Args:
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
    if args.simple and suffix != ".svg":
        msg_0 = "--simple applies only to SVG output"
        raise CommandError(msg_0)
    if (args.indexes or args.clock_colors) and suffix != ".dot":
        msg_0 = "drawing indices and clock colors apply only to DOT output"
        raise CommandError(msg_0)
    if args.clock_colors and expected == "network":
        msg_0 = "--clock-colors requires a gate-level layout"
        raise CommandError(msg_0)


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
    if not isinstance(layout, types):
        names = " or ".join(t.__name__ for t in (types if isinstance(types, tuple) else (types,)))
        msg = f"'{suffix}' files take a {names}, not a {type(layout).__name__}"
        raise CommandError(msg)


def _tt_arguments(parser: Parser) -> None:
    source = parser.exclusive_group(required=True)
    source.add_argument("-t", "--table", metavar="BITS", help="a binary string, or a hex string with a 0x prefix")
    source.add_argument(
        "-e",
        "--expression",
        metavar="EXPR",
        help="a Boolean expression over a..p: !E, (E..E) for AND, {E..E} for OR, [E..E] for XOR, <EEE> for MAJ",
    )
    source.add_argument("-r", "--random", type=int, metavar="VARS", help="a random function of VARS variables")


@command("tt", Category.IO, _tt_arguments)
def tt(session: Session, args: argparse.Namespace) -> Result:
    """Create a truth table from a bit string, a hex string, an expression, or at random.

    The number of variables follows from the input: a binary string of 2^n bits, a hex string of
    2^n / 4 digits, or the largest variable in an expression. Bit strings are most significant bit
    first, so 'tt -t 1000' is the two-input AND.
    """
    if args.random is not None:
        table = dynamic_truth_table(args.random)
        table.create_random()
    elif args.expression is not None:
        variables = [ord(char) - ord("a") + 1 for char in args.expression if "a" <= char <= "p"]
        table = dynamic_truth_table(max(variables, default=0))
        table.create_from_expression(args.expression)
    else:
        table = _table_from_string(args.table)
    session.truth_tables.add(table)
    return {"truth_table": describe(table)}


def _table_from_string(text: str) -> dynamic_truth_table:
    if text.lower().startswith("0x"):
        digits = text[2:]
        bits = len(digits) * 4
        num_vars = _log2(bits, what="hex digits times four")
        table = dynamic_truth_table(num_vars)
        table.create_from_hex_string(digits)
        return table
    table = dynamic_truth_table(_log2(len(text), what="bits"))
    table.create_from_binary_string(text)
    return table


SMALLEST_TRUTH_TABLE_BITS = 1
"""A truth table of one variable has two bits; that is the smallest string ``tt`` accepts."""


def _log2(count: int, *, what: str) -> int:
    if count < SMALLEST_TRUTH_TABLE_BITS or count & (count - 1):
        msg = f"the number of {what} must be a power of two, got {count}"
        raise CommandError(msg)
    return int(math.log2(count))
