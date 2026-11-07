# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import json
from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.layouts import cartesian_gate_layout, layout_input_port
from mnt.pyfiction.networks import simulate, technology_network
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.synthesis import dynamic_truth_table
from mnt.pyfiction.verification import eq_type, equivalence_checking, equivalence_checking_stats

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path

    from .conftest import GateLayout


def test_non_eq(resources_dir: Path) -> None:
    """A returned counterexample distinguishes the two Boolean functions."""
    xor2_net = read_network(str(resources_dir / "xor2.v"))
    xnor2_net = read_network(str(resources_dir / "xnor2.v"))

    stats = equivalence_checking_stats()
    assert stats.counter_example == []

    eq = equivalence_checking(xor2_net, xnor2_net, stats)
    assert eq == eq_type.NO
    assert stats.eq == eq_type.NO
    assert len(stats.counter_example) == xor2_net.num_pis()
    assignment = sum(int(value) << index for index, value in enumerate(stats.counter_example))
    row = (1 << xor2_net.num_pis()) - 1 - assignment
    assert next(iter(simulate(xor2_net).values()))[row] != next(iter(simulate(xnor2_net).values()))[row]


def test_equivalence_matches_named_interfaces(tmp_path: Path) -> None:
    """Named interfaces match independently of their declaration order."""
    left_file = tmp_path / "left.blif"
    right_file = tmp_path / "right.blif"
    extra_file = tmp_path / "extra.blif"
    logic = ".names a b compare\n10 1\n.names a pass\n1 1\n.end\n"
    left_file.write_text(".model left\n.inputs a b\n.outputs compare pass\n" + logic)
    right_file.write_text(".model right\n.inputs b a\n.outputs pass compare\n" + logic)
    extra_file.write_text(".model extra\n.inputs b a unused\n.outputs pass compare\n" + logic)
    left = read_network(str(left_file))
    right = read_network(str(right_file))
    extra = read_network(str(extra_file))
    assert equivalence_checking(left, right) == eq_type.STRONG
    assert equivalence_checking(left, extra) == eq_type.NO


def test_empty_layout_and_network_equivalence(
    make_layout: Callable[[tuple[int, int, int], str], GateLayout],
) -> None:
    """Empty layouts and the exposed equivalence-network type agree in both directions."""
    layout = make_layout((0, 0, 1), "OPEN")
    for left, right in [(technology_network(), layout), (layout, technology_network()), (layout, layout.clone())]:
        stats = equivalence_checking_stats()
        assert equivalence_checking(left, right, stats) == eq_type.STRONG
        assert stats.eq == eq_type.STRONG
        assert stats.counter_example == []
        assert (stats.tp_spec, stats.tp_impl, stats.tp_diff) == (1, 1, 0)
        assert stats.spec_drv_stats.drvs == stats.impl_drv_stats.drvs == 0


def test_empty_networks_are_equivalent() -> None:
    """Two empty technology networks compare strongly without an output miter."""
    stats = equivalence_checking_stats()
    assert equivalence_checking(technology_network(), technology_network(), stats) == eq_type.STRONG
    assert stats.eq == eq_type.STRONG
    assert stats.counter_example == []


@pytest.mark.parametrize("direction", ["layout-layout", "network-layout", "layout-network"])
def test_logical_inequivalence_has_no_physical_defects(
    tmp_path: Path, direction: str, make_layout: Callable[[tuple[int, int, int], str], GateLayout]
) -> None:
    """Valid physical layouts with different functions return a distinguishing counterexample."""
    identity = cartesian_gate_layout((1, 3, 1), "2DDWave")
    signal = identity.create_pi("a", (0, 0))
    identity.create_po(signal, "result", (0, 1))
    inverted = make_layout((1, 3, 1), "2DDWave")
    source = inverted.create_pi("a", (0, 0))
    gate = inverted.create_not(source, (0, 1))
    inverted.create_po(gate, "result", (0, 2))
    path = tmp_path / "identity.blif"
    path.write_text(".model identity\n.inputs a\n.outputs result\n.names a result\n1 1\n.end\n", encoding="utf-8")
    network = read_network(str(path))
    specification = network if direction == "network-layout" else identity
    implementation = network if direction == "layout-network" else inverted
    if direction == "layout-network":
        specification = inverted
    stats = equivalence_checking_stats()
    assert equivalence_checking(specification, implementation, stats) == eq_type.NO
    assert stats.eq == eq_type.NO
    assert stats.spec_drv_stats.drvs == stats.impl_drv_stats.drvs == 0
    assert len(stats.counter_example) == 1
    assignment = int(stats.counter_example[0])
    assert simulate(specification)["result"][1 - assignment] != simulate(implementation)["result"][1 - assignment]


