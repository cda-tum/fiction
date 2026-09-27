# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction.sidb import DotTag, Lattice, LatticeSite, SiDBLayout
from mnt.pyfiction.sidb.generators import (
    design_sidb_gates,
    design_sidb_gates_mode,
    design_sidb_gates_params,
    design_sidb_gates_stats,
    termination_condition,
)
from mnt.pyfiction.sidb.simulation import SimulationEngine
from mnt.pyfiction.sidb.simulation.logic import operational_condition
from mnt.pyfiction.synthesis import (
    standard_functions,
)


@pytest.fixture
def nor_gate_skeleton() -> SiDBLayout:
    """The H-Si(111) 1x1 NOR gate skeleton that the canvas SiDBs are designed into.

    Returns:
        The skeleton as an SiDB layout over the H-Si(111) 1x1 lattice, with an empty canvas.
    """
    layout = SiDBLayout(Lattice.si_111_1x1())

    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.INPUT)
    layout.assign_sidb(LatticeSite(1, 1, 1), DotTag.INPUT)

    layout.assign_sidb(LatticeSite(25, 0, 0), DotTag.INPUT)
    layout.assign_sidb(LatticeSite(23, 1, 1), DotTag.INPUT)

    layout.assign_sidb(LatticeSite(4, 4, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(21, 4, 0), DotTag.NORMAL)

    layout.assign_sidb(LatticeSite(5, 5, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(19, 5, 1), DotTag.NORMAL)

    layout.assign_sidb(LatticeSite(8, 8, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(17, 8, 0), DotTag.NORMAL)

    layout.assign_sidb(LatticeSite(9, 9, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(15, 9, 1), DotTag.NORMAL)

    layout.assign_sidb(LatticeSite(15, 21, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(17, 23, 0), DotTag.NORMAL)

    layout.assign_sidb(LatticeSite(19, 25, 1), DotTag.OUTPUT)
    layout.assign_sidb(LatticeSite(21, 27, 0), DotTag.OUTPUT)

    layout.assign_sidb(LatticeSite(23, 29, 1), DotTag.NORMAL)
    return layout


def test_siqad_and_gate_skeleton_100():
    layout = SiDBLayout()

    layout.assign_sidb(LatticeSite(0, 0, 1), DotTag.INPUT)
    layout.assign_sidb(LatticeSite(2, 1, 1), DotTag.INPUT)

    layout.assign_sidb(LatticeSite(20, 0, 1), DotTag.INPUT)
    layout.assign_sidb(LatticeSite(18, 1, 1), DotTag.INPUT)

    layout.assign_sidb(LatticeSite(4, 2, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(6, 3, 1), DotTag.NORMAL)

    layout.assign_sidb(LatticeSite(14, 3, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(16, 2, 1), DotTag.NORMAL)

    layout.assign_sidb(LatticeSite(10, 6, 0), DotTag.OUTPUT)
    layout.assign_sidb(LatticeSite(10, 7, 0), DotTag.OUTPUT)

    layout.assign_sidb(LatticeSite(10, 9, 1), DotTag.NORMAL)

    params = design_sidb_gates_params()
    params.operational_params.simulation_parameters.base = 2
    params.operational_params.simulation_parameters.mu_minus = -0.28
    params.design_mode = design_sidb_gates_mode.AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER
    params.termination_cond = termination_condition.ALL_COMBINATIONS_ENUMERATED
    params.canvas = (LatticeSite(4, 4, 0), LatticeSite(14, 5, 1))
    params.number_of_canvas_sidbs = 1
    params.operational_params.sim_engine = SimulationEngine.QUICKEXACT

    assert params.operational_params.simulation_parameters.mu_minus == -0.28
    assert params.number_of_canvas_sidbs == 1
    assert params.maximal_random_design_attempts == 1_000_000
    assert params.operational_params.timeout == 2**64 - 1
    assert params.canvas[0] == LatticeSite(4, 4, 0)
    assert params.canvas[1] == LatticeSite(14, 5, 1)

    reports: list[tuple[str, int, int]] = []
    params.on_progress = lambda task, done, total: reports.append((task, done, total))

    stats = design_sidb_gates_stats()
    designed_gates = design_sidb_gates(layout, [standard_functions("and")[0]], params, stats)

    assert len(designed_gates) == 23
    assert "total time" in repr(stats)
    assert reports
    assert reports[-1][1] == reports[-1][2]


def test_nor_gate_111(nor_gate_skeleton):
    layout = nor_gate_skeleton
    params = design_sidb_gates_params()
    params.operational_params.simulation_parameters.base = 2
    params.operational_params.simulation_parameters.mu_minus = -0.32
    params.design_mode = design_sidb_gates_mode.AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER
    params.termination_cond = termination_condition.ALL_COMBINATIONS_ENUMERATED
    params.canvas = (LatticeSite(10, 11, 0), LatticeSite(14, 17, 0))
    params.number_of_canvas_sidbs = 3
    params.operational_params.sim_engine = SimulationEngine.QUICKEXACT
    params.operational_params.op_condition = operational_condition.REJECT_KINKS

    assert params.operational_params.simulation_parameters.mu_minus == -0.32
    assert params.number_of_canvas_sidbs == 3
    assert params.canvas[0] == LatticeSite(10, 11, 0)
    assert params.canvas[1] == LatticeSite(14, 17, 0)

    designed_gates = design_sidb_gates(layout, [standard_functions("nor")[0]], params)
    assert len(designed_gates) == 44

    params.design_mode = design_sidb_gates_mode.PRUNING_ONLY
    designed_gate_candidates = design_sidb_gates(layout, [standard_functions("nor")[0]], params)
    assert len(designed_gate_candidates) == 44

    # tolerate kink states
    params.design_mode = design_sidb_gates_mode.AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER
    params.operational_params.op_condition = operational_condition.TOLERATE_KINKS
    designed_gates = design_sidb_gates(layout, [standard_functions("nor")[0]], params)
    assert len(designed_gates) == 175


def test_nor_gate_111_quickcell(nor_gate_skeleton):
    layout = nor_gate_skeleton
    params = design_sidb_gates_params()
    params.operational_params.simulation_parameters.base = 2
    params.operational_params.simulation_parameters.mu_minus = -0.32
    params.design_mode = design_sidb_gates_mode.AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER
    params.termination_cond = termination_condition.ALL_COMBINATIONS_ENUMERATED

    params.canvas = (LatticeSite(10, 13, 0), LatticeSite(14, 17, 0))
    params.number_of_canvas_sidbs = 3
    params.operational_params.sim_engine = SimulationEngine.QUICKEXACT

    assert params.operational_params.simulation_parameters.mu_minus == -0.32
    assert params.number_of_canvas_sidbs == 3
    assert params.canvas[0] == LatticeSite(10, 13, 0)
    assert params.canvas[1] == LatticeSite(14, 17, 0)

    designed_gates = design_sidb_gates(layout, [standard_functions("nor")[0]], params)
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
def test_gate_design_timeout(nor_gate_skeleton: SiDBLayout, mode: design_sidb_gates_mode) -> None:
    """Every search mode raises TimeoutError without changing its inputs or publishing partial statistics."""
    params = design_sidb_gates_params()
    params.operational_params.timeout = 0
    params.design_mode = mode
    params.number_of_canvas_sidbs = 3
    params.termination_cond = termination_condition.ALL_COMBINATIONS_ENUMERATED
    stats = design_sidb_gates_stats()
    initial_stats = repr(stats)
    initial_dots = nor_gate_skeleton.sidbs()

    with pytest.raises(TimeoutError):
        design_sidb_gates(nor_gate_skeleton, [standard_functions("nor")[0]], params, stats)

    assert nor_gate_skeleton.sidbs() == initial_dots
    assert params.operational_params.timeout == 0
    assert params.number_of_canvas_sidbs == 3
    assert repr(stats) == initial_stats
