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

from mnt.pyfiction.sidb import DotTag, LatticeSite, SiDBLayout
from mnt.pyfiction.sidb.analysis import (
    CriticalTemperatureDomain,
    InputEncoding,
    OperationalAnalysisStrategy,
    OperationalCondition,
    OperationalDomain,
    OperationalDomainParams,
    OperationalStatus,
    ParameterPoint,
    SweepParameter,
    SweepRange,
    critical_temperature_domain_contour_tracing,
    critical_temperature_domain_flood_fill,
    critical_temperature_domain_grid_search,
    critical_temperature_domain_random_sampling,
    operational_domain_contour_tracing,
    operational_domain_flood_fill,
    operational_domain_grid_search,
    operational_domain_random_sampling,
)
from mnt.pyfiction.sidb.io import read_sqd_layout
from mnt.pyfiction.sidb.simulation import SimulationEngine
from mnt.pyfiction.synthesis import (
    standard_functions,
)

if TYPE_CHECKING:
    from pathlib import Path


@pytest.fixture
def wire_with_canvas() -> SiDBLayout:
    """A BDL wire with two LOGIC dots, so that the sketch has a canvas to enumerate.

    Returns:
        The wire layout.
    """
    lyt = SiDBLayout()

    lyt.assign_sidb(LatticeSite(0, 0, 0), DotTag.INPUT)
    lyt.assign_sidb(LatticeSite(2, 0, 1), DotTag.INPUT)

    lyt.assign_sidb(LatticeSite(6, 1, 0), DotTag.NORMAL)
    lyt.assign_sidb(LatticeSite(8, 1, 1), DotTag.NORMAL)
    lyt.assign_sidb(LatticeSite(12, 2, 0), DotTag.NORMAL)
    lyt.assign_sidb(LatticeSite(14, 2, 1), DotTag.NORMAL)

    lyt.assign_sidb(LatticeSite(11, 3, 1), DotTag.LOGIC)
    lyt.assign_sidb(LatticeSite(13, 6, 1), DotTag.LOGIC)

    lyt.assign_sidb(LatticeSite(14, 7, 1), DotTag.NORMAL)
    lyt.assign_sidb(LatticeSite(12, 8, 0), DotTag.NORMAL)

    lyt.assign_sidb(LatticeSite(8, 8, 1), DotTag.OUTPUT)
    lyt.assign_sidb(LatticeSite(6, 9, 0), DotTag.OUTPUT)

    lyt.assign_sidb(LatticeSite(2, 9, 1), DotTag.NORMAL)

    return lyt


def test_operational_domain_siqad_or_100_lattice(resources_dir):
    lyt = read_sqd_layout(str(resources_dir / "siqad_or_gate.sqd"))

    params = OperationalDomainParams()
    params.operational_params.sim_engine = SimulationEngine.QUICKEXACT
    params.operational_params.simulation_parameters.base = 2

    params.operational_params.simulation_parameters.mu_minus = -0.28
    params.operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = 1.5

    params.operational_params.op_condition = OperationalCondition.TOLERATE_KINKS

    params.sweep_dimensions = [
        SweepRange(SweepParameter.EPSILON_R, 5.70, 6.70, 0.01),
        SweepRange(SweepParameter.LAMBDA_TF, 3.00, 4.00, 0.01),
    ]

    result = operational_domain_grid_search(lyt, [standard_functions("or")[0]], params=params)
    stats_grid = result.stats
    assert stats_grid.num_operational_parameter_combinations == 10201


