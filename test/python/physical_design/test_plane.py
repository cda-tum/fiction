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

from mnt.pyfiction.physical_design import num_clks, plane, plane_params, plane_stats
from mnt.pyfiction.synthesis import network_balancing, network_balancing_params, planarization
from mnt.pyfiction.verification import eq_type, equivalence_checking

if TYPE_CHECKING:
    from mnt.pyfiction.networks import technology_network


def _planar(network: technology_network) -> technology_network:
    """Balance the network with unified outputs and planarize it.

    Args:
        network: Technology network.

    Returns:
        The planar network whose node order is its rank order.
    """
    params = network_balancing_params()
    params.unify_outputs = True
    params.buffer_constant_outputs = False
    planar, _ = planarization(network_balancing(network, params))
    return planar


def test_plane_default(mux21: technology_network) -> None:
    planar = _planar(mux21)
    layout = plane(planar)
    assert layout.num_crossings() == 0
    assert layout.num_pis() == planar.num_pis()
    assert equivalence_checking(planar, layout) == eq_type.STRONG


def test_plane_three_clock_phases(mux21: technology_network) -> None:
    params = plane_params()
    params.number_of_clock_phases = num_clks.THREE
    layout = plane(_planar(mux21), params)
    assert layout.num_clocks() == 3
    assert layout.num_crossings() == 0


def test_plane_with_stats(mux21: technology_network) -> None:
    stats = plane_stats()
    layout = plane(_planar(mux21), statistics=stats)
    assert stats.x_size == layout.x() + 1
    assert stats.y_size == layout.y() + 1
    assert stats.num_gates > 0
    assert stats.num_wires > 0
    assert stats.num_crossings == 0
    assert "num. gates" in repr(stats)


def test_plane_reports_progress(mux21: technology_network) -> None:
    params = plane_params()
    reports = []
    params.on_progress = lambda task, done, total: reports.append((task, done, total))
    planar = _planar(mux21)
    layout = plane(planar, params)
    assert equivalence_checking(planar, layout) == eq_type.STRONG
    placements = [(done, total) for task, done, total in reports if task == "placing levels"]
    assert placements[0][0] == 0
    assert placements == sorted(placements)
    assert placements[-1][0] == placements[-1][1] > 0


def test_plane_rejects_crossings(mux21: technology_network) -> None:
    # the balanced network is ranked by node order, which still has crossings
    params = network_balancing_params()
    params.unify_outputs = True
    params.buffer_constant_outputs = False
    with pytest.raises(ValueError, match="crossings"):
        plane(network_balancing(mux21, params))
