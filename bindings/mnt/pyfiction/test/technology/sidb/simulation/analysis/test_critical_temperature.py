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
    bdl_input_iterator_params,
    bdl_wire_selection,
    create_and_tt,
    create_not_tt,
    create_xor_tt,
    critical_temperature_gate_based,
    critical_temperature_non_gate_based,
    critical_temperature_params,
    critical_temperature_stats,
    detect_bdl_pairs,
    detect_bdl_pairs_params,
    detect_bdl_wires,
    detect_bdl_wires_params,
    generate_bdl_input_pattern_layouts,
    lattice,
    lattice_site,
    read_sqd_layout,
    sidb_layout,
    sidb_simulation_engine,
    sidb_simulation_parameters,
    sidb_technology,
)


@pytest.mark.parametrize(
    "lat",
    [pytest.param(lattice.si_100_2x1(), id="100"), pytest.param(lattice.si_111_1x1(), id="111")],
)
def test_perturber_and_sidb_pair(lat):
    layout = sidb_layout(lat)
    layout.assign_cell_type(lattice_site(0, 0, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(4, 0, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type(lattice_site(6, 0, 1), sidb_technology.cell_type.NORMAL)

    params = critical_temperature_params()

    params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT

    stats = critical_temperature_stats()

    assert critical_temperature_non_gate_based(layout, params, stats) == 400

    assert stats.algorithm_name == "QuickExact"
    assert stats.num_valid_lyt == 1


def test_gate_based_simulation(resources_dir):
    layout = read_sqd_layout(str(resources_dir / "hex_21_inputsdbp_xor_v1.sqd"), "xor_gate")
    params = critical_temperature_params()

    params.operational_params.simulation_parameters.base = 2

    params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT

    stats = critical_temperature_stats()
    spec = [create_xor_tt()]

    assert critical_temperature_gate_based(layout, spec, params, stats) <= 200

    assert stats.algorithm_name == "QuickExact"


def test_bestagon_inv(resources_dir):
    layout = read_sqd_layout(
        str(resources_dir / "hex_11_inputsdbp_inv_straight_v0_manual.sqd"),
        "inverter_input_0",
    )

    params = critical_temperature_params()

    params.operational_params.sim_engine = sidb_simulation_engine.QUICKSIM
    params.operational_params.simulation_parameters.base = 2  # QuickSim simulates two charge states only

    stats = critical_temperature_stats()
    spec = [create_not_tt()]

    assert critical_temperature_gate_based(layout, spec, params, stats) <= 400

    assert stats.algorithm_name == "QuickSim"
    assert stats.num_valid_lyt > 1


def test_bestagon_inv_with_different_mu(resources_dir):
    layout = read_sqd_layout(
        str(resources_dir / "hex_11_inputsdbp_inv_straight_v0_manual.sqd"),
        "inverter_input_0",
    )

    params = critical_temperature_params()
    params.operational_params.simulation_parameters.base = 2
    params.operational_params.simulation_parameters.mu_minus = -0.2

    params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT

    stats = critical_temperature_stats()
    spec = [create_not_tt()]

    assert critical_temperature_gate_based(layout, spec, params, stats) <= 5

    assert stats.algorithm_name == "QuickExact"


def test_critical_temperature_with_input_pattern_layouts():
    lyt = sidb_layout()

    lyt.assign_cell_type(lattice_site(0, 0, 1), sidb_technology.cell_type.INPUT)
    lyt.assign_cell_type(lattice_site(2, 1, 1), sidb_technology.cell_type.INPUT)

    lyt.assign_cell_type(lattice_site(20, 0, 1), sidb_technology.cell_type.INPUT)
    lyt.assign_cell_type(lattice_site(19, 1, 1), sidb_technology.cell_type.INPUT)

    lyt.assign_cell_type(lattice_site(4, 2, 1), sidb_technology.cell_type.NORMAL)
    lyt.assign_cell_type(lattice_site(6, 3, 1), sidb_technology.cell_type.NORMAL)

    lyt.assign_cell_type(lattice_site(14, 3, 1), sidb_technology.cell_type.NORMAL)
    lyt.assign_cell_type(lattice_site(16, 2, 1), sidb_technology.cell_type.NORMAL)

    lyt.assign_cell_type(lattice_site(10, 6, 0), sidb_technology.cell_type.OUTPUT)
    lyt.assign_cell_type(lattice_site(10, 7, 0), sidb_technology.cell_type.OUTPUT)

    lyt.assign_cell_type(lattice_site(10, 9, 1), sidb_technology.cell_type.NORMAL)

    params = critical_temperature_params()
    params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
    params.operational_params.simulation_parameters = sidb_simulation_parameters(2, -0.28)

    input_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params(), bdl_wire_selection.INPUT)
    output_bdl_wires = detect_bdl_wires(lyt, detect_bdl_wires_params(), bdl_wire_selection.OUTPUT)
    output_bdl_pairs = detect_bdl_pairs(lyt, sidb_technology.cell_type.OUTPUT, detect_bdl_pairs_params())

    input_pattern_layouts = generate_bdl_input_pattern_layouts(
        lyt,
        bdl_input_iterator_params(),
        input_bdl_wires,
    )

    # a 2-input gate has 4 input patterns
    assert len(input_pattern_layouts) == 4

    reference_stats = critical_temperature_stats()
    reference_ct = critical_temperature_gate_based(lyt, [create_and_tt()], params, reference_stats)

    stats = critical_temperature_stats()
    ct = critical_temperature_gate_based(
        input_pattern_layouts,
        [create_and_tt()],
        params,
        output_bdl_pairs,
        input_bdl_wires,
        output_bdl_wires,
        stats,
    )

    # the two overloads run the same computation, so the results must be identical
    assert ct == reference_ct
    assert stats.num_valid_lyt == reference_stats.num_valid_lyt
    assert stats.algorithm_name == reference_stats.algorithm_name

    # a layout list that does not match the specification is rejected
    with pytest.raises(ValueError, match="expected 4 input pattern layouts"):
        critical_temperature_gate_based(
            input_pattern_layouts[:1],
            [create_and_tt()],
            params,
            output_bdl_pairs,
            input_bdl_wires,
            output_bdl_wires,
        )

    # more output BDL pairs than truth tables is rejected
    with pytest.raises(ValueError, match="expected 1 output BDL pairs"):
        critical_temperature_gate_based(
            input_pattern_layouts,
            [create_and_tt()],
            params,
            [*output_bdl_pairs, output_bdl_pairs[0]],
            input_bdl_wires,
            output_bdl_wires,
        )
