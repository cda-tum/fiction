# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import Any

import pytest

from mnt.pyfiction.sidb import lattice, lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.generators import (
    design_sidb_gates,
    design_sidb_gates_mode,
    design_sidb_gates_stats,
    termination_condition,
)
from mnt.pyfiction.sidb.simulation import sidb_simulation_engine
from mnt.pyfiction.sidb.simulation.logic import operational_condition
from mnt.pyfiction.synthesis import create_and_tt, create_nor_tt


@pytest.fixture
def nor_gate_skeleton() -> sidb_layout:
    """The H-Si(111) 1x1 NOR gate skeleton that the canvas SiDBs are designed into.

    Returns:
        The skeleton as an SiDB layout over the H-Si(111) 1x1 lattice, with an empty canvas.
    """
    layout = sidb_layout(lattice.si_111_1x1())

    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.INPUT)
    layout.assign_sidb(lattice_site(1, 1, 1), sidb_dot_tag.INPUT)

    layout.assign_sidb(lattice_site(25, 0, 0), sidb_dot_tag.INPUT)
    layout.assign_sidb(lattice_site(23, 1, 1), sidb_dot_tag.INPUT)

    layout.assign_sidb(lattice_site(4, 4, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(21, 4, 0), sidb_dot_tag.NORMAL)

    layout.assign_sidb(lattice_site(5, 5, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(19, 5, 1), sidb_dot_tag.NORMAL)

    layout.assign_sidb(lattice_site(8, 8, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(17, 8, 0), sidb_dot_tag.NORMAL)

    layout.assign_sidb(lattice_site(9, 9, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(15, 9, 1), sidb_dot_tag.NORMAL)

    layout.assign_sidb(lattice_site(15, 21, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(17, 23, 0), sidb_dot_tag.NORMAL)

    layout.assign_sidb(lattice_site(19, 25, 1), sidb_dot_tag.OUTPUT)
    layout.assign_sidb(lattice_site(21, 27, 0), sidb_dot_tag.OUTPUT)

    layout.assign_sidb(lattice_site(23, 29, 1), sidb_dot_tag.NORMAL)
    return layout


def test_siqad_and_gate_skeleton_100():
    layout = sidb_layout()

    layout.assign_sidb(lattice_site(0, 0, 1), sidb_dot_tag.INPUT)
    layout.assign_sidb(lattice_site(2, 1, 1), sidb_dot_tag.INPUT)

    layout.assign_sidb(lattice_site(20, 0, 1), sidb_dot_tag.INPUT)
    layout.assign_sidb(lattice_site(18, 1, 1), sidb_dot_tag.INPUT)

    layout.assign_sidb(lattice_site(4, 2, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(6, 3, 1), sidb_dot_tag.NORMAL)

    layout.assign_sidb(lattice_site(14, 3, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(16, 2, 1), sidb_dot_tag.NORMAL)

    layout.assign_sidb(lattice_site(10, 6, 0), sidb_dot_tag.OUTPUT)
    layout.assign_sidb(lattice_site(10, 7, 0), sidb_dot_tag.OUTPUT)

    layout.assign_sidb(lattice_site(10, 9, 1), sidb_dot_tag.NORMAL)

    options: dict[str, Any] = {
        "base": 2,
        "mu_minus": -0.28,
        "design_mode": design_sidb_gates_mode.AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
        "termination_cond": termination_condition.ALL_COMBINATIONS_ENUMERATED,
        "canvas": (lattice_site(4, 4, 0), lattice_site(14, 5, 1)),
        "number_of_canvas_sidbs": 1,
        "sim_engine": sidb_simulation_engine.QUICKEXACT,
    }

    assert options["canvas"][0] == lattice_site(4, 4, 0)
    assert options["canvas"][1] == lattice_site(14, 5, 1)

    reports: list[tuple[str, int, int]] = []
    options["on_progress"] = lambda task, done, total: reports.append((task, done, total))

    stats = design_sidb_gates_stats()
    designed_gates = design_sidb_gates(layout, [create_and_tt()], stats=stats, **options)

    assert len(designed_gates) == 23
    assert "total time" in repr(stats)
    assert reports
    assert reports[-1][1] == reports[-1][2]


def test_nor_gate_111(nor_gate_skeleton):
    layout = nor_gate_skeleton
    options: dict[str, Any] = {
        "base": 2,
        "mu_minus": -0.32,
        "design_mode": design_sidb_gates_mode.AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
        "termination_cond": termination_condition.ALL_COMBINATIONS_ENUMERATED,
        "canvas": (lattice_site(10, 11, 0), lattice_site(14, 17, 0)),
        "number_of_canvas_sidbs": 3,
        "sim_engine": sidb_simulation_engine.QUICKEXACT,
        "op_condition": operational_condition.REJECT_KINKS,
    }

    assert options["canvas"][0] == lattice_site(10, 11, 0)
    assert options["canvas"][1] == lattice_site(14, 17, 0)

    designed_gates = design_sidb_gates(layout, [create_nor_tt()], **options)
    assert len(designed_gates) == 44

    options["design_mode"] = design_sidb_gates_mode.PRUNING_ONLY
    designed_gate_candidates = design_sidb_gates(layout, [create_nor_tt()], **options)
    assert len(designed_gate_candidates) == 44

    # tolerate kink states
    options["design_mode"] = design_sidb_gates_mode.AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER
    options["op_condition"] = operational_condition.TOLERATE_KINKS
    designed_gates = design_sidb_gates(layout, [create_nor_tt()], **options)
    assert len(designed_gates) == 175


def test_nor_gate_111_quickcell(nor_gate_skeleton):
    layout = nor_gate_skeleton
    options: dict[str, Any] = {
        "base": 2,
        "mu_minus": -0.32,
        "design_mode": design_sidb_gates_mode.AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
        "termination_cond": termination_condition.ALL_COMBINATIONS_ENUMERATED,
    }

    options["canvas"] = (lattice_site(10, 13, 0), lattice_site(14, 17, 0))
    options["number_of_canvas_sidbs"] = 3
    options["sim_engine"] = sidb_simulation_engine.QUICKEXACT

    assert options["canvas"][0] == lattice_site(10, 13, 0)
    assert options["canvas"][1] == lattice_site(14, 17, 0)

    designed_gates = design_sidb_gates(layout, [create_nor_tt()], **options)
    assert len(designed_gates) == 14


@pytest.mark.parametrize(
    "mode",
    [
        design_sidb_gates_mode.QUICKCELL,
        design_sidb_gates_mode.AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER,
        design_sidb_gates_mode.RANDOM,
        design_sidb_gates_mode.PRUNING_ONLY,
    ],
)
def test_gate_design_timeout(nor_gate_skeleton: sidb_layout, mode: design_sidb_gates_mode) -> None:
    """Every search mode raises TimeoutError without changing its inputs or publishing partial statistics."""
    stats = design_sidb_gates_stats()
    initial_stats = repr(stats)
    initial_dots = nor_gate_skeleton.sidbs()

    with pytest.raises(TimeoutError):
        design_sidb_gates(
            nor_gate_skeleton,
            [create_nor_tt()],
            stats=stats,
            timeout=0,
            design_mode=mode,
            number_of_canvas_sidbs=3,
            termination_cond=termination_condition.ALL_COMBINATIONS_ENUMERATED,
        )

    assert nor_gate_skeleton.sidbs() == initial_dots

    assert repr(stats) == initial_stats