def test_number_of_threads(resources_dir):
    """The thread count is configurable and does not change the resulting operational domain."""
    lyt = read_sqd_layout(str(resources_dir / "siqad_or_gate.sqd"))

    params = OperationalDomainParams()
    params.operational_params.sim_engine = SimulationEngine.QUICKEXACT
    params.operational_params.simulation_parameters.base = 2
    params.operational_params.simulation_parameters.mu_minus = -0.28
    params.operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = 1.5
    params.operational_params.op_condition = OperationalCondition.TOLERATE_KINKS

    params.sweep_dimensions = [
        SweepRange(SweepParameter.EPSILON_R, 5.70, 5.80, 0.01),
        SweepRange(SweepParameter.LAMBDA_TF, 3.00, 3.10, 0.01),
    ]

    # defaults to the number of hardware threads
    assert params.number_of_threads >= 1

    result = operational_domain_grid_search(lyt, [standard_functions("or")[0]], params=params)
    stats_default = result.stats

    params.number_of_threads = 1

    result = operational_domain_grid_search(lyt, [standard_functions("or")[0]], params=params)
    stats_single = result.stats

    assert stats_single.num_operational_parameter_combinations == stats_default.num_operational_parameter_combinations
    assert stats_single.num_evaluated_parameter_combinations == stats_default.num_evaluated_parameter_combinations


def test_three_dimensional_operational_domain_sketch(wire_with_canvas):
    """The sketch and the boundary-following strategies work over three sweep dimensions."""
    lyt = wire_with_canvas

    params = OperationalDomainParams()
    params.operational_params.sim_engine = SimulationEngine.QUICKEXACT
    params.operational_params.simulation_parameters.base = 2
    params.operational_params.op_condition = OperationalCondition.REJECT_KINKS
    params.operational_params.strategy_to_analyze_operational_status = OperationalAnalysisStrategy.FILTER_ONLY

    params.sweep_dimensions = [
        SweepRange(SweepParameter.EPSILON_R, 5.5, 5.7, 0.1),
        SweepRange(SweepParameter.LAMBDA_TF, 5.0, 5.2, 0.1),
        SweepRange(SweepParameter.MU_MINUS, -0.32, -0.30, 0.02),
    ]

    # 3 x 3 x 2 parameter points
    result = operational_domain_grid_search(lyt, [standard_functions("id")[0]], params=params)
    grid_domain = result.domain
    stats_grid = result.stats
    assert stats_grid.num_evaluated_parameter_combinations == 18
    assert len(grid_domain) == 18

    # flood fill and contour tracing both accept three dimensions. They sample the same grid, so every point they
    # report must carry the status the exhaustive search determined for it
    flood_domain = operational_domain_flood_fill(lyt, [standard_functions("id")[0]], 4, params=params).domain
    contour_domain = operational_domain_contour_tracing(lyt, [standard_functions("id")[0]], 4, params=params).domain

    for domain in (flood_domain, contour_domain):
        assert len(domain) > 0
        assert len(domain) <= 18

        for point in domain:
            assert point in grid_domain
            assert domain[point] == grid_domain[point]


def test_operational_domain_sketch_preconditions(wire_with_canvas, resources_dir):
    """The sketch is rejected when it cannot filter anything."""
    lyt = read_sqd_layout(str(resources_dir / "siqad_or_gate.sqd"))

    params = OperationalDomainParams()
    params.operational_params.sim_engine = SimulationEngine.QUICKEXACT
    params.operational_params.strategy_to_analyze_operational_status = OperationalAnalysisStrategy.FILTER_ONLY
    params.operational_params.op_condition = OperationalCondition.REJECT_KINKS
    params.sweep_dimensions = [
        SweepRange(SweepParameter.EPSILON_R, 5.5, 5.6, 0.1),
        SweepRange(SweepParameter.LAMBDA_TF, 5.0, 5.1, 0.1),
    ]

    # the layout has no LOGIC dots, so there is no canvas for the filtering steps to enumerate
    with pytest.raises(ValueError, match="requires a canvas"):
        operational_domain_grid_search(lyt, [standard_functions("or")[0]], params=params)

    # tolerating kinks leaves the filtering steps undefined. This uses a layout that does have a canvas, so that
    # the rejection can only come from the kink condition
    canvas_lyt = wire_with_canvas

    params.operational_params.op_condition = OperationalCondition.TOLERATE_KINKS
    with pytest.raises(ValueError, match="requires that kinks are rejected"):
        operational_domain_grid_search(canvas_lyt, [standard_functions("id")[0]], params=params)

    # the same layout is accepted once kinks are rejected again
    params.operational_params.op_condition = OperationalCondition.REJECT_KINKS
    operational_domain_grid_search(canvas_lyt, [standard_functions("id")[0]], params=params)


