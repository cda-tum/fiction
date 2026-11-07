# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import itertools
from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction.layouts import layout_input_port
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.physical_design import (
    gold_cost_objective,
    gold_effort_mode,
    graph_oriented_layout_design,
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

    layout = graph_oriented_layout_design(mux21, return_first=True)
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_stats(mux21: technology_network) -> None:
    stats = graph_oriented_layout_design_stats()

    layout = graph_oriented_layout_design(mux21, statistics=stats)
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_stats_and_parameters(mux21: technology_network) -> None:

    stats = graph_oriented_layout_design_stats()

    layout = graph_oriented_layout_design(mux21, statistics=stats, return_first=True)
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_different_parameters(mux21: technology_network) -> None:
    options: dict[str, Any] = {
        "return_first": True,
        "mode": gold_effort_mode.HIGH_EFFORT,
        "timeout": 10000,
        "verbose": True,
        "num_vertex_expansions": 5,
        "planar": False,
        "cost": gold_cost_objective.WIRES,
        "enable_multithreading": False,
        "straight_inverters": True,
        "tiles_to_skip_between_pis": 1,
        "randomize_tiles_to_skip_between_pis": True,
    }

    layout = graph_oriented_layout_design(mux21, **options)
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO

    options["mode"] = gold_effort_mode.MAXIMUM_EFFORT

    layout = graph_oriented_layout_design(mux21, **options)
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO

    options["seed"] = 42

    layout = graph_oriented_layout_design(mux21, **options)
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO


@pytest.mark.parametrize("skip", [-1, 2**20 + 1])
def test_graph_oriented_layout_design_rejects_pi_spacing_outside_of_its_range(mux21, skip):

    with pytest.raises(ValueError, match="tiles_to_skip_between_pis"):
        graph_oriented_layout_design(mux21, return_first=True, tiles_to_skip_between_pis=skip)


def test_graph_oriented_layout_design_with_custom_cost_function(mux21: technology_network) -> None:

    evaluated = []

    def custom_cost_objective(layout):
        evaluated.append(layout.num_gates())
        return layout.num_wires() * 2 + layout.num_crossings()

    layout = graph_oriented_layout_design(
        mux21,
        custom_cost_objective=custom_cost_objective,
        return_first=True,
        mode=gold_effort_mode.HIGH_EFFORT,
        cost=gold_cost_objective.CUSTOM,
    )
    assert layout is not None
    assert evaluated

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_graph_oriented_layout_design_with_multithreading(mux21: technology_network) -> None:

    layout = graph_oriented_layout_design(
        mux21, return_first=True, mode=gold_effort_mode.HIGH_EFFORT, enable_multithreading=True
    )
    assert layout is not None

    assert equivalence_checking(mux21, layout) != eq_type.NO


def test_gold_preserves_declared_interface_order(tmp_path: Path) -> None:
    """GOLD retains unused inputs and terminal names in a serial result."""
    source = tmp_path / "ordered.blif"
    source.write_text(".model ordered\n.inputs unused a b\n.outputs less\n.names b a less\n01 1\n.end\n")
    ntk = read_network(str(source))
    layout = graph_oriented_layout_design(ntk, enable_multithreading=False, return_first=True, timeout=10000)
    assert layout is not None
    assert layout.num_pis() == 3
    assert [layout.get_name(layout.pi_at(i)) for i in range(3)] == ["unused", "a", "b"]
    assert layout.get_output_name(0) == "less"


@pytest.mark.parametrize(
    ("mode", "cost"),
    [
        (gold_effort_mode.HIGH_EFFICIENCY, gold_cost_objective.AREA),
        (gold_effort_mode.HIGH_EFFORT, gold_cost_objective.WIRES),
        (gold_effort_mode.HIGHEST_EFFORT, gold_cost_objective.CROSSINGS),
        (gold_effort_mode.MAXIMUM_EFFORT, gold_cost_objective.ACP),
    ],
)
def test_gold_modes_objectives_planarity_and_statistics(
    mux21: technology_network, mode: gold_effort_mode, cost: gold_cost_objective
) -> None:
    """Each effort mode and built-in objective returns a valid planar result."""
    stats = graph_oriented_layout_design_stats()
    reports = []
    layout = graph_oriented_layout_design(
        mux21,
        mode=mode,
        cost=cost,
        return_first=True,
        planar=True,
        timeout=10000,
        enable_multithreading=False,
        statistics=stats,
        on_progress=lambda *report: reports.append(report),
    )
    assert layout is not None
    assert equivalence_checking(mux21, layout) != eq_type.NO
    assert layout.num_crossings() == 0
    assert layout.layers() == 1
    assert (stats.x_size, stats.y_size) == (layout.width(), layout.height())
    assert (stats.num_gates, stats.num_wires, stats.num_crossings) == (
        layout.num_gates(),
        layout.num_wires(),
        layout.num_crossings(),
    )
    assert stats.time_total.total_seconds() >= 0
    counts = [(done, total) for task, done, total in reports if task == "expansions"]
    assert counts
    assert counts[-1][0] > 0
    assert counts[-1][1] == 0


def test_gold_seed_and_spacing_are_local_to_each_call(mux21: technology_network) -> None:
    """Reusing seeded options repeats PI placements after a different spacing invocation."""
    positions = []
    snapshots = []
    for skip, seed, randomize in [(0, 7, False), (3, 42, True), (0, 7, True), (3, 42, True)]:
        layout = graph_oriented_layout_design(
            mux21,
            return_first=True,
            timeout=10000,
            enable_multithreading=False,
            seed=seed,
            tiles_to_skip_between_pis=skip,
            randomize_tiles_to_skip_between_pis=randomize,
        )
        assert layout is not None
        assert equivalence_checking(mux21, layout) != eq_type.NO
        pi_sites = [layout.get_tile(pi) for pi in layout.pis()]
        positions.append(pi_sites)
        objects = []
        for node in [*layout.pis(), *layout.pos(), *layout.gates(), *layout.wires()]:
            sources = [layout.source(layout_input_port(node, i)) for i in range(layout.input_count(node))]
            objects.append((
                layout.get_tile(node),
                layout.object_function(node).to_hex(),
                [None if source is None else layout.get_tile(source) for source in sources],
            ))
        snapshots.append((
            layout.width(),
            layout.height(),
            layout.layers(),
            sorted(objects),
            pi_sites,
            [layout.get_tile(po) for po in layout.pos()],
        ))
        min_gap = max(1, skip)
        for edge in [
            sorted(site.x for site in pi_sites if site.y == 0),
            sorted(site.y for site in pi_sites if site.x == 0),
        ]:
            assert all(right - left >= min_gap for left, right in itertools.pairwise(edge))
    assert positions[0] == positions[2]
    assert positions[1] == positions[3]
    assert snapshots[1] == snapshots[3]
