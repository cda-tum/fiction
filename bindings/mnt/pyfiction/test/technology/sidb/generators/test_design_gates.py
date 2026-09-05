# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction import (
    create_and_tt,
    create_nor_tt,
    design_sidb_gates,
    design_sidb_gates_mode,
    design_sidb_gates_params,
    lattice,
    lattice_site,
    operational_condition,
    sidb_layout,
    sidb_simulation_engine,
    sidb_technology,
    termination_condition,
)


@pytest.fixture
def nor_gate_skeleton() -> sidb_layout:
    """The H-Si(111) 1x1 NOR gate skeleton that the canvas SiDBs are designed into.

    Returns:
        The skeleton as an SiDB layout over the H-Si(111) 1x1 lattice, with an empty canvas.
    """
    layout = sidb_layout(lattice.si_111_1x1())

    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.INPUT)
    layout.assign_cell_type(lattice_site(1, 1, 1), sidb_technology.cell_type.INPUT)

    layout.assign_cell_type(lattice_site(25, 0, 0), sidb_technology.cell_type.INPUT)
    layout.assign_cell_type(lattice_site(23, 1, 1), sidb_technology.cell_type.INPUT)

    layout.assign_cell_type(lattice_site(4, 4, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(21, 4, 0), sidb_technology.cell_type.NORMAL)

    layout.assign_cell_type(lattice_site(5, 5, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(19, 5, 1), sidb_technology.cell_type.NORMAL)

    layout.assign_cell_type(lattice_site(8, 8, 0), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(17, 8, 0), sidb_technology.cell_type.NORMAL)

    layout.assign_cell_type(lattice_site(9, 9, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(15, 9, 1), sidb_technology.cell_type.NORMAL)

    layout.assign_cell_type(lattice_site(15, 21, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(17, 23, 0), sidb_technology.cell_type.NORMAL)

    layout.assign_cell_type(lattice_site(19, 25, 1), sidb_technology.cell_type.OUTPUT)
    layout.assign_cell_type(lattice_site(21, 27, 0), sidb_technology.cell_type.OUTPUT)

    layout.assign_cell_type(lattice_site(23, 29, 1), sidb_technology.cell_type.NORMAL)
    return layout


def test_siqad_and_gate_skeleton_100():
    layout = sidb_layout()

    layout.assign_cell_type(lattice_site(0, 0, 1), sidb_technology.cell_type.INPUT)
    layout.assign_cell_type(lattice_site(2, 1, 1), sidb_technology.cell_type.INPUT)

    layout.assign_cell_type(lattice_site(20, 0, 1), sidb_technology.cell_type.INPUT)
    layout.assign_cell_type(lattice_site(18, 1, 1), sidb_technology.cell_type.INPUT)

    layout.assign_cell_type(lattice_site(4, 2, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(6, 3, 1), sidb_technology.cell_type.NORMAL)

    layout.assign_cell_type(lattice_site(14, 3, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(16, 2, 1), sidb_technology.cell_type.NORMAL)

    layout.assign_cell_type(lattice_site(10, 6, 0), sidb_technology.cell_type.OUTPUT)
    layout.assign_cell_type(lattice_site(10, 7, 0), sidb_technology.cell_type.OUTPUT)

    layout.assign_cell_type(lattice_site(10, 9, 1), sidb_technology.cell_type.NORMAL)

    params = design_sidb_gates_params()
    params.operational_params.simulation_parameters.base = 2
    params.operational_params.simulation_parameters.mu_minus = -0.28
    params.design_mode = design_sidb_gates_mode.AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER
    params.termination_cond = termination_condition.ALL_COMBINATIONS_ENUMERATED
    params.canvas = (lattice_site(4, 4, 0), lattice_site(14, 5, 1))
    params.number_of_canvas_sidbs = 1
    params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT

    assert params.operational_params.simulation_parameters.mu_minus == -0.28
    assert params.number_of_canvas_sidbs == 1
    assert params.canvas[0] == lattice_site(4, 4, 0)
    assert params.canvas[1] == lattice_site(14, 5, 1)

    designed_gates = design_sidb_gates(layout, [create_and_tt()], params)

    assert len(designed_gates) == 23


def test_nor_gate_111(nor_gate_skeleton):
    layout = nor_gate_skeleton
    params = design_sidb_gates_params()
    params.operational_params.simulation_parameters.base = 2
    params.operational_params.simulation_parameters.mu_minus = -0.32
    params.design_mode = design_sidb_gates_mode.AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER
    params.termination_cond = termination_condition.ALL_COMBINATIONS_ENUMERATED
    params.canvas = (lattice_site(10, 11, 0), lattice_site(14, 17, 0))
    params.number_of_canvas_sidbs = 3
    params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
    params.operational_params.op_condition = operational_condition.REJECT_KINKS

    assert params.operational_params.simulation_parameters.mu_minus == -0.32
    assert params.number_of_canvas_sidbs == 3
    assert params.canvas[0] == lattice_site(10, 11, 0)
    assert params.canvas[1] == lattice_site(14, 17, 0)

    designed_gates = design_sidb_gates(layout, [create_nor_tt()], params)
    assert len(designed_gates) == 44

    params.design_mode = design_sidb_gates_mode.PRUNING_ONLY
    designed_gate_candidates = design_sidb_gates(layout, [create_nor_tt()], params)
    assert len(designed_gate_candidates) == 44

    # tolerate kink states
    params.design_mode = design_sidb_gates_mode.AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER
    params.operational_params.op_condition = operational_condition.TOLERATE_KINKS
    designed_gates = design_sidb_gates(layout, [create_nor_tt()], params)
    assert len(designed_gates) == 175


def test_nor_gate_111_quickcell(nor_gate_skeleton):
    layout = nor_gate_skeleton
    params = design_sidb_gates_params()
    params.operational_params.simulation_parameters.base = 2
    params.operational_params.simulation_parameters.mu_minus = -0.32
    params.design_mode = design_sidb_gates_mode.AUTOMATIC_EXHAUSTIVE_GATE_DESIGNER
    params.termination_cond = termination_condition.ALL_COMBINATIONS_ENUMERATED

    params.canvas = (lattice_site(10, 13, 0), lattice_site(14, 17, 0))
    params.number_of_canvas_sidbs = 3
    params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT

    assert params.operational_params.simulation_parameters.mu_minus == -0.32
    assert params.number_of_canvas_sidbs == 3
    assert params.canvas[0] == lattice_site(10, 13, 0)
    assert params.canvas[1] == lattice_site(14, 17, 0)

    designed_gates = design_sidb_gates(layout, [create_nor_tt()], params)
    assert len(designed_gates) == 14