def test_operational_domain_xor_gate_100_lattice(resources_dir):
    lyt = read_sqd_layout(str(resources_dir / "hex_21_inputsdbp_xor_v1.sqd"))

    params = OperationalDomainParams()
    params.operational_params.sim_engine = SimulationEngine.QUICKEXACT
    params.operational_params.simulation_parameters.base = 2

    params.sweep_dimensions = [
        SweepRange(SweepParameter.EPSILON_R, 5.55, 5.65, 0.01),
        SweepRange(SweepParameter.LAMBDA_TF, 4.95, 5.05, 0.01),
    ]

    result = operational_domain_grid_search(lyt, [standard_functions("xor")[0]], params=params)
    stats_grid = result.stats
    assert stats_grid.num_operational_parameter_combinations > 0

    result = operational_domain_flood_fill(lyt, [standard_functions("xor")[0]], 100, params=params)
    stats_flood_fill = result.stats
    assert stats_flood_fill.num_operational_parameter_combinations > 0

    result = operational_domain_random_sampling(lyt, [standard_functions("xor")[0]], 100, params=params)
    stats_random_sampling = result.stats
    assert stats_random_sampling.num_operational_parameter_combinations > 0

    result = operational_domain_contour_tracing(lyt, [standard_functions("xor")[0]], 100, params=params)
    stats_contour_tracing = result.stats
    assert stats_contour_tracing.num_operational_parameter_combinations > 0


def test_critical_temperature_domain_xor_gate_100_lattice(resources_dir: Path) -> None:
    """Critical-temperature searches agree on every evaluated XOR parameter point."""
    lyt = read_sqd_layout(str(resources_dir / "hex_21_inputsdbp_xor_v1.sqd"))

    params = OperationalDomainParams()
    params.operational_params.sim_engine = SimulationEngine.QUICKEXACT
    params.operational_params.simulation_parameters.base = 2

    params.sweep_dimensions = [
        SweepRange(SweepParameter.EPSILON_R, 5.55, 5.65, 0.01),
        SweepRange(SweepParameter.LAMBDA_TF, 4.95, 5.05, 0.01),
    ]

    result = critical_temperature_domain_grid_search(lyt, [standard_functions("xor")[0]], params=params)
    ct_domain_grid = result.domain
    stats_grid = result.stats
    assert ct_domain_grid[ParameterPoint([5.6, 5.0])][0] == OperationalStatus.OPERATIONAL
    assert ct_domain_grid[ParameterPoint([5.6, 5.0])][1] > 30
    assert stats_grid.num_operational_parameter_combinations > 0
    assert ct_domain_grid.minimum_ct() > 23
    assert ct_domain_grid.maximum_ct() < 38

    result = critical_temperature_domain_flood_fill(lyt, [standard_functions("xor")[0]], 100, params=params)
    ct_domain_flood = result.domain
    stats_flood_fill = result.stats
    assert ct_domain_flood[ParameterPoint([5.6, 5.0])][0] == OperationalStatus.OPERATIONAL
    assert ct_domain_flood[ParameterPoint([5.6, 5.0])][1] > 30
    assert stats_flood_fill.num_operational_parameter_combinations > 0

    result = critical_temperature_domain_contour_tracing(lyt, [standard_functions("xor")[0]], 1000, params=params)
    ct_domain_contour = result.domain
    stats_contour_tracing = result.stats
    assert len(ct_domain_contour) > 0
    for point, (status, temperature) in ct_domain_contour.items():
        expected_status, expected_temperature = ct_domain_grid[point]
        assert status == expected_status
        assert temperature == pytest.approx(expected_temperature)
    assert stats_contour_tracing.num_operational_parameter_combinations > 0

    params.sweep_dimensions = [
        SweepRange(SweepParameter.EPSILON_R, 5.60, 5.60, 0.01),
        SweepRange(SweepParameter.LAMBDA_TF, 5.00, 5.00, 0.01),
    ]

    result = critical_temperature_domain_random_sampling(lyt, [standard_functions("xor")[0]], 1000, params=params)
    ct_domain_random = result.domain
    stats_random_sampling = result.stats
    assert ct_domain_random[ParameterPoint([5.6, 5.0])][0] == OperationalStatus.OPERATIONAL
    assert ct_domain_random[ParameterPoint([5.6, 5.0])][1] > 30
    assert stats_random_sampling.num_operational_parameter_combinations > 0


