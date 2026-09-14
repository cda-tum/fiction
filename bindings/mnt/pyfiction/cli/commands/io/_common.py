# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Shared command helpers."""

from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import TYPE_CHECKING

from aigverse.io import read_ascii_aiger_into_aig, read_pla_into_aig

from mnt.pyfiction import (
    convert_network,
    inml_layout,
    mol_qca_layout,
    network_target,
    qca_layout,
    read_aig_network,
    read_cartesian_fgl_layout,
    read_even_column_cartesian_fgl_layout,
    read_even_column_hex_fgl_layout,
    read_even_row_cartesian_fgl_layout,
    read_hexagonal_fgl_layout,
    read_mig_network,
    read_odd_column_hex_fgl_layout,
    read_odd_row_cartesian_fgl_layout,
    read_odd_row_hex_fgl_layout,
    read_shifted_cartesian_fgl_layout,
    read_technology_network,
    read_xag_network,
    set_name,
    sidb_layout,
    stacked_qca_layout,
)
from mnt.pyfiction.cli.aigverse_bridge import from_aigverse
from mnt.pyfiction.cli.errors import CommandError

if TYPE_CHECKING:
    from aigverse.networks import Aig

    from mnt.pyfiction.cli.registry import Parser
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


AIGVERSE_NETWORK_SUFFIXES = {suffix for suffix, spec in FORMATS.items() if spec.reader == "aigverse"}
"""Formats read as AIGs through aigverse."""


AIGVERSE_TARGETS = {"xag": network_target.XAG, "mig": network_target.MIG, "tec": network_target.TEC}
"""What ``--type`` converts an AIG read through ``aigverse`` into."""


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


def _read_path_argument(parser: Parser) -> None:
    """Add the command's arguments to the parser."""
    parser.add_argument("path", type=Path, help="the file to read")


def _read_network(session: Session, path: Path, network_type: str, file_format: str | None = None) -> Network:
    """Read a network in the requested format and convert it to the selected type."""
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