@pytest.mark.parametrize("defect", [None, "non_adjacent", "disconnected"])
def test_physical_interface_and_defect_diagnostics(
    tmp_path: Path, defect: str | None, make_layout: Callable[[tuple[int, int, int], str], GateLayout]
) -> None:
    """Declared wire interfaces compare strongly unless a required physical connection is invalid."""
    source = tmp_path / "identity.blif"
    source.write_text(".model identity\n.inputs a\n.outputs result\n.names a result\n1 1\n.end\n", encoding="utf-8")
    network = read_network(str(source))
    layout = make_layout((1, 3, 1), "2DDWave")
    signal = layout.create_pi("a", (0, 0))
    output = layout.create_po(signal, "result", (0, 1))
    if defect == "non_adjacent":
        layout.move_object(output, (0, 2))
    elif defect == "disconnected":
        layout.disconnect(layout_input_port(output, 0))
    stats = equivalence_checking_stats()
    expected = eq_type.STRONG if defect is None else eq_type.NO
    assert equivalence_checking(network, layout, stats) == expected
    assert stats.eq == expected
    assert stats.spec_drv_stats.drvs == 0
    assert (stats.impl_drv_stats.drvs == 0) is (defect is None)
    if defect == "non_adjacent":
        report = json.loads(stats.impl_drv_stats.report)
        assert len(report["Non adjacent connections"]) == 2
        assert not report.get("Objects outside extent")
    if defect is None:
        assert equivalence_checking(layout, network) == eq_type.STRONG
        assert equivalence_checking(layout, layout.clone()) == eq_type.STRONG
        reference = cartesian_gate_layout((1, 3, 1), "2DDWave")
        reference_source = reference.create_pi("a", (0, 0))
        reference.create_po(reference_source, "result", (0, 1))
        assert equivalence_checking(reference, layout) == eq_type.STRONG
        assert equivalence_checking(layout, reference) == eq_type.STRONG


def test_wire_throughput_is_weak_equivalence(
    tmp_path: Path, make_layout: Callable[[tuple[int, int, int], str], GateLayout]
) -> None:
    """Logical equivalence retains the throughput denominator of a longer physical branch."""
    source = tmp_path / "and.blif"
    source.write_text(".model and\n.inputs a b\n.outputs result\n.names a b result\n11 1\n.end\n", encoding="utf-8")
    network = read_network(str(source))
    layout = make_layout((6, 2, 1), "2DDWave")
    path = layout.create_pi("a", (0, 0))
    short_path = layout.create_pi("b", (3, 1))
    for x in range(1, 5):
        path = layout.create_buf(path, (x, 0))
    gate = layout.create_and(path, short_path, (4, 1))
    layout.create_po(gate, "result", (5, 1))
    stats = equivalence_checking_stats()
    assert equivalence_checking(network, layout, stats) == eq_type.WEAK
    assert stats.eq == eq_type.WEAK
    assert (stats.tp_spec, stats.tp_impl, stats.tp_diff) == (1, 2, 1)
    assert stats.counter_example == []
    assert stats.spec_drv_stats.drvs == stats.impl_drv_stats.drvs == 0
    assert equivalence_checking(layout, layout.clone()) == eq_type.STRONG


def test_clocked_required_cycle_is_not_equivalent(tmp_path: Path) -> None:
    """A clock-legal physical cycle cannot implement an acyclic identity specification."""
    source = tmp_path / "identity.blif"
    source.write_text(".model identity\n.inputs a\n.outputs result\n.names a result\n1 1\n.end\n", encoding="utf-8")
    network = read_network(str(source))
    layout = cartesian_gate_layout((3, 3, 1), "OPEN")
    signal = layout.create_pi("a", (0, 1))
    function = dynamic_truth_table(2)
    function.create_from_hex_string("8")
    gate = layout.create_gate([signal], function, (1, 1))
    first = layout.create_buf(gate, (2, 1))
    second = layout.create_buf(first, (2, 2))
    third = layout.create_buf(second, (1, 2))
    layout.connect(third, layout_input_port(gate, 1))
    layout.create_po(third, "result", (0, 2))
    for coordinate, phase in [((0, 1), 3), ((1, 1), 0), ((2, 1), 1), ((2, 2), 2), ((1, 2), 3), ((0, 2), 0)]:
        layout.assign_clock_number(coordinate, phase)
    stats = equivalence_checking_stats()
    assert equivalence_checking(network, layout, stats) == eq_type.NO
    assert stats.eq == eq_type.NO
    assert stats.impl_drv_stats.drvs == 0