def test_operational_domain_and_gate_111_lattice(resources_dir):
    lyt = read_sqd_layout(str(resources_dir / "AND_mu_032_111_surface.sqd"))

    params = OperationalDomainParams()
    params.operational_params.sim_engine = SimulationEngine.QUICKEXACT
    params.operational_params.simulation_parameters.base = 2

    params.sweep_dimensions = [
        SweepRange(SweepParameter.EPSILON_R, 5.60, 5.64, 0.01),
        SweepRange(SweepParameter.LAMBDA_TF, 5.00, 5.01, 0.01),
    ]

    result = operational_domain_grid_search(lyt, [standard_functions("and")[0]], params=params)
    stats_grid = result.stats
    assert stats_grid.num_operational_parameter_combinations > 0

    result = operational_domain_flood_fill(lyt, [standard_functions("and")[0]], 100, params=params)
    stats_flood_fill = result.stats
    assert stats_flood_fill.num_operational_parameter_combinations > 0

    result = operational_domain_random_sampling(lyt, [standard_functions("and")[0]], 100, params=params)
    stats_random_sampling = result.stats
    assert stats_random_sampling.num_operational_parameter_combinations > 0

    result = operational_domain_contour_tracing(lyt, [standard_functions("and")[0]], 1000, params=params)
    stats_contour_tracing = result.stats
    assert stats_contour_tracing.num_operational_parameter_combinations > 0


def test_temperature_operational_domain():
    # Create an instance of critical_temperature_domain
    temp_domain = CriticalTemperatureDomain([SweepParameter.EPSILON_R, SweepParameter.LAMBDA_TF])

    # Create test key and value
    key = ParameterPoint([1.0, 2.0])
    value = (OperationalStatus.OPERATIONAL, 0.1)

    # Add a value to the domain using __setitem__
    temp_domain[key] = value

    # __getitem__ should return the value for an existing key
    assert temp_domain[key] == value

    # __getitem__ should raise KeyError for missing key
    missing_key = ParameterPoint([4.0, 5.0])
    with pytest.raises(KeyError):
        _ = temp_domain[missing_key]

    # __setitem__ should add or update a value
    new_key = ParameterPoint([3.3, 4.4])
    new_value = (OperationalStatus.NON_OPERATIONAL, 0.0)
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
    assert temp_domain.get_dimension(0) == SweepParameter.EPSILON_R
    assert temp_domain.get_dimension(1) == SweepParameter.LAMBDA_TF


