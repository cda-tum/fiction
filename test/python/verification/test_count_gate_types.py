# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests of counting gate types in networks and layouts."""

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.networks import aig_network, technology_network, xag_network
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.physical_design import orthogonal
from mnt.pyfiction.synthesis import dynamic_truth_table
from mnt.pyfiction.verification import count_gate_types

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path

    from .conftest import GateLayout


def _total(stats: object) -> int:
    """Sum the exported gate counts.

    Args:
        stats: Public gate-count statistics.

    Returns:
        The number of classified gates and wires.
    """
    return sum(getattr(stats, name) for name in dir(stats) if name.startswith("num_"))


def test_count_gate_types_of_a_network(mux21: technology_network) -> None:
    """A known mux reports its complete gate total and optional detailed categories."""
    stats = count_gate_types(mux21)
    assert stats.num_and2 == 2
    assert stats.num_or2 == 1
    assert stats.num_inv == 1
    assert _total(stats) == mux21.num_gates()
    assert "AND2" in stats.report()
    assert "LT2" not in stats.report()
    assert "LT2" in stats.report(detailed=True)


def test_count_gate_types_of_an_aig(resources_dir: Path) -> None:
    """AIG nodes are classified as AND gates."""
    aig = read_network(str(resources_dir / "mux21.v"), network_type=aig_network)
    assert count_gate_types(aig).num_and2 == aig.num_gates()


def test_count_gate_types_of_a_layout(mux21: technology_network) -> None:
    """Placed wires contribute buffer and fanout categories alongside the logic gates."""
    layout = orthogonal(mux21)
    stats = count_gate_types(layout)
    assert stats.num_and2 == 2
    assert stats.num_or2 == 1
    assert stats.num_inv == 1
    assert stats.num_fanout >= 1
    assert stats.num_buf >= 1  # every wire tile is a buffer


@pytest.mark.parametrize("network_type", [technology_network, aig_network, xag_network])
def test_network_terminals_and_constants_are_excluded(
    tmp_path: Path, network_type: type[technology_network | aig_network | xag_network]
) -> None:
    """One AND remains one counted gate regardless of primary terminals and constant outputs."""
    source = tmp_path / "and.v"
    source.write_text(
        "module top(a, b, result, zero, one);\ninput a, b;\noutput result, zero, one;\n"
        "assign result = a & b;\nassign zero = 1'b0;\nassign one = 1'b1;\nendmodule\n",
        encoding="utf-8",
    )
    stats = count_gate_types(read_network(str(source), network_type=network_type))
    assert stats.num_and2 == 1
    assert _total(stats) == 1


@pytest.mark.parametrize(
    ("hexadecimal", "category"),
    [
        ("80", "and3"),
        ("28", "xor_and"),
        ("a8", "or_and"),
        ("16", "onehot"),
        ("81", "gamble"),
        ("6a", "and_xor"),
        ("e8", "maj3"),
        ("d8", "mux"),
        ("52", "dot"),
    ],
)
def test_technology_network_ternary_categories(tmp_path: Path, hexadecimal: str, category: str) -> None:
    """Technology-network LUTs retain the documented ternary classifications."""
    rows = [
        "".join(str((row >> bit) & 1) for bit in range(3)) + " 1\n"
        for row in range(8)
        if (int(hexadecimal, 16) >> row) & 1
    ]
    source = tmp_path / "ternary.blif"
    source.write_text(
        ".model ternary\n.inputs a b c\n.outputs result\n.names a b c result\n" + "".join(rows) + ".end\n",
        encoding="utf-8",
    )
    stats = count_gate_types(read_network(str(source)))
    assert getattr(stats, f"num_{category}") == 1
    assert _total(stats) == 1


def test_placed_functions_and_wire_roles(make_layout: Callable[[tuple[int, int, int], str], GateLayout]) -> None:
    """All native gate functions are counted once; generic LUTs stay in the other category."""
    layout = make_layout((19, 2, 1), "OPEN")
    a, b, c = [layout.create_pi(name, (index, 0)) for index, name in enumerate(["a", "b", "c"])]
    buffer = layout.create_buf(a, (3, 0))
    fanout = layout.create_buf(a, (4, 0))
    layout.create_po(buffer, "buffer", (3, 1))
    layout.create_not(a, (5, 0))
    for x, (name, left) in enumerate(
        [
            ("and", fanout),
            ("or", a),
            ("nand", fanout),
            ("nor", a),
            ("xor", a),
            ("xnor", a),
            ("lt", a),
            ("gt", a),
            ("le", a),
            ("ge", a),
        ],
        start=6,
    ):
        getattr(layout, f"create_{name}")(left, b, (x, 0))
    layout.create_maj(a, b, c, (16, 0))
    function = dynamic_truth_table(3)
    function.create_from_hex_string("80")
    layout.create_gate([a, b, c], function, (17, 0))
    stats = count_gate_types(layout)
    expected = {
        "num_fanout",
        "num_buf",
        "num_inv",
        "num_and2",
        "num_or2",
        "num_nand2",
        "num_nor2",
        "num_xor2",
        "num_xnor2",
        "num_lt2",
        "num_gt2",
        "num_le2",
        "num_ge2",
        "num_maj3",
        "num_other",
    }
    assert {name: getattr(stats, name) for name in dir(stats) if name.startswith("num_")} == {
        name: int(name in expected) for name in dir(stats) if name.startswith("num_")
    }
    assert _total(stats) == len(expected)
