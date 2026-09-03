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
    create_id_tt,
    create_or_tt,
    create_xor_tt,
    critical_temperature_domain,
    critical_temperature_domain_contour_tracing,
    critical_temperature_domain_flood_fill,
    critical_temperature_domain_grid_search,
    critical_temperature_domain_random_sampling,
    input_bdl_configuration,
    lattice_site,
    operational_analysis_strategy,
    operational_condition,
    operational_domain,
    operational_domain_contour_tracing,
    operational_domain_flood_fill,
    operational_domain_grid_search,
    operational_domain_params,
    operational_domain_random_sampling,
    operational_domain_stats,
    operational_domain_value_range,
    operational_status,
    parameter_point,
    read_sqd_layout,
    sidb_layout,
    sidb_simulation_engine,
    sidb_technology,
    sweep_parameter,
)


@pytest.fixture
def wire_with_canvas() -> sidb_layout:
    """A BDL wire with two LOGIC cells, so that the sketch has a canvas to enumerate.

    Returns:
        The wire layout.
    """
    lyt = sidb_layout()

    lyt.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.INPUT)
    lyt.assign_cell_type(lattice_site(2, 0, 1), sidb_technology.cell_type.INPUT)

    lyt.assign_cell_type(lattice_site(6, 1, 0), sidb_technology.cell_type.NORMAL)
    lyt.assign_cell_type(lattice_site(8, 1, 1), sidb_technology.cell_type.NORMAL)
    lyt.assign_cell_type(lattice_site(12, 2, 0), sidb_technology.cell_type.NORMAL)
    lyt.assign_cell_type(lattice_site(14, 2, 1), sidb_technology.cell_type.NORMAL)

    lyt.assign_cell_type(lattice_site(11, 3, 1), sidb_technology.cell_type.LOGIC)
    lyt.assign_cell_type(lattice_site(13, 6, 1), sidb_technology.cell_type.LOGIC)

    lyt.assign_cell_type(lattice_site(14, 7, 1), sidb_technology.cell_type.NORMAL)
    lyt.assign_cell_type(lattice_site(12, 8, 0), sidb_technology.cell_type.NORMAL)

    lyt.assign_cell_type(lattice_site(8, 8, 1), sidb_technology.cell_type.OUTPUT)
    lyt.assign_cell_type(lattice_site(6, 9, 0), sidb_technology.cell_type.OUTPUT)

    lyt.assign_cell_type(lattice_site(2, 9, 1), sidb_technology.cell_type.NORMAL)

    return lyt


def test_operational_domain_siqad_or_100_lattice(resources_dir):
    lyt = read_sqd_layout(str(resources_dir / "siqad_or_gate.sqd"))

    params = operational_domain_params()
    params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
    params.operational_params.simulation_parameters.base = 2

    params.operational_params.simulation_parameters.mu_minus = -0.28
    params.operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = 1.5

    params.operational_params.op_condition = operational_condition.TOLERATE_KINKS

    params.sweep_dimensions = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 5.70, 6.70, 0.01),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 3.00, 4.00, 0.01),
    ]

    stats_grid = operational_domain_stats()
    operational_domain_grid_search(lyt, [create_or_tt()], params, stats_grid)
    assert stats_grid.num_operational_parameter_combinations == 10201


def test_number_of_threads(resources_dir):
    """The thread count is configurable and does not change the resulting operational domain."""
    lyt = read_sqd_layout(str(resources_dir / "siqad_or_gate.sqd"))

    params = operational_domain_params()
    params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
    params.operational_params.simulation_parameters.base = 2
    params.operational_params.simulation_parameters.mu_minus = -0.28
    params.operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = 1.5
    params.operational_params.op_condition = operational_condition.TOLERATE_KINKS

    params.sweep_dimensions = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 5.70, 5.80, 0.01),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 3.00, 3.10, 0.01),
    ]

    # defaults to the number of hardware threads
    assert params.number_of_threads >= 1

    stats_default = operational_domain_stats()
    operational_domain_grid_search(lyt, [create_or_tt()], params, stats_default)

    params.number_of_threads = 1

    stats_single = operational_domain_stats()
    operational_domain_grid_search(lyt, [create_or_tt()], params, stats_single)

    assert stats_single.num_operational_parameter_combinations == stats_default.num_operational_parameter_combinations
    assert stats_single.num_evaluated_parameter_combinations == stats_default.num_evaluated_parameter_combinations


