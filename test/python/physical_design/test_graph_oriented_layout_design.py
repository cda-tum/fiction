# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction.physical_design import (
    GoldCostObjective,
    GoldEffortMode,
    GraphOrientedLayoutDesignParams,
    graph_oriented_layout_design,
)
from mnt.pyfiction.verification import eq_type, equivalence_checking


def test_graph_oriented_layout_design(mux21):
    layout = graph_oriented_layout_design(mux21).layout
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_parameters(mux21):
    params = GraphOrientedLayoutDesignParams()
    params.return_first = True

    layout = graph_oriented_layout_design(mux21, params=params).layout
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_stats(mux21):

    result = graph_oriented_layout_design(mux21)
    layout = result.layout
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_stats_and_parameters(mux21):
    params = GraphOrientedLayoutDesignParams()
    params.return_first = True

    result = graph_oriented_layout_design(mux21, params=params)
    layout = result.layout
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_different_parameters(mux21):
    params = GraphOrientedLayoutDesignParams()
    params.return_first = True
    params.mode = GoldEffortMode.HIGH_EFFORT
    params.timeout = 10000
    params.verbose = True
    params.num_vertex_expansions = 5
    params.planar = False
    params.cost = GoldCostObjective.WIRES
    params.enable_multithreading = False
    params.straight_inverters = True
    params.tiles_to_skip_between_pis = 1
    params.randomize_tiles_to_skip_between_pis = True

    layout = graph_oriented_layout_design(mux21, params=params).layout
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO

    params.mode = GoldEffortMode.MAXIMUM_EFFORT

    layout = graph_oriented_layout_design(mux21, params=params).layout
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO

    params.seed = 42

    layout = graph_oriented_layout_design(mux21, params=params).layout
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_custom_cost_function(mux21):
    params = GraphOrientedLayoutDesignParams()
    params.return_first = True
    params.mode = GoldEffortMode.HIGH_EFFORT
    params.cost = GoldCostObjective.CUSTOM

    def custom_cost_objective(layout):
        return layout.num_wires() * 2 + layout.num_crossings()

    layout = graph_oriented_layout_design(mux21, params=params, custom_cost_objective=custom_cost_objective).layout
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_multithreading(mux21):
    params = GraphOrientedLayoutDesignParams()
    params.return_first = True
    params.mode = GoldEffortMode.HIGH_EFFORT
    params.enable_multithreading = True

    layout = graph_oriented_layout_design(mux21, params=params).layout
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO
