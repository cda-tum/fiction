# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of reading networks of every type and writing them back."""

from __future__ import annotations

import errno
import re
from typing import TYPE_CHECKING, Any, cast

import pytest

from mnt.pyfiction.networks import (
    aig_network,
    get_name,
    io,
    mig_network,
    simulate_outputs,
    technology_network,
    xag_network,
)
from mnt.pyfiction.networks.io import (
    read_network,
    write_aiger,
    write_blif,
    write_dot_network,
    write_verilog,
)
from mnt.pyfiction.synthesis import convert_network, network_target

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path


Network = technology_network | aig_network | xag_network | mig_network
NETWORK_TYPES = [technology_network, aig_network, xag_network, mig_network]


@pytest.mark.parametrize("cls", NETWORK_TYPES)
def test_readers(resources_dir: Path, cls: type[technology_network | aig_network | xag_network | mig_network]) -> None:
    network = read_network(str(resources_dir / "mux21.v"), network_type=cls)
    assert type(network) is cls
    assert network.num_pis() == 3
    assert network.num_pos() == 1
    assert network.num_gates() > 0
    assert network.depth() > 0
    assert get_name(network) == "mux21"
    assert [network.get_name(pi) for pi in network.pis()] == ["in0", "in1", "in2"]
    assert [network.get_output_name(index) for index in range(network.num_pos())] == ["out"]
    assert simulate_outputs(network) == [("out", [True, True, False, False, True, False, True, False])]
    assert all(network.fanins(g) for g in network.gates())


@pytest.mark.parametrize(("suffix", "gates"), [("v", 1), ("blif", 4), ("aig", 1)])
def test_tec_reader_preserves_output_drivers(tmp_path: Path, suffix: str, gates: int) -> None:
    """Reading TEC networks keeps shared gate and PI outputs without adding buffers."""
    source = tmp_path / "source.v"
    source.write_text(
        "module top(a,b,y,z,t);\ninput a,b;\noutput y,z,t;\n"
        "assign y = a & b;\nassign z = y;\nassign t = a;\nendmodule\n",
        encoding="utf-8",
    )
    network = read_network(str(source), network_type=aig_network)
    path = tmp_path / f"outputs.{suffix}"
    writer = {"v": write_verilog, "blif": write_blif, "aig": write_aiger}[suffix]
    writer(network, str(path))
    restored = read_network(str(path))
    # BLIF names each output with an explicit buffer record; the reader preserves those records.
    assert restored.num_gates() == gates
    assert restored.num_pos() == 3
    assert restored.depth() == (2 if suffix == "blif" else 1)
    expected = [("y", [True, False, False, False]), ("z", [True, False, False, False]), ("t", [True, False] * 2)]
    assert [restored.get_name(pi) for pi in restored.pis()] == ["a", "b"]
    assert simulate_outputs(network) == expected
    assert simulate_outputs(restored) == expected


@pytest.mark.parametrize("cls", [aig_network, xag_network, mig_network])
def test_reader_rejects_blif_for_specialized_networks(
    tmp_path: Path, cls: type[aig_network | xag_network | mig_network]
) -> None:
    """BLIF input supports the technology network representation only."""
    source = tmp_path / "one.blif"
    source.write_text(".model one\n.inputs a b\n.outputs y\n.names a b y\n11 1\n.end\n", encoding="utf-8")
    with pytest.raises(RuntimeError, match="could not parse"):
        read_network(str(source), network_type=cls)


@pytest.mark.parametrize("target", ["TEC", "AIG", "XAG", "MIG"])
@pytest.mark.parametrize("suffix", ["v", "blif"])
def test_network_writer_round_trip(
    interface_network: technology_network, tmp_path: Path, target: str, suffix: str
) -> None:
    """Network writers retain functions, names, and unused inputs."""
    network = convert_network(interface_network, getattr(network_target, target))
    path = tmp_path / f"out.{suffix}"
    writer = write_verilog if suffix == "v" else write_blif
    path.write_text("replace this file", encoding="utf-8")
    writer(network=network, filename=str(path))
    restored = read_network(str(path))
    assert [restored.get_name(pi) for pi in restored.pis()] == ["apple", "banana", "cherry", "unused"]
    assert simulate_outputs(interface_network) == expected_interface_outputs()
    assert simulate_outputs(restored) == expected_interface_outputs()