def test_three_dimensional_operational_domain_sketch(wire_with_canvas):
    """The sketch and the boundary-following strategies work over three sweep dimensions."""
    lyt = wire_with_canvas

    params = operational_domain_params()
    params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
    params.operational_params.simulation_parameters.base = 2
    params.operational_params.op_condition = operational_condition.REJECT_KINKS
    params.operational_params.strategy_to_analyze_operational_status = operational_analysis_strategy.FILTER_ONLY

    params.sweep_dimensions = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 5.5, 5.7, 0.1),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.0, 5.2, 0.1),
        operational_domain_value_range(sweep_parameter.MU_MINUS, -0.32, -0.30, 0.02),
    ]

    # 3 x 3 x 2 parameter points
    stats_grid = operational_domain_stats()
    grid_domain = operational_domain_grid_search(lyt, [create_id_tt()], params, stats_grid)
    assert stats_grid.num_evaluated_parameter_combinations == 18
    assert len(grid_domain) == 18

    # flood fill and contour tracing both accept three dimensions. They sample the same grid, so every point they
    # report must carry the status the exhaustive search determined for it
    flood_domain = operational_domain_flood_fill(lyt, [create_id_tt()], 4, params)
    contour_domain = operational_domain_contour_tracing(lyt, [create_id_tt()], 4, params)

    for domain in (flood_domain, contour_domain):
        assert len(domain) > 0
        assert len(domain) <= 18

        for point in domain:
            assert point in grid_domain
            assert domain[point] == grid_domain[point]


def test_operational_domain_sketch_preconditions(wire_with_canvas, resources_dir):
    """The sketch is rejected when it cannot filter anything."""
    lyt = read_sqd_layout(str(resources_dir / "siqad_or_gate.sqd"))

    params = operational_domain_params()
    params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
    params.operational_params.strategy_to_analyze_operational_status = operational_analysis_strategy.FILTER_ONLY
    params.operational_params.op_condition = operational_condition.REJECT_KINKS
    params.sweep_dimensions = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 5.5, 5.6, 0.1),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.0, 5.1, 0.1),
    ]

    # the layout has no LOGIC cells, so there is no canvas for the filtering steps to enumerate
    with pytest.raises(ValueError, match="requires a canvas"):
        operational_domain_grid_search(lyt, [create_or_tt()], params)

    # tolerating kinks leaves the filtering steps undefined. This uses a layout that does have a canvas, so that
    # the rejection can only come from the kink condition
    canvas_lyt = wire_with_canvas

    params.operational_params.op_condition = operational_condition.TOLERATE_KINKS
    with pytest.raises(ValueError, match="requires that kinks are rejected"):
        operational_domain_grid_search(canvas_lyt, [create_id_tt()], params)

    # the same layout is accepted once kinks are rejected again
    params.operational_params.op_condition = operational_condition.REJECT_KINKS
    operational_domain_grid_search(canvas_lyt, [create_id_tt()], params)


def test_operational_domain_xor_gate_100_lattice(resources_dir):
    lyt = read_sqd_layout(str(resources_dir / "hex_21_inputsdbp_xor_v1.sqd"))

    params = operational_domain_params()
    params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
    params.operational_params.simulation_parameters.base = 2

    params.sweep_dimensions = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 5.55, 5.65, 0.01),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 4.95, 5.05, 0.01),
    ]

    stats_grid = operational_domain_stats()
    operational_domain_grid_search(lyt, [create_xor_tt()], params, stats_grid)
    assert stats_grid.num_operational_parameter_combinations > 0

    stats_flood_fill = operational_domain_stats()
    operational_domain_flood_fill(lyt, [create_xor_tt()], 100, params, stats_flood_fill)
    assert stats_flood_fill.num_operational_parameter_combinations > 0

    stats_random_sampling = operational_domain_stats()
    operational_domain_random_sampling(lyt, [create_xor_tt()], 100, params, stats_random_sampling)
    assert stats_random_sampling.num_operational_parameter_combinations > 0

    stats_contour_tracing = operational_domain_stats()
    operational_domain_contour_tracing(lyt, [create_xor_tt()], 100, params, stats_contour_tracing)
    assert stats_contour_tracing.num_operational_parameter_combinations > 0


