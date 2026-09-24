# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction import (
    eq_type,
    equivalence_checking,
    gold_cost_objective,
    gold_effort_mode,
    graph_oriented_layout_design,
    graph_oriented_layout_design_params,
    graph_oriented_layout_design_stats,
)


def test_graph_oriented_layout_design(mux21):
    layout = graph_oriented_layout_design(mux21)

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_parameters(mux21):
    params = graph_oriented_layout_design_params()
    params.return_first = True

    layout = graph_oriented_layout_design(mux21, params)

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_stats(mux21):
    stats = graph_oriented_layout_design_stats()

    layout = graph_oriented_layout_design(mux21, statistics=stats)

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_stats_and_parameters(mux21):
    params = graph_oriented_layout_design_params()
    params.return_first = True

    stats = graph_oriented_layout_design_stats()

    layout = graph_oriented_layout_design(mux21, params, statistics=stats)

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_different_parameters(mux21):
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

    assert equivalence_checking(mux21, layout) != eq_type.NO

    params.mode = gold_effort_mode.MAXIMUM_EFFORT

    layout = graph_oriented_layout_design(mux21, params)

    assert equivalence_checking(mux21, layout) != eq_type.NO

    params.seed = 42

    layout = graph_oriented_layout_design(mux21, params)

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_custom_cost_function(mux21):
    params = graph_oriented_layout_design_params()
    params.return_first = True
    params.mode = gold_effort_mode.HIGH_EFFORT
    params.cost = gold_cost_objective.CUSTOM

    def custom_cost_objective(layout):
        return layout.num_wires() * 2 + layout.num_crossings()

    layout = graph_oriented_layout_design(mux21, params, custom_cost_objective=custom_cost_objective)

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_multithreading(mux21):
    params = graph_oriented_layout_design_params()
    params.return_first = True
    params.mode = gold_effort_mode.HIGH_EFFORT
    params.enable_multithreading = True

    layout = graph_oriented_layout_design(mux21, params)

    assert equivalence_checking(mux21, layout) != eq_type.NO
