# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.layouts import LayoutInputPort, cartesian_gate_layout
from mnt.pyfiction.networks import simulate, simulate_outputs
from mnt.pyfiction.networks.io import read_technology_network
from mnt.pyfiction.physical_design import exact_cartesian, exact_params

if TYPE_CHECKING:
    from pathlib import Path


def test_logic_simulation(resources_dir):
    xor2_net = read_technology_network(str(resources_dir / "xor2.v"))
    xnor2_net = read_technology_network(str(resources_dir / "xnor2.v"))

    xor_sim = simulate(xor2_net)
    assert xor_sim["out"] == [False, True, True, False]

    xnor_sim = simulate(xnor2_net)
    assert xnor_sim["out"] == [True, False, False, True]

    params = exact_params()
    params.crossings = True
    xor_lyt = exact_cartesian(xor2_net, params)
    assert xor_lyt is not None
    xor_lyt_sim = simulate(xor_lyt)
    assert xor_lyt_sim["out"] == [False, True, True, False]


def test_duplicate_output_names_preserve_order(tmp_path: Path) -> None:
    path = tmp_path / "outputs.v"
    path.write_text(
        "module top(a, f, g);\ninput a;\noutput f, g;\nassign f = a;\nassign g = ~a;\nendmodule\n", encoding="utf-8"
    )
    network = read_technology_network(str(path))
    network.set_output_name(0, "same")
    network.set_output_name(1, "same")
    outputs = simulate_outputs(network)
    assert [name for name, bits in outputs] == ["same", "same"]
    assert outputs[0][1] != outputs[1][1]


def test_layout_simulation_preserves_input_slots_and_unused_inputs() -> None:
    """Extract ordered logic while ignoring incomplete dangling objects."""
    layout = cartesian_gate_layout((5, 2))
    left = layout.create_pi("left", (0, 0))
    right = layout.create_pi("right", (1, 0))
    layout.create_pi("unused", (2, 0))
    less = layout.create_lt(right, left, (3, 0))
    layout.create_po(less, "same", (4, 0))
    layout.create_po(left, "same", (4, 1))
    layout.create_buf((3, 1))
    outputs = simulate_outputs(layout)
    assert [name for name, _ in outputs] == ["same", "same"]
    assert outputs[0][1] == [False, True, False, False, False, True, False, False]
    assert outputs[1][1] == [True, False, True, False, True, False, True, False]
    layout.disconnect(LayoutInputPort(less, 0))
    with pytest.raises(ValueError, match="disconnected input"):
        simulate_outputs(layout)
