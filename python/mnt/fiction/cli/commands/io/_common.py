# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Shared command helpers."""

from __future__ import annotations

from pathlib import Path
from typing import TYPE_CHECKING

from aigverse.io import read_ascii_aiger_into_aig, read_pla_into_aig

from mnt.fiction.cli.aigverse_bridge import from_aigverse
from mnt.fiction.cli.errors import CommandError
from mnt.fiction.cli.stores import CellEntry, describe
from mnt.fiction.cli.topologies import FGL_READERS
from mnt.pyfiction.networks import set_name
from mnt.pyfiction.networks.io import read_aig_network, read_mig_network, read_technology_network, read_xag_network
from mnt.pyfiction.sidb.io import read_sqd_layout
from mnt.pyfiction.synthesis import convert_network, network_target

if TYPE_CHECKING:
    from collections.abc import Callable

    from mnt.fiction.cli.parsing import Parser
    from mnt.fiction.cli.registry import Result
    from mnt.fiction.cli.session import Session
    from mnt.fiction.cli.stores import Network


NETWORK_READERS: dict[str, Callable[..., Network]] = {
    "aig": read_aig_network,
    "xag": read_xag_network,
    "mig": read_mig_network,
    "tec": read_technology_network,
}
"""The ``--type`` names and the readers that produce them."""


READ_SUFFIXES = (".v", ".aig", ".blif", ".aag", ".pla", ".fgl", ".sqd")
"""Formats accepted by the generic reader."""


AIGVERSE_NETWORK_SUFFIXES = {".aag", ".pla"}
"""Formats read as AIGs through aigverse."""


AIGVERSE_TARGETS = {"xag": network_target.XAG, "mig": network_target.MIG, "tec": network_target.TEC}
"""What ``--type`` converts an AIG read through ``aigverse`` into."""


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
    if suffixes and path.suffix.lower() not in suffixes:
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
        network = (
            from_aigverse(session, read_ascii_aiger_into_aig(str(path)), path.stem)
            if suffix == ".aag"
            else _read_pla(session, path)
        )
        if network_type == "aig":
            return network
        converted = convert_network(network, AIGVERSE_TARGETS[network_type])
        set_name(converted, path.stem)
        return converted
    if suffix == ".blif" and network_type != "tec":
        msg = "BLIF files are read as technology networks only; drop --type"
        raise CommandError(msg)
    return NETWORK_READERS[network_type](str(path), format=suffix[1:])


def _read_pla(session: Session, path: Path) -> Network:
    """Read a PLA after separating labels unsupported by the AIG reader.

    Args:
        session: Owns the temporary transfer path.
        path: PLA source, including optional input and output labels.

    Returns:
        The network with its interface labels.
    """
    lines = path.read_text(encoding="utf-8").splitlines()
    temporary = session.temp_file(".pla")
    try:
        temporary.write_text(
            "\n".join(line for line in lines if line.split()[:1] not in ([".ilb"], [".ob"])) + "\n",
            encoding="utf-8",
        )
        network = from_aigverse(session, read_pla_into_aig(str(temporary)), path.stem)
        _restore_pla_labels(network, lines)
        return network
    finally:
        temporary.unlink(missing_ok=True)


def _restore_pla_labels(network: Network, lines: list[str]) -> None:
    """Restore PLA interface labels omitted by the AIG reader.

    Args:
        network: Network with the declaration's input and output order.
        lines: Lines of the PLA source.

    Raises:
        CommandError: A label list disagrees with the interface size.
    """
    for line in lines:
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


def read_file(
    session: Session,
    path: Path,
    *,
    file_format: str | None = None,
    network_type: str = "tec",
    topology: str = "cartesian",
    suffixes: tuple[str, ...] = (),
) -> Result:
    """Load one file and append its network or layout to the appropriate store.

    Args:
        session: The destination stores.
        path: The source file.
        file_format: An explicit format, or None to use the suffix.
        network_type: The type produced by network readers.
        topology: The topology produced by the FGL reader.
        suffixes: Accepted filename suffixes for a format-specific command.

    Returns:
        The description of the imported element.

    Raises:
        CommandError: The file or selected format is invalid.
    """
    _existing_file(path, suffixes)
    suffix = f".{file_format}" if file_format else path.suffix.lower()
    if suffix in {".v", ".aig", ".blif", ".aag", ".pla"}:
        network = _read_network(session, path, network_type, file_format)
        session.networks.add(network)
        return {"network": describe(network)}
    if suffix == ".fgl":
        layout = FGL_READERS[topology](str(path), path.stem)
        session.gate_layouts.add(layout)
        return {"gate_layout": describe(layout)}
    if suffix == ".sqd":
        entry = CellEntry(read_sqd_layout(str(path), path.stem))
    else:
        msg = f"cannot read '{path.suffix}' files"
        raise CommandError(msg)
    session.cell_layouts.add(entry)
    return {"cell_layout": describe(entry)}
