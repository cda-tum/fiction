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

from mnt.pyfiction.physical_design import num_clks, plane, plane_stats
from mnt.pyfiction.synthesis import network_balancing, planarization
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
    planar, _ = planarization(network_balancing(network, unify_outputs=True, buffer_constant_outputs=False))
    return planar


@pytest.mark.parametrize("phases", [None, num_clks.THREE], ids=["default-four", "three"])
def test_plane_layout_and_stats(mux21: technology_network, phases: num_clks | None) -> None:
    """Both clock configurations preserve the planar function and report the returned layout."""
    planar = _planar(mux21)
    stats = plane_stats()
    layout = (
        plane(planar, statistics=stats)
        if phases is None
        else plane(planar, statistics=stats, number_of_clock_phases=phases)
    )
    assert layout.num_clocks() == (4 if phases is None else 3)
    assert layout.num_crossings() == 0
    assert layout.num_pis() == planar.num_pis()
    assert equivalence_checking(planar, layout) == eq_type.STRONG
    assert stats.x_size == layout.width()
    assert stats.y_size == layout.height()
    assert stats.num_gates == layout.num_gates()
    assert stats.num_wires == layout.num_wires()
    assert stats.num_crossings == 0
    assert "num. gates" in repr(stats)


def test_plane_reports_progress(mux21: technology_network) -> None:
    reports = []
    planar = _planar(mux21)
    layout = plane(planar, on_progress=lambda task, done, total: reports.append((task, done, total)))
    assert equivalence_checking(planar, layout) == eq_type.STRONG
    placements = [(done, total) for task, done, total in reports if task == "placing levels"]
    assert placements[0][0] == 0
    assert placements == sorted(placements)
    assert placements[-1][0] == placements[-1][1] > 0


def test_plane_rejects_crossings(mux21: technology_network) -> None:
    # the balanced network is ranked by node order, which still has crossings
    with pytest.raises(ValueError, match="crossings"):
        plane(network_balancing(mux21, unify_outputs=True, buffer_constant_outputs=False))