def test_aiger_round_trip_keeps_names(interface_network: technology_network, tmp_path: Path) -> None:
    """AIGER preserves interface labels and output functions."""
    aig = convert_network(interface_network, network_target.AIG)
    path = tmp_path / "out.aig"
    write_aiger(aig, str(path))
    back = read_network(str(path), network_type=aig_network)
    assert [back.get_name(pi) for pi in back.pis()] == ["apple", "banana", "cherry", "unused"]
    assert get_name(back) == "out"
    assert simulate_outputs(interface_network) == expected_interface_outputs()
    assert simulate_outputs(back) == expected_interface_outputs()


WRITERS = [(write_verilog, ".v"), (write_blif, ".blif"), (write_aiger, ".aig"), (write_dot_network, ".dot")]


@pytest.mark.parametrize(("writer", "suffix"), WRITERS)
def test_writers_report_unwritable_files(
    resources_dir: Path, tmp_path: Path, writer: Callable[[Any, str], None], suffix: str
) -> None:
    """A file under a directory that does not exist cannot be opened, which the writers report."""
    aig = read_network(str(resources_dir / "mux21.v"), network_type=aig_network)
    target = tmp_path / "missing" / f"out{suffix}"
    with pytest.raises(RuntimeError, match="missing"):
        writer(aig, str(target))
    assert not target.exists()


@pytest.mark.parametrize("selector", [None, "aig", int, aig_network()])
def test_reader_rejects_invalid_selectors_before_io(selector: Any) -> None:
    """Validate the selector before attempting to open a nonexistent file."""
    with pytest.raises(TypeError):
        read_network("missing.v", network_type=selector)


def test_reader_default_and_keyword_only_selector(resources_dir: Path) -> None:
    """The general reader defaults to a technology network."""
    assert type(read_network(str(resources_dir / "mux21.v"))) is technology_network
    with pytest.raises(TypeError):
        cast("Any", read_network)("missing.v", "", aig_network)


@pytest.mark.parametrize("cls", NETWORK_TYPES)
def test_reader_explicit_format_and_directory(
    tmp_path: Path, cls: type[technology_network | aig_network | xag_network | mig_network]
) -> None:
    """Select each representation for an explicit format and a directory."""
    directory = tmp_path / "networks"
    directory.mkdir()
    source = directory / "one.v"
    source.write_text("module top(a,b,y); input a,b; output y; assign y = a & b; endmodule\n", encoding="utf-8")
    expected = [("y", [True, False, False, False])]
    network = read_network(str(directory), network_type=cls)
    assert type(network) is cls
    assert get_name(network) == "one"
    assert [network.get_name(pi) for pi in network.pis()] == ["a", "b"]
    assert simulate_outputs(network) == expected
    renamed = tmp_path / "one.data"
    source.rename(renamed)
    network = read_network(str(renamed), format="V", network_type=cls)
    assert type(network) is cls
    assert simulate_outputs(network) == expected
    uppercase = tmp_path / "one.V"
    renamed.rename(uppercase)
    assert simulate_outputs(read_network(str(uppercase), network_type=cls)) == expected


@pytest.mark.parametrize(
    "name", ["read_technology_network", "read_aig_network", "read_xag_network", "read_mig_network"]
)
def test_reader_specialization_names_are_removed(name: str) -> None:
    """Expose one reader name for every network representation."""
    assert not hasattr(io, name)