def test_operational_domain():
    # Create an instance of operational_domain
    op_domain = OperationalDomain([SweepParameter.EPSILON_R, SweepParameter.LAMBDA_TF])

    # Create test key and value
    key = ParameterPoint([10.0, 20.0])
    value = OperationalStatus.NON_OPERATIONAL

    # Add a value to the domain
    op_domain[key] = value

    # Test retrieving a value that doesn't exist
    missing_key = ParameterPoint([7.0, 8.0])
    assert missing_key not in op_domain

    # Modify dimensions and verify
    assert op_domain.get_dimension(0) == SweepParameter.EPSILON_R
    assert op_domain.get_dimension(1) == SweepParameter.LAMBDA_TF

    # __getitem__ should return the value for an existing key
    assert op_domain[key] == value

    # __getitem__ should raise KeyError for missing key
    with pytest.raises(KeyError):
        _ = op_domain[missing_key]

    # __setitem__ should add or update a value
    new_key = ParameterPoint([1.1, 2.2])
    new_value = OperationalStatus.OPERATIONAL
    op_domain[new_key] = OperationalStatus.OPERATIONAL
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
    wire_layout = SiDBLayout()

    wire_layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.INPUT)
    wire_layout.assign_sidb(LatticeSite(2, 0, 0), DotTag.INPUT)

    wire_layout.assign_sidb(LatticeSite(6, 0, 0), DotTag.NORMAL)
    wire_layout.assign_sidb(LatticeSite(8, 0, 0), DotTag.NORMAL)

    wire_layout.assign_sidb(LatticeSite(12, 0, 0), DotTag.OUTPUT)
    wire_layout.assign_sidb(LatticeSite(14, 0, 0), DotTag.OUTPUT)

    wire_layout.assign_sidb(LatticeSite(18, 0, 0), DotTag.NORMAL)

    params = OperationalDomainParams()
    params.operational_params.sim_engine = SimulationEngine.QUICKEXACT
    params.operational_params.simulation_parameters.base = 2
    params.operational_params.input_bdl_iterator_params.input_bdl_config = InputEncoding.PERTURBER_DISTANCE_ENCODED

    params.sweep_dimensions = [
        SweepRange(SweepParameter.EPSILON_R, 1.0, 10.0, 0.1),
        SweepRange(SweepParameter.LAMBDA_TF, 1.0, 10.0, 0.1),
    ]

    result = operational_domain_grid_search(wire_layout, [standard_functions("id")[0]], params=params)
    op_domain = result.domain
    stats_grid = result.stats

    assert len(op_domain) == 8281

    assert stats_grid.num_simulator_invocations == 10034
    assert stats_grid.num_evaluated_parameter_combinations == 8281
    assert stats_grid.num_operational_parameter_combinations == 0
    assert stats_grid.num_non_operational_parameter_combinations == 8281


@pytest.mark.parametrize("strategy", ["grid", "flood"])
def test_domain_reports_progress(resources_dir: Path, strategy: str) -> None:
    """The parameter points are reported as they are evaluated, ending at the grid size."""
    lyt = read_sqd_layout(str(resources_dir / "siqad_or_gate.sqd"))

    params = OperationalDomainParams()
    params.operational_params.sim_engine = SimulationEngine.QUICKEXACT
    params.operational_params.simulation_parameters.base = 2
    params.operational_params.simulation_parameters.mu_minus = -0.28
    params.operational_params.input_bdl_iterator_params.bdl_wire_params.threshold_bdl_interdistance = 1.5
    params.operational_params.op_condition = OperationalCondition.TOLERATE_KINKS
    params.sweep_dimensions = [
        SweepRange(SweepParameter.EPSILON_R, 5.70, 5.80, 0.01),
        SweepRange(SweepParameter.LAMBDA_TF, 3.00, 3.10, 0.01),
    ]
    params.number_of_threads = 2

    reports = []
    params.on_progress = lambda task, done, total: reports.append((task, done, total))
    workers = []
    params.on_worker_progress = lambda *report: workers.append(report)

    if strategy == "grid":
        result = operational_domain_grid_search(lyt, [standard_functions("or")[0]], params=params)
        stats = result.stats
    else:
        result = operational_domain_flood_fill(lyt, [standard_functions("or")[0]], 1, params=params)
        stats = result.stats

    points = [(done, total) for task, done, total in reports if task == "parameter points"]
    assert points[0] == (0, 0)  # the total is unknown until the grid is set up
    assert points == sorted(points)
    assert points[-1][0] == stats.num_evaluated_parameter_combinations == 121
    assert points[-1][1] == (121 if strategy == "grid" else 0)

    finished = [(done, total) for worker, count, description, done, total, active in workers if not active]
    if strategy == "grid":
        assert len(finished) == params.number_of_threads
    assert sum(done for done, total in finished) == stats.num_evaluated_parameter_combinations
    assert all(total in (done, 0) for done, total in finished)
    if strategy == "flood":
        assert any("exploring" in description and total == 0 for _, _, description, _, total, _ in workers)
