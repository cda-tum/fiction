# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Input and output commands: reading and writing files, and creating truth tables."""

from __future__ import annotations

import math
from pathlib import Path
from typing import TYPE_CHECKING

from aigverse.io import read_ascii_aiger_into_aig, read_pla_into_aig

from mnt.pyfiction import (
    aig_network,
    dynamic_truth_table,
    inml_layout,
    mol_qca_layout,
    qca_layout,
    read_aig_network,
    read_cartesian_fgl_layout,
    read_fqca_layout,
    read_hexagonal_fgl_layout,
    read_mig_network,
    read_shifted_cartesian_fgl_layout,
    read_sqd_layout,
    read_technology_network,
    read_xag_network,
    sidb_layout,
    write_aiger,
    write_blif,
    write_dot_layout,
    write_dot_network,
    write_fgl_layout,
    write_fqca_layout,
    write_fqca_layout_params,
    write_qca_layout,
    write_qca_layout_params,
    write_qcc_layout,
    write_qll_layout,
    write_sqd_layout,
    write_verilog,
)
from mnt.pyfiction.cli.errors import CommandError
from mnt.pyfiction.cli.registry import Category, command
from mnt.pyfiction.cli.stores import CellEntry, describe

from .general import write_svg
from .logic import from_aigverse

if TYPE_CHECKING:
    import argparse

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

NETWORK_SUFFIXES = {".v", ".aig", ".blif"}
"""The file suffixes the network readers accept."""

FGL_READERS = {
    "cartesian": read_cartesian_fgl_layout,
    "shifted_cartesian": read_shifted_cartesian_fgl_layout,
    "hexagonal": read_hexagonal_fgl_layout,
}
"""The ``--topology`` names and the FGL readers that produce them."""


def _read_arguments(parser: Parser) -> None:
    parser.add_argument("path", type=Path, help="the file, or a directory of network files")
    parser.add_argument(
        "--type",
        choices=list(NETWORK_READERS),
        default="tec",
        help="the network type to read .v, .aig, .aag, .blif, and .pla files as (default: tec)",
    )
    parser.add_argument(
        "--topology",
        choices=list(FGL_READERS),
        default="cartesian",
        help="the layout topology to read .fgl files as (default: cartesian)",
    )
    parser.add_argument("--sort", action="store_true", help="read a directory's networks in order of gate count")


@command("read", Category.IO, _read_arguments)
def read(session: Session, args: argparse.Namespace) -> Result:
    """Read a logic network or a layout from a file, chosen by its suffix.

    Networks: .v (Verilog), .aig and .aag (AIGER), .blif, and .pla; the type defaults to a
    technology network. Gate-level layouts: .fgl. Cell-level layouts: .sqd (SiDB) and .fqca (QCA).
    A directory reads every network file in it.
    """
    path: Path = args.path
    if path.is_dir():
        files = sorted(file for file in path.iterdir() if file.suffix in NETWORK_SUFFIXES)
        if not files:
            msg = f"no network files in '{path}'"
            raise CommandError(msg)
        networks = [_read_network(file, args.type) for file in files]
        if args.sort:
            networks.sort(key=lambda network: network.num_gates())
        for network in networks:
            session.networks.add(network)
        return {"networks": [describe(network) for network in networks]}

    if not path.is_file():
        msg = f"no such file: '{path}'"
        raise CommandError(msg)

    suffix = path.suffix.lower()
    if suffix in NETWORK_SUFFIXES or suffix in {".aag", ".pla"}:
        network = _read_network(path, args.type, session=session)
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
        entry = CellEntry(read_fqca_layout(str(path), path.stem))
        session.cell_layouts.add(entry)
        return {"cell_layout": describe(entry)}
    msg = f"cannot read '{path.suffix}' files"
    raise CommandError(msg)


def _read_network(path: Path, network_type: str, session: Session | None = None) -> Network:
    suffix = path.suffix.lower()
    if suffix in {".aag", ".pla"}:
        if session is None or network_type not in {"aig", "tec"}:
            msg = f"'{suffix}' files are read as AIGs; use --type aig or --type tec"
            raise CommandError(msg)
        aig = read_ascii_aiger_into_aig(str(path)) if suffix == ".aag" else read_pla_into_aig(str(path))
        network = from_aigverse(session, aig, path.stem)
        return session.as_technology_network(network) if network_type == "tec" else network
    if suffix == ".blif" and network_type != "tec":
        msg = "BLIF files are read as technology networks only; drop --type"
        raise CommandError(msg)
    return NETWORK_READERS[network_type](str(path))