def expected_interface_outputs() -> list[tuple[str, list[bool]]]:
    """Evaluate the BLIF fixture's functions from its declared input bits.

    Returns:
        Complete output names and truth tables in declaration order.
    """
    assignments = range(15, -1, -1)
    return [
        ("zero", [False] * 16),
        ("one", [True] * 16),
        ("inverted", [not bool(value & 1) for value in assignments]),
        ("xor", [bool(value & 1) != bool(value & 2) for value in assignments]),
        ("majority", [(value & 7).bit_count() >= 2 for value in assignments]),
        ("lut", [value & 7 in (2, 4, 7) for value in assignments]),
        ("copy", [bool(value & 1) for value in assignments]),
    ]


@pytest.mark.parametrize("whitespace", ["", " \t\r"])
def test_blif_whitespace_comments_and_continuations(tmp_path: Path, whitespace: str) -> None:
    """Blank lines do not alter constants, gates, covers, or continued input declarations."""
    source = tmp_path / "whitespace.blif"
    source.write_text(
        f"{whitespace}\n# before model\n.model whitespace\n{whitespace}\n.inputs a b \\\n"
        f"{whitespace}\n unused\n.outputs zero one inverted xor lut copy\n"
        f"{whitespace}\n.names zero\n{whitespace}\n.names one\n{whitespace}\n1\n"
        f"# between gates\n.names a inverted\n{whitespace}\n0 1\n.names a b xor\n01 1\n"
        f"{whitespace}\n# between cover rows\n10 1\n.names a b \\\n lut\n01 1\n"
        f".names a copy\n1 1\n.end\n{whitespace}\n",
        encoding="utf-8",
    )
    network = read_network(str(source))
    assert [network.get_name(pi) for pi in network.pis()] == ["a", "b", "unused"]
    assert get_name(network) == "whitespace"
    assert network.fanins(network.pos()[4]) == network.pis()[:2]
    assert simulate_outputs(network) == [
        ("zero", [False] * 8),
        ("one", [True] * 8),
        ("inverted", [False, True] * 4),
        ("xor", [False, True, True, False] * 2),
        ("lut", [False, True, False, False] * 2),
        ("copy", [True, False] * 4),
    ]


@pytest.mark.parametrize("cls", NETWORK_TYPES)
@pytest.mark.parametrize("kind", ["missing", "unsupported", "empty-directory", "invalid-verilog"])
def test_reader_reports_unreadable_inputs(tmp_path: Path, cls: type[Network], kind: str) -> None:
    """Missing files, unsupported formats, empty directories, and parse errors raise RuntimeError."""
    source = tmp_path / "source.v"
    if kind == "unsupported":
        source = source.with_suffix(".txt")
        source.write_text("ignored", encoding="utf-8")
    elif kind == "empty-directory":
        source.mkdir()
    elif kind == "invalid-verilog":
        source.write_text("module broken(\n", encoding="utf-8")
    with pytest.raises(RuntimeError, match="could not parse") as error:
        read_network(str(source), network_type=cls)
    assert str(source) in str(error.value)


def test_blif_reports_invalid_declarations_after_blank_lines(tmp_path: Path) -> None:
    """Whitespace normalization preserves parser errors and their source filename."""
    source = tmp_path / "invalid.blif"
    source.write_text("\n.model invalid\n \t\n.unsupported declaration\n\n.end\n", encoding="utf-8")
    with pytest.raises(RuntimeError, match="parsing error") as error:
        read_network(str(source))
    assert str(source) in str(error.value)


