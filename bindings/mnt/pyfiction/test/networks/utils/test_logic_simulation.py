# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction import exact_cartesian, exact_params, read_technology_network, simulate


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
    xor_lyt_sim = simulate(xor_lyt)
    assert xor_lyt_sim["out"] == [False, True, True, False]