def _write_arguments(parser: Parser) -> None:
    parser.add_argument("file", type=Path, help="the output file; its suffix selects the format")
    parser.add_argument(
        "-n", "--network", action="store_true", help="write the active network to a .dot file instead of the layout"
    )
    parser.add_argument(
        "--via-layers",
        action="store_true",
        default=None,
        help="add inter-layer via cells to .qca and .fqca files (.qca default: on, .fqca default: off)",
    )
    parser.add_argument("--no-via-layers", dest="via_layers", action="store_false", help="omit the via cells")
    parser.add_argument("--simple", action="store_true", help="draw .svg QCA cells without dots and clock numbers")


@command("write", Category.IO, _write_arguments)
def write(session: Session, args: argparse.Namespace) -> Result:
    """Write the active network or layout to a file, in the format its suffix names.

    Networks: .v, .blif, .aig (AIGs only), .dot (with -n). Gate-level layouts: .fgl, .dot.
    Cell-level layouts: .qca (QCADesigner), .fqca (QCA-STACK), .qcc and .qll (ToPoliNano and
    MagCAD), .sqd (SiQAD), .svg.
    """
    path: Path = args.file
    suffix = path.suffix.lower()
    if suffix in {".v", ".blif", ".aig"} or (suffix == ".dot" and args.network):
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
            write_dot_network(network, str(path))
    elif suffix in {".fgl", ".dot"}:
        layout = session.gate_layouts.current()
        if suffix == ".fgl":
            write_fgl_layout(layout, str(path))
        else:
            write_dot_layout(layout, str(path))
    elif suffix in {".qca", ".fqca", ".qcc", ".qll", ".sqd", ".svg"}:
        _write_cell_layout(session, path, suffix, via_layers=args.via_layers, simple=args.simple)
    else:
        msg = f"cannot write '{path.suffix}' files"
        raise CommandError(msg)
    session.info(f"wrote {path}")
    return {"file": str(path)}


def _write_cell_layout(session: Session, path: Path, suffix: str, *, via_layers: bool | None, simple: bool) -> None:
    entry = session.cell_layouts.current()
    layout = entry.layout
    if suffix == ".svg":
        write_svg(entry, path, simple=simple)
        return
    if suffix == ".sqd":
        _require(layout, sidb_layout, suffix)
        write_sqd_layout(layout, str(path))
        return
    if suffix == ".qcc":
        _require(layout, inml_layout, suffix)
        write_qcc_layout(layout, str(path))
        return
    if suffix == ".qll":
        _require(layout, (inml_layout, qca_layout, mol_qca_layout), suffix)
        write_qll_layout(layout, str(path))
        return
    _require(layout, qca_layout, suffix)
    if suffix == ".qca":
        qca_params = write_qca_layout_params()
        qca_params.create_inter_layer_via_cells = via_layers if via_layers is not None else True
        write_qca_layout(layout, str(path), qca_params)
    else:
        fqca_params = write_fqca_layout_params()
        fqca_params.create_inter_layer_via_cells = via_layers if via_layers is not None else False
        write_fqca_layout(layout, str(path), fqca_params)


def _require(layout: object, types: type | tuple[type, ...], suffix: str) -> None:
    if not isinstance(layout, types):
        names = " or ".join(t.__name__ for t in (types if isinstance(types, tuple) else (types,)))
        msg = f"'{suffix}' files take a {names}, not a {type(layout).__name__}"
        raise CommandError(msg)


def _tt_arguments(parser: Parser) -> None:
    source = parser.add_mutually_exclusive_group(required=True)
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
        num_vars = 1 if len(digits) == 1 else _log2(bits, what="hex digits times four")
        table = dynamic_truth_table(num_vars)
        table.create_from_hex_string(digits)
        return table
    table = dynamic_truth_table(_log2(len(text), what="bits"))
    table.create_from_binary_string(text)
    return table


SMALLEST_TRUTH_TABLE_BITS = 2
"""A truth table of one variable has two bits; that is the smallest string ``tt`` accepts."""


def _log2(count: int, *, what: str) -> int:
    if count < SMALLEST_TRUTH_TABLE_BITS or count & (count - 1):
        msg = f"the number of {what} must be a power of two, got {count}"
        raise CommandError(msg)
    return int(math.log2(count))