def test_critical_temperature_domain_xor_gate_100_lattice(resources_dir):
    lyt = read_sqd_layout(str(resources_dir / "hex_21_inputsdbp_xor_v1.sqd"))

    params = operational_domain_params()
    params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
    params.operational_params.simulation_parameters.base = 2

    params.sweep_dimensions = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 5.55, 5.65, 0.01),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 4.95, 5.05, 0.01),
    ]

    stats_grid = operational_domain_stats()
    ct_domain_grid = critical_temperature_domain_grid_search(lyt, [create_xor_tt()], params, stats_grid)
    assert ct_domain_grid[parameter_point([5.6, 5.0])][0] == operational_status.OPERATIONAL
    assert ct_domain_grid[parameter_point([5.6, 5.0])][1] > 30
    assert stats_grid.num_operational_parameter_combinations > 0
    assert ct_domain_grid.minimum_ct() > 23
    assert ct_domain_grid.maximum_ct() < 38

    stats_flood_fill = operational_domain_stats()
    ct_domain_flood = critical_temperature_domain_flood_fill(lyt, [create_xor_tt()], 100, params, stats_flood_fill)
    assert ct_domain_flood[parameter_point([5.6, 5.0])][0] == operational_status.OPERATIONAL
    assert ct_domain_flood[parameter_point([5.6, 5.0])][1] > 30
    assert stats_flood_fill.num_operational_parameter_combinations > 0

    stats_contour_tracing = operational_domain_stats()
    ct_domain_contour = critical_temperature_domain_contour_tracing(
        lyt,
        [create_xor_tt()],
        1000,
        params,
        stats_contour_tracing,
    )
    assert ct_domain_contour[parameter_point([5.6, 5.0])][0] == operational_status.OPERATIONAL
    assert ct_domain_contour[parameter_point([5.6, 5.0])][1] > 30
    assert stats_contour_tracing.num_operational_parameter_combinations > 0

    params.sweep_dimensions = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 5.60, 5.60, 0.01),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.00, 5.00, 0.01),
    ]

    stats_random_sampling = operational_domain_stats()
    ct_domain_random = critical_temperature_domain_random_sampling(
        lyt,
        [create_xor_tt()],
        1000,
        params,
        stats_random_sampling,
    )
    assert ct_domain_random[parameter_point([5.6, 5.0])][0] == operational_status.OPERATIONAL
    assert ct_domain_random[parameter_point([5.6, 5.0])][1] > 30
    assert stats_random_sampling.num_operational_parameter_combinations > 0


def test_operational_domain_and_gate_111_lattice(resources_dir):
    lyt = read_sqd_layout(str(resources_dir / "AND_mu_032_111_surface.sqd"))

    params = operational_domain_params()
    params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
    params.operational_params.simulation_parameters.base = 2

    params.sweep_dimensions = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 5.60, 5.64, 0.01),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.00, 5.01, 0.01),
    ]

    stats_grid = operational_domain_stats()
    operational_domain_grid_search(lyt, [create_and_tt()], params, stats_grid)
    assert stats_grid.num_operational_parameter_combinations > 0

    stats_flood_fill = operational_domain_stats()
    operational_domain_flood_fill(lyt, [create_and_tt()], 100, params, stats_flood_fill)
    assert stats_flood_fill.num_operational_parameter_combinations > 0

    stats_random_sampling = operational_domain_stats()
    operational_domain_random_sampling(lyt, [create_and_tt()], 100, params, stats_random_sampling)
    assert stats_random_sampling.num_operational_parameter_combinations > 0

    stats_contour_tracing = operational_domain_stats()
    operational_domain_contour_tracing(lyt, [create_and_tt()], 1000, params, stats_contour_tracing)
    assert stats_contour_tracing.num_operational_parameter_combinations > 0


def test_temperature_operational_domain():
    # Create an instance of critical_temperature_domain
    temp_domain = critical_temperature_domain([sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])

    # Create test key and value
    key = parameter_point([1.0, 2.0])
    value = (operational_status.OPERATIONAL, 0.1)

    # Add a value to the domain using __setitem__
    temp_domain[key] = value

    # __getitem__ should return the value for an existing key
    assert temp_domain[key] == value

    # __getitem__ should raise KeyError for missing key
    missing_key = parameter_point([4.0, 5.0])
    with pytest.raises(KeyError):
        _ = temp_domain[missing_key]

    # __setitem__ should add or update a value
    new_key = parameter_point([3.3, 4.4])
    new_value = (operational_status.NON_OPERATIONAL, 0.0)
    temp_domain[new_key] = new_value
    assert temp_domain[new_key] == new_value

    # __contains__ should work for present and missing keys
    assert key in temp_domain
    assert new_key in temp_domain
    assert missing_key not in temp_domain

    # __len__ should reflect the number of items
    assert len(temp_domain) == 2

    # __iter__ should yield all keys
    keys = list(iter(temp_domain))
    assert key in keys
    assert new_key in keys
    assert set(keys) == set(temp_domain.keys())

    # keys() should return all keys
    keys_method = temp_domain.keys()
    assert set(keys_method) == {key, new_key}

    # values() should return all values
    values_method = temp_domain.values()
    assert value in values_method
    assert new_value in values_method
    assert len(values_method) == 2

    # items() should return all (key, value) pairs
    items_method = temp_domain.items()
    assert (key, value) in items_method
    assert (new_key, new_value) in items_method
    assert len(items_method) == 2

    # Test retrieving a value that doesn't exist using contains (should return None)
    assert missing_key not in temp_domain

    # Modify dimensions and verify
    assert temp_domain.get_dimension(0) == sweep_parameter.EPSILON_R
    assert temp_domain.get_dimension(1) == sweep_parameter.LAMBDA_TF


