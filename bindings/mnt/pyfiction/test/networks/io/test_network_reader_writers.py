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
    eq_type,
    equivalence_checking,
    get_name,
    mig_network,
    read_aig_network,
    read_mig_network,
    read_technology_network,
    read_xag_network,
    technology_network,
    write_aiger,
    write_blif,
    write_dot_network,
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


def test_reader_reports_diagnostics(tmp_path: Path) -> None:
    broken = tmp_path / "broken.v"
    broken.write_text("module broken(\n", encoding="utf-8")
    with pytest.raises(RuntimeError, match="could not parse"):
        read_technology_network(str(broken))


@pytest.mark.parametrize(("reader", "cls"), READERS)
def test_verilog_and_blif_round_trip(
    resources_dir: Path, tmp_path: Path, reader: Callable[[str], Any], cls: type
) -> None:
    del cls
    network = reader(str(resources_dir / "mux21.v"))
    write_verilog(network, str(tmp_path / "out.v"))
    write_blif(network, str(tmp_path / "out.blif"))
    write_dot_network(network, str(tmp_path / "out.dot"))
    assert "digraph" in (tmp_path / "out.dot").read_text(encoding="utf-8")
    for suffix in (".v", ".blif"):
        back = read_technology_network(str(tmp_path / f"out{suffix}"))
        assert equivalence_checking(convert_network(network), back) == eq_type.STRONG


def test_aiger_round_trip_keeps_names(resources_dir: Path, tmp_path: Path) -> None:
    aig = read_aig_network(str(resources_dir / "mux21.v"))
    write_aiger(aig, str(tmp_path / "out.aig"))
    back = read_aig_network(str(tmp_path / "out.aig"))
    assert back.num_gates() == aig.num_gates()
    assert [back.get_name(pi) for pi in back.pis()] == [aig.get_name(pi) for pi in aig.pis()]
    assert equivalence_checking(convert_network(aig), convert_network(back)) == eq_type.STRONG
