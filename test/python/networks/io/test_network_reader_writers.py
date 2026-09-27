# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of reading networks of every type and writing them back."""

from __future__ import annotations

from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction.networks import AigNetwork, MigNetwork, TechnologyNetwork, XagNetwork, simulate_outputs
from mnt.pyfiction.networks.io import read_network, write_aiger, write_blif, write_verilog
from mnt.pyfiction.synthesis import convert_network

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path


READERS = [TechnologyNetwork, AigNetwork, XagNetwork, MigNetwork]


@pytest.mark.parametrize("cls", READERS)
def test_readers(resources_dir: Path, cls: type[TechnologyNetwork | AigNetwork | XagNetwork | MigNetwork]) -> None:
    network = read_network(resources_dir / "mux21.v", network_type=cls)
    assert type(network).__name__ == cls.__name__
    assert network.num_pis() == 3
    assert network.num_pos() == 1
    assert network.num_gates() > 0
    assert network.depth() > 0
    assert network.name == "mux21"
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
    network = read_network(str(source), network_type=AigNetwork)
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


@pytest.mark.parametrize("target", [TechnologyNetwork, AigNetwork, XagNetwork, MigNetwork])
@pytest.mark.parametrize("suffix", ["v", "blif"])
def test_network_writer_round_trip(
    interface_network: TechnologyNetwork,
    tmp_path: Path,
    target: type[TechnologyNetwork | AigNetwork | XagNetwork | MigNetwork],
    suffix: str,
) -> None:
    """Network writers retain functions, names, and unused inputs."""
    network = convert_network(interface_network, network_type=target)
    path = tmp_path / f"out.{suffix}"
    writer = write_verilog if suffix == "v" else write_blif
    writer(network, str(path))
    restored = read_network(str(path))
    assert [restored.get_name(pi) for pi in restored.pis()] == ["apple", "banana", "cherry", "unused"]
    assert simulate_outputs(restored) == simulate_outputs(interface_network)


def test_aiger_round_trip_keeps_names(interface_network: TechnologyNetwork, tmp_path: Path) -> None:
    """AIGER preserves interface labels and output functions."""
    aig = convert_network(interface_network, network_type=AigNetwork)
    path = tmp_path / "out.aig"
    write_aiger(aig, str(path))
    back = read_network(str(path), network_type=AigNetwork)
    assert [back.get_name(pi) for pi in back.pis()] == ["apple", "banana", "cherry", "unused"]
    assert simulate_outputs(back) == simulate_outputs(interface_network)


WRITERS = [(write_verilog, ".v"), (write_blif, ".blif"), (write_aiger, ".aig")]


@pytest.mark.parametrize(("writer", "suffix"), WRITERS)
def test_writers_report_unwritable_files(
    resources_dir: Path, tmp_path: Path, writer: Callable[[Any, str], None], suffix: str
) -> None:
    """A file under a directory that does not exist cannot be opened, which the writers report."""
    aig = read_network(str(resources_dir / "mux21.v"), network_type=AigNetwork)
    target = tmp_path / "missing" / f"out{suffix}"
    with pytest.raises(RuntimeError, match="missing"):
        writer(aig, str(target))
    assert not target.exists()
