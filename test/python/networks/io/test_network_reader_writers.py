# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of reading networks of every type and writing them back."""

from __future__ import annotations

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
    write_verilog,
)
from mnt.pyfiction.synthesis import convert_network, network_target

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path


NETWORK_TYPES = [technology_network, aig_network, xag_network, mig_network]


@pytest.mark.parametrize("cls", NETWORK_TYPES)
def test_readers(resources_dir: Path, cls: type[technology_network | aig_network | xag_network | mig_network]) -> None:
    network = read_network(str(resources_dir / "mux21.v"), network_type=cls)
    assert type(network).__name__ == cls.__name__
    assert network.num_pis() == 3
    assert network.num_pos() == 1
    assert network.num_gates() > 0
    assert network.depth() > 0
    assert get_name(network) == "mux21"
    assert [network.get_name(pi) for pi in network.pis()] == ["in0", "in1", "in2"]
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
    assert simulate_outputs(restored) == simulate_outputs(network)


def test_reader_reports_diagnostics(tmp_path: Path) -> None:
    broken = tmp_path / "broken.v"
    broken.write_text("module broken(\n", encoding="utf-8")
    with pytest.raises(RuntimeError, match="could not parse"):
        read_network(str(broken))


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
    writer(network, str(path))
    restored = read_network(str(path))
    assert [restored.get_name(pi) for pi in restored.pis()] == ["apple", "banana", "cherry", "unused"]
    assert simulate_outputs(restored) == simulate_outputs(interface_network)


def test_aiger_round_trip_keeps_names(interface_network: technology_network, tmp_path: Path) -> None:
    """AIGER preserves interface labels and output functions."""
    aig = convert_network(interface_network, network_target.AIG)
    path = tmp_path / "out.aig"
    write_aiger(aig, str(path))
    back = read_network(str(path), network_type=aig_network)
    assert [back.get_name(pi) for pi in back.pis()] == ["apple", "banana", "cherry", "unused"]
    assert simulate_outputs(back) == simulate_outputs(interface_network)


WRITERS = [(write_verilog, ".v"), (write_blif, ".blif"), (write_aiger, ".aig")]


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
    source.write_text("module one(a,b,y); input a,b; output y; assign y = a & b; endmodule\n", encoding="utf-8")
    assert type(read_network(str(directory), network_type=cls)) is cls
    renamed = tmp_path / "one.data"
    source.rename(renamed)
    assert type(read_network(str(renamed), format="v", network_type=cls)) is cls


@pytest.mark.parametrize(
    "name", ["read_technology_network", "read_aig_network", "read_xag_network", "read_mig_network"]
)
def test_reader_specialization_names_are_removed(name: str) -> None:
    """Expose one reader name for every network representation."""
    assert not hasattr(io, name)
