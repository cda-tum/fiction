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

from mnt.pyfiction.networks.io import read_technology_network
from mnt.pyfiction.physical_design import (
    gold_cost_objective,
    gold_effort_mode,
    graph_oriented_layout_design,
    graph_oriented_layout_design_params,
    graph_oriented_layout_design_stats,
)
from mnt.pyfiction.verification import eq_type, equivalence_checking

if TYPE_CHECKING:
    from pathlib import Path

    from mnt.pyfiction.networks import technology_network


def test_graph_oriented_layout_design(mux21: technology_network) -> None:
    layout = graph_oriented_layout_design(mux21)
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_parameters(mux21: technology_network) -> None:
    params = graph_oriented_layout_design_params()
    params.return_first = True

    layout = graph_oriented_layout_design(mux21, params)
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_stats(mux21: technology_network) -> None:
    stats = graph_oriented_layout_design_stats()

    layout = graph_oriented_layout_design(mux21, statistics=stats)
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_stats_and_parameters(mux21: technology_network) -> None:
    params = graph_oriented_layout_design_params()
    params.return_first = True

    stats = graph_oriented_layout_design_stats()

    layout = graph_oriented_layout_design(mux21, params, statistics=stats)
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_different_parameters(mux21: technology_network) -> None:
    params = graph_oriented_layout_design_params()
    params.return_first = True
    params.mode = gold_effort_mode.HIGH_EFFORT
    params.timeout = 10000
    params.verbose = True
    params.num_vertex_expansions = 5
    params.planar = False
    params.cost = gold_cost_objective.WIRES
    params.enable_multithreading = False
    params.straight_inverters = True
    params.tiles_to_skip_between_pis = 1
    params.randomize_tiles_to_skip_between_pis = True

    layout = graph_oriented_layout_design(mux21, params)
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO

    params.mode = gold_effort_mode.MAXIMUM_EFFORT

    layout = graph_oriented_layout_design(mux21, params)
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO

    params.seed = 42

    layout = graph_oriented_layout_design(mux21, params)
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO


@pytest.mark.parametrize("skip", [-1, 2**20 + 1])
def test_graph_oriented_layout_design_rejects_pi_spacing_outside_of_its_range(mux21, skip):
    params = graph_oriented_layout_design_params()
    params.return_first = True
    params.tiles_to_skip_between_pis = skip

    with pytest.raises(ValueError, match="tiles_to_skip_between_pis"):
        graph_oriented_layout_design(mux21, params)


def test_graph_oriented_layout_design_with_custom_cost_function(mux21: technology_network) -> None:
    params = graph_oriented_layout_design_params()
    params.return_first = True
    params.mode = gold_effort_mode.HIGH_EFFORT
    params.cost = gold_cost_objective.CUSTOM

    def custom_cost_objective(layout):
        return layout.num_wires() * 2 + layout.num_crossings()

    layout = graph_oriented_layout_design(mux21, params, custom_cost_objective=custom_cost_objective)
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_multithreading(mux21: technology_network) -> None:
    params = graph_oriented_layout_design_params()
    params.return_first = True
    params.mode = gold_effort_mode.HIGH_EFFORT
    params.enable_multithreading = True

    layout = graph_oriented_layout_design(mux21, params)
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_gold_preserves_declared_interface_order(tmp_path: Path) -> None:
    """GOLD retains unused inputs and terminal names in a serial result."""
    source = tmp_path / "ordered.blif"
    source.write_text(".model ordered\n.inputs unused a b\n.outputs less\n.names b a less\n01 1\n.end\n")
    ntk = read_technology_network(str(source))
    params = graph_oriented_layout_design_params()
    params.enable_multithreading = False
    params.return_first = True
    params.timeout = 10000
    layout = graph_oriented_layout_design(ntk, params)
    assert layout is not None
    assert layout.num_pis() == 3
    assert [layout.get_name(layout.pi_at(i)) for i in range(3)] == ["unused", "a", "b"]
    assert layout.get_output_name(0) == "less"
