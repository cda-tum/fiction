# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING

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