@pytest.mark.parametrize("cls", NETWORK_TYPES)
def test_network_dot_graph_and_index_labels(tmp_path: Path, cls: type[Network]) -> None:
    """DOT retains complete graph edges, complemented outputs, gate labels, and colors."""
    source = tmp_path / "and.v"
    source.write_text(
        "module top(a,b,y,z); input a,b; output y,z; assign y = a & b; assign z = ~y; endmodule\n",
        encoding="utf-8",
    )
    network = read_network(str(source), network_type=cls)
    a, b = network.pis()
    gate, inverted = network.pos()
    expected_edges = [(str(a), str(gate), "solid"), (str(b), str(gate), "solid"), (str(gate), "po0", "solid")]
    if cls is technology_network:
        expected_edges += [(str(gate), str(inverted), "solid"), (str(inverted), "po1", "solid")]
    else:
        expected_edges += [(str(gate), "po1", "dashed")]
    path = tmp_path / "network.dot"
    for indexed in [True, False]:
        if indexed:
            write_dot_network(network, str(path))
        else:
            write_dot_network(network=network, filename=str(path), indexes=False)
        text = path.read_text(encoding="utf-8")
        assert re.match(r"\s*digraph\s*\{", text)
        assert text.rstrip().endswith("}")
        nodes = re.findall(r"^[ \t]*(\d+|po\d+)[ \t]*\[([^\n]*)\][ \t]*;?[ \t]*$", text, re.MULTILINE)
        assert sorted(node for node, _ in nodes) == sorted([*(str(node) for node in network.nodes()), "po0", "po1"])
        attributes = dict(nodes)
        label = f"{gate}: AND" if indexed else "AND"
        assert re.findall(r'label\s*=\s*"([^"]*)"', attributes[str(gate)]) == [label]
        assert re.findall(r"fillcolor\s*=\s*([a-z0-9]+)", attributes[str(gate)]) == ["lightcoral"]
        if cls is technology_network:
            label = f"{inverted}: INV" if indexed else "INV"
            assert re.findall(r'label\s*=\s*"([^"]*)"', attributes[str(inverted)]) == [label]
            assert re.findall(r"fillcolor\s*=\s*([a-z0-9]+)", attributes[str(inverted)]) == ["paleturquoise"]
        edges = re.findall(
            r"^[ \t]*(\d+)\s*->\s*(\d+|po\d+)\s*\[style\s*=\s*(solid|dashed)\][ \t]*;?[ \t]*$", text, re.MULTILINE
        )
        assert sorted(edges) == sorted(expected_edges)


@pytest.mark.parametrize(("writer", "suffix"), WRITERS)
def test_network_writers_preserve_symlinks(
    resources_dir: Path, tmp_path: Path, writer: Callable[[Any, str], None], suffix: str
) -> None:
    """Successful writes preserve symlinks; dangling links fail without creating their targets.

    Raises:
        OSError: A filesystem error other than unsupported or unprivileged symlink creation occurs.
    """
    target = tmp_path / f"target{suffix}"
    target.write_text("original", encoding="utf-8")
    symbolic = tmp_path / f"latest{suffix}"
    try:
        symbolic.symlink_to(target.name)
    except OSError as error:
        if getattr(error, "winerror", None) == 1314 or error.errno in (errno.ENOSYS, errno.EOPNOTSUPP):
            pytest.skip("The filesystem does not permit symbolic-link creation")
        raise
    network = read_network(str(resources_dir / "mux21.v"), network_type=aig_network)
    writer(network, str(symbolic))
    assert symbolic.is_symlink()
    assert symbolic.read_bytes() == target.read_bytes()
    assert target.read_bytes() != b"original"
    if suffix != ".dot":
        assert simulate_outputs(read_network(str(symbolic))) == [
            ("out", [True, True, False, False, True, False, True, False])
        ]
    target.unlink()
    with pytest.raises(RuntimeError):
        writer(network, str(symbolic))
    assert symbolic.is_symlink()
    assert not target.exists()
    assert list(tmp_path.iterdir()) == [symbolic]


@pytest.mark.parametrize("cls", [technology_network, xag_network, mig_network])
def test_aiger_writer_rejects_other_representations(tmp_path: Path, cls: type[Network]) -> None:
    """AIGER accepts AIGs and rejects other network representations before creating a file."""
    path = tmp_path / "out.aig"
    with pytest.raises(TypeError):
        cast("Any", write_aiger)(cls(), str(path))
    assert not path.exists()
