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

from mnt.pyfiction import (
    aig_network,
    convert_network,
    get_name,
    mig_network,
    network_target,
    read_aig_network,
    read_mig_network,
    read_technology_network,
    read_xag_network,
    simulate_outputs,
    technology_network,
    write_aiger,
    write_blif,
    write_verilog,
    xag_network,
)

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path


READERS = [
    (read_technology_network, technology_network),
    (read_aig_network, aig_network),
    (read_xag_network, xag_network),
    (read_mig_network, mig_network),
]


@pytest.mark.parametrize(("reader", "cls"), READERS)
def test_readers(resources_dir: Path, reader: Callable[[str], Any], cls: type) -> None:
    network = reader(str(resources_dir / "mux21.v"))
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
    network = read_aig_network(str(source))
    path = tmp_path / f"outputs.{suffix}"
    writer = {"v": write_verilog, "blif": write_blif, "aig": write_aiger}[suffix]
    writer(network, str(path))
    restored = read_technology_network(str(path))
    # BLIF names each output with an explicit buffer record; the reader preserves those records.
    assert restored.num_gates() == gates
    assert restored.num_pos() == 3
    assert restored.depth() == (2 if suffix == "blif" else 1)
    assert simulate_outputs(restored) == simulate_outputs(network)


def test_reader_reports_diagnostics(tmp_path: Path) -> None:
    broken = tmp_path / "broken.v"
    broken.write_text("module broken(\n", encoding="utf-8")
    with pytest.raises(RuntimeError, match="could not parse"):
        read_technology_network(str(broken))


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
    restored = read_technology_network(str(path))
    assert [restored.get_name(pi) for pi in restored.pis()] == ["apple", "banana", "cherry", "unused"]
    assert simulate_outputs(restored) == simulate_outputs(interface_network)


def test_aiger_round_trip_keeps_names(interface_network: technology_network, tmp_path: Path) -> None:
    """AIGER preserves interface labels and output functions."""
    aig = convert_network(interface_network, network_target.AIG)
    path = tmp_path / "out.aig"
    write_aiger(aig, str(path))
    back = read_aig_network(str(path))
    assert [back.get_name(pi) for pi in back.pis()] == ["apple", "banana", "cherry", "unused"]
    assert simulate_outputs(back) == simulate_outputs(interface_network)


WRITERS = [(write_verilog, ".v"), (write_blif, ".blif"), (write_aiger, ".aig")]


@pytest.mark.parametrize(("writer", "suffix"), WRITERS)
def test_writers_report_unwritable_files(
    resources_dir: Path, tmp_path: Path, writer: Callable[[Any, str], None], suffix: str
) -> None:
    """A file under a directory that does not exist cannot be opened, which the writers report."""
    aig = read_aig_network(str(resources_dir / "mux21.v"))
    target = tmp_path / "missing" / f"out{suffix}"
    with pytest.raises(RuntimeError, match="missing"):
        writer(aig, str(target))
    assert not target.exists()