def test_operational_domain():
    # Create an instance of operational_domain
    op_domain = operational_domain([sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])

    # Create test key and value
    key = parameter_point([10.0, 20.0])
    value = operational_status.NON_OPERATIONAL

    # Add a value to the domain
    op_domain[key] = value

    # Test retrieving a value that doesn't exist
    missing_key = parameter_point([7.0, 8.0])
    assert missing_key not in op_domain

    # Modify dimensions and verify
    assert op_domain.get_dimension(0) == sweep_parameter.EPSILON_R
    assert op_domain.get_dimension(1) == sweep_parameter.LAMBDA_TF

    # __getitem__ should return the value for an existing key
    assert op_domain[key] == value

    # __getitem__ should raise KeyError for missing key
    with pytest.raises(KeyError):
        _ = op_domain[missing_key]

    # __setitem__ should add or update a value
    new_key = parameter_point([1.1, 2.2])
    new_value = operational_status.OPERATIONAL
    op_domain[new_key] = operational_status.OPERATIONAL
    assert op_domain[new_key] == new_value

    # __contains__ should work for present and missing keys
    assert key in op_domain
    assert new_key in op_domain
    assert missing_key not in op_domain

    # __len__ should reflect the number of items
    assert len(op_domain) == 2

    # __iter__ should yield all keys
    keys = list(iter(op_domain))
    assert key in keys
    assert new_key in keys
    assert set(keys) == set(op_domain.keys())

    # keys() should return all keys
    keys_method = op_domain.keys()
    assert set(keys_method) == {key, new_key}

    # values() should return all values
    values_method = op_domain.values()
    assert value in values_method
    assert new_value in values_method
    assert len(values_method) == 2

    # items() should return all (key, value) pairs
    items_method = op_domain.items()
    assert (key, value) in items_method
    assert (new_key, new_value) in items_method
    assert len(items_method) == 2


def test_operational_domain_two_bdl_pair_wire():
    bdl_wire = sidb_layout()

    bdl_wire.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.INPUT)
    bdl_wire.assign_cell_type(lattice_site(2, 0, 0), sidb_technology.cell_type.INPUT)

    bdl_wire.assign_cell_type(lattice_site(6, 0, 0), sidb_technology.cell_type.NORMAL)
    bdl_wire.assign_cell_type(lattice_site(8, 0, 0), sidb_technology.cell_type.NORMAL)

    bdl_wire.assign_cell_type(lattice_site(12, 0, 0), sidb_technology.cell_type.OUTPUT)
    bdl_wire.assign_cell_type(lattice_site(14, 0, 0), sidb_technology.cell_type.OUTPUT)

    bdl_wire.assign_cell_type(lattice_site(18, 0, 0), sidb_technology.cell_type.NORMAL)

    params = operational_domain_params()
    params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
    params.operational_params.simulation_parameters.base = 2
    params.operational_params.input_bdl_iterator_params.input_bdl_config = (
        input_bdl_configuration.PERTURBER_DISTANCE_ENCODED
    )

    params.sweep_dimensions = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 1.0, 10.0, 0.1),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 1.0, 10.0, 0.1),
    ]

    stats_grid = operational_domain_stats()
    op_domain = operational_domain_grid_search(bdl_wire, [create_id_tt()], params, stats_grid)

    assert len(op_domain) == 8281

    assert stats_grid.num_simulator_invocations == 10034
    assert stats_grid.num_evaluated_parameter_combinations == 8281
    assert stats_grid.num_operational_parameter_combinations == 0
    assert stats_grid.num_non_operational_parameter_combinations == 8281
