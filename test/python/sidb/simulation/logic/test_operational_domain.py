# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from collections import Counter
from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction.sidb import lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.io import read_sqd_layout
from mnt.pyfiction.sidb.simulation import sidb_simulation_engine
from mnt.pyfiction.sidb.simulation.logic import (
    critical_temperature_domain,
    critical_temperature_domain_contour_tracing,
    critical_temperature_domain_flood_fill,
    critical_temperature_domain_grid_search,
    critical_temperature_domain_random_sampling,
    input_bdl_configuration,
    operational_analysis_strategy,
    operational_condition,
    operational_domain,
    operational_domain_contour_tracing,
    operational_domain_flood_fill,
    operational_domain_grid_search,
    operational_domain_random_sampling,
    operational_domain_stats,
    operational_domain_value_range,
    operational_status,
    parameter_point,
    sweep_parameter,
)
from mnt.pyfiction.synthesis import create_and_tt, create_id_tt, create_or_tt, create_xor_tt

if TYPE_CHECKING:
    from collections.abc import Callable
    from pathlib import Path


@pytest.fixture
def wire_with_canvas() -> sidb_layout:
    """A BDL wire with two LOGIC dots, so that the sketch has a canvas to enumerate.

    Returns:
        The wire layout.
    """
    lyt = sidb_layout()

    lyt.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.INPUT)
    lyt.assign_sidb(lattice_site(2, 0, 1), sidb_dot_tag.INPUT)

    lyt.assign_sidb(lattice_site(6, 1, 0), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(8, 1, 1), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(12, 2, 0), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(14, 2, 1), sidb_dot_tag.NORMAL)

    lyt.assign_sidb(lattice_site(11, 3, 1), sidb_dot_tag.LOGIC)
    lyt.assign_sidb(lattice_site(13, 6, 1), sidb_dot_tag.LOGIC)

    lyt.assign_sidb(lattice_site(14, 7, 1), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(12, 8, 0), sidb_dot_tag.NORMAL)

    lyt.assign_sidb(lattice_site(8, 8, 1), sidb_dot_tag.OUTPUT)
    lyt.assign_sidb(lattice_site(6, 9, 0), sidb_dot_tag.OUTPUT)

    lyt.assign_sidb(lattice_site(2, 9, 1), sidb_dot_tag.NORMAL)

    return lyt


def test_parameter_point_extreme_values() -> None:
    nan = float("nan")
    points = {parameter_point([value]) for value in (1e300, 2e300, -1e300, float("inf"), -float("inf"), nan)}
    assert len(points) == 6
    assert parameter_point([nan]) in points


@pytest.mark.parametrize("domain_type", [operational_domain, critical_temperature_domain])
def test_domain_dimension_metadata(domain_type: type[operational_domain | critical_temperature_domain]) -> None:
    """Default domains have no axes; constructed domains retain their declared axis order."""
    domain = domain_type()
    assert len(domain) == 0
    assert domain.get_number_of_dimensions() == 0
    for index in (0, 1):
        with pytest.raises(IndexError):
            domain.get_dimension(index)
    for dimensions in (
        [sweep_parameter.LAMBDA_TF, sweep_parameter.MU_MINUS],
        [sweep_parameter.LAMBDA_TF, sweep_parameter.EPSILON_R, sweep_parameter.MU_MINUS],
    ):
        domain = domain_type(dimensions)
        assert domain.get_number_of_dimensions() == len(dimensions)
        assert [domain.get_dimension(index) for index in range(len(dimensions))] == dimensions
        with pytest.raises(IndexError):
            domain.get_dimension(len(dimensions))


def test_operational_domain_add_dimension() -> None:
    """Adding an axis updates its count and indexed lookup."""
    domain = operational_domain()
    domain.add_dimension(sweep_parameter.EPSILON_R)
    assert domain.get_number_of_dimensions() == 1
    assert domain.get_dimension(0) == sweep_parameter.EPSILON_R
    with pytest.raises(IndexError):
        domain.get_dimension(1)


def test_operational_domain_siqad_or_100_lattice(resources_dir: Path) -> None:
    lyt = read_sqd_layout(str(resources_dir / "siqad_or_gate.sqd"))

    options: dict[str, Any] = {
        "sim_engine": sidb_simulation_engine.QUICKEXACT,
        "base": 2,
    }

    options["mu_minus"] = -0.28
    options["threshold_bdl_interdistance"] = 1.5

    options["op_condition"] = operational_condition.TOLERATE_KINKS

    options["sweep_dimensions"] = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 5.70, 6.70, 0.01),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 3.00, 4.00, 0.01),
    ]

    stats_grid = operational_domain_stats()
    operational_domain_grid_search(lyt, [create_or_tt()], stats=stats_grid, **options)
    assert stats_grid.num_operational_parameter_combinations == 10201


def test_number_of_threads(resources_dir: Path) -> None:
    """The thread count is configurable and does not change the resulting operational domain."""
    lyt = read_sqd_layout(str(resources_dir / "siqad_or_gate.sqd"))

    options: dict[str, Any] = {
        "sim_engine": sidb_simulation_engine.QUICKEXACT,
        "base": 2,
        "mu_minus": -0.28,
        "threshold_bdl_interdistance": 1.5,
        "op_condition": operational_condition.TOLERATE_KINKS,
    }

    options["sweep_dimensions"] = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 5.70, 5.80, 0.01),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 3.00, 3.10, 0.01),
    ]

    # defaults to the number of hardware threads

    stats_default = operational_domain_stats()
    operational_domain_grid_search(lyt, [create_or_tt()], stats=stats_default, **options)

    options["number_of_threads"] = 1

    stats_single = operational_domain_stats()
    operational_domain_grid_search(lyt, [create_or_tt()], stats=stats_single, **options)

    assert stats_single.num_operational_parameter_combinations == stats_default.num_operational_parameter_combinations
    assert stats_single.num_evaluated_parameter_combinations == stats_default.num_evaluated_parameter_combinations


def test_three_dimensional_operational_domain_sketch(wire_with_canvas: sidb_layout) -> None:
    """The sketch and the boundary-following strategies work over three sweep dimensions."""
    lyt = wire_with_canvas

    options: dict[str, Any] = {
        "sim_engine": sidb_simulation_engine.QUICKEXACT,
        "base": 2,
        "op_condition": operational_condition.REJECT_KINKS,
        "strategy_to_analyze_operational_status": operational_analysis_strategy.FILTER_ONLY,
    }

    options["sweep_dimensions"] = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 5.5, 5.7, 0.1),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.0, 5.2, 0.1),
        operational_domain_value_range(sweep_parameter.MU_MINUS, -0.32, -0.30, 0.02),
    ]

    # 3 x 3 x 2 parameter points
    stats_grid = operational_domain_stats()
    grid_domain = operational_domain_grid_search(lyt, [create_id_tt()], stats=stats_grid, **options)
    assert stats_grid.num_evaluated_parameter_combinations == 18
    assert len(grid_domain) == 18

    # flood fill and contour tracing both accept three dimensions. They sample the same grid, so every point they
    # report must carry the status the exhaustive search determined for it
    flood_domain = operational_domain_flood_fill(lyt, [create_id_tt()], 4, **options)
    contour_domain = operational_domain_contour_tracing(lyt, [create_id_tt()], 4, **options)

    for domain in (flood_domain, contour_domain):
        assert len(domain) > 0
        assert len(domain) <= 18

        for point in domain:
            assert point in grid_domain
            assert domain[point] == grid_domain[point]


def test_operational_domain_sketch_preconditions(wire_with_canvas: sidb_layout, resources_dir: Path) -> None:
    """The sketch is rejected when it cannot filter anything."""
    lyt = read_sqd_layout(str(resources_dir / "siqad_or_gate.sqd"))

    options: dict[str, Any] = {
        "sim_engine": sidb_simulation_engine.QUICKEXACT,
        "strategy_to_analyze_operational_status": operational_analysis_strategy.FILTER_ONLY,
        "op_condition": operational_condition.REJECT_KINKS,
        "sweep_dimensions": [
            operational_domain_value_range(sweep_parameter.EPSILON_R, 5.5, 5.6, 0.1),
            operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.0, 5.1, 0.1),
        ],
    }

    # the layout has no LOGIC dots, so there is no canvas for the filtering steps to enumerate
    with pytest.raises(ValueError, match="requires a canvas"):
        operational_domain_grid_search(lyt, [create_or_tt()], **options)

    # tolerating kinks leaves the filtering steps undefined. This uses a layout that does have a canvas, so that
    # the rejection can only come from the kink condition
    canvas_lyt = wire_with_canvas

    options["op_condition"] = operational_condition.TOLERATE_KINKS
    with pytest.raises(ValueError, match="requires that kinks are rejected"):
        operational_domain_grid_search(canvas_lyt, [create_id_tt()], **options)

    # the same layout is accepted once kinks are rejected again
    options["op_condition"] = operational_condition.REJECT_KINKS
    operational_domain_grid_search(canvas_lyt, [create_id_tt()], **options)


def test_operational_domain_xor_gate_100_lattice(resources_dir: Path) -> None:
    lyt = read_sqd_layout(str(resources_dir / "hex_21_inputsdbp_xor_v1.sqd"))

    options: dict[str, Any] = {
        "sim_engine": sidb_simulation_engine.QUICKEXACT,
        "base": 2,
    }

    options["sweep_dimensions"] = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 5.55, 5.65, 0.01),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 4.95, 5.05, 0.01),
    ]

    stats_grid = operational_domain_stats()
    operational_domain_grid_search(lyt, [create_xor_tt()], stats=stats_grid, **options)
    assert stats_grid.num_operational_parameter_combinations > 0

    stats_flood_fill = operational_domain_stats()
    operational_domain_flood_fill(lyt, [create_xor_tt()], 100, stats=stats_flood_fill, **options)
    assert stats_flood_fill.num_operational_parameter_combinations > 0

    stats_random_sampling = operational_domain_stats()
    operational_domain_random_sampling(lyt, [create_xor_tt()], 100, stats=stats_random_sampling, **options)
    assert stats_random_sampling.num_operational_parameter_combinations > 0

    stats_contour_tracing = operational_domain_stats()
    operational_domain_contour_tracing(lyt, [create_xor_tt()], 100, stats=stats_contour_tracing, **options)
    assert stats_contour_tracing.num_operational_parameter_combinations > 0


def test_critical_temperature_domain_xor_gate_100_lattice(resources_dir: Path) -> None:
    """Critical-temperature searches agree on every evaluated XOR parameter point."""
    lyt = read_sqd_layout(str(resources_dir / "hex_21_inputsdbp_xor_v1.sqd"))

    options: dict[str, Any] = {
        "sim_engine": sidb_simulation_engine.QUICKEXACT,
        "base": 2,
    }

    options["sweep_dimensions"] = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 5.55, 5.65, 0.01),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 4.95, 5.05, 0.01),
    ]

    stats_grid = operational_domain_stats()
    ct_domain_grid = critical_temperature_domain_grid_search(lyt, [create_xor_tt()], stats=stats_grid, **options)
    assert ct_domain_grid[parameter_point([5.6, 5.0])][0] == operational_status.OPERATIONAL
    assert ct_domain_grid[parameter_point([5.6, 5.0])][1] > 30
    assert stats_grid.num_operational_parameter_combinations > 0
    assert ct_domain_grid.minimum_ct() > 23
    assert ct_domain_grid.maximum_ct() < 38

    stats_flood_fill = operational_domain_stats()
    ct_domain_flood = critical_temperature_domain_flood_fill(
        lyt, [create_xor_tt()], 100, stats=stats_flood_fill, **options
    )
    assert ct_domain_flood[parameter_point([5.6, 5.0])][0] == operational_status.OPERATIONAL
    assert ct_domain_flood[parameter_point([5.6, 5.0])][1] > 30
    assert stats_flood_fill.num_operational_parameter_combinations > 0

    stats_contour_tracing = operational_domain_stats()
    ct_domain_contour = critical_temperature_domain_contour_tracing(
        lyt, [create_xor_tt()], 1000, stats=stats_contour_tracing, **options
    )
    assert len(ct_domain_contour) > 0
    for point, (status, temperature) in ct_domain_contour.items():
        expected_status, expected_temperature = ct_domain_grid[point]
        assert status == expected_status
        assert temperature == pytest.approx(expected_temperature)
    assert stats_contour_tracing.num_operational_parameter_combinations > 0

    options["sweep_dimensions"] = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 5.60, 5.60, 0.01),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.00, 5.00, 0.01),
    ]

    stats_random_sampling = operational_domain_stats()
    ct_domain_random = critical_temperature_domain_random_sampling(
        lyt, [create_xor_tt()], 1000, stats=stats_random_sampling, **options
    )
    assert ct_domain_random[parameter_point([5.6, 5.0])][0] == operational_status.OPERATIONAL
    assert ct_domain_random[parameter_point([5.6, 5.0])][1] > 30
    assert stats_random_sampling.num_operational_parameter_combinations > 0


def test_operational_domain_and_gate_111_lattice(resources_dir: Path) -> None:
    lyt = read_sqd_layout(str(resources_dir / "AND_mu_032_111_surface.sqd"))

    options: dict[str, Any] = {
        "sim_engine": sidb_simulation_engine.QUICKEXACT,
        "base": 2,
    }

    options["sweep_dimensions"] = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 5.60, 5.64, 0.01),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.00, 5.01, 0.01),
    ]

    stats_grid = operational_domain_stats()
    operational_domain_grid_search(lyt, [create_and_tt()], stats=stats_grid, **options)
    assert stats_grid.num_operational_parameter_combinations > 0

    stats_flood_fill = operational_domain_stats()
    operational_domain_flood_fill(lyt, [create_and_tt()], 100, stats=stats_flood_fill, **options)
    assert stats_flood_fill.num_operational_parameter_combinations > 0

    stats_random_sampling = operational_domain_stats()
    operational_domain_random_sampling(lyt, [create_and_tt()], 100, stats=stats_random_sampling, **options)
    assert stats_random_sampling.num_operational_parameter_combinations > 0

    stats_contour_tracing = operational_domain_stats()
    operational_domain_contour_tracing(lyt, [create_and_tt()], 1000, stats=stats_contour_tracing, **options)
    assert stats_contour_tracing.num_operational_parameter_combinations > 0


@pytest.mark.parametrize(
    ("domain_type", "points", "values"),
    [
        (
            critical_temperature_domain,
            ((1.0, 2.0), (3.3, 4.4), (4.0, 5.0)),
            ((operational_status.OPERATIONAL, 0.1), (operational_status.NON_OPERATIONAL, 0.0)),
        ),
        (
            operational_domain,
            ((10.0, 20.0), (1.1, 2.2), (7.0, 8.0)),
            (operational_status.NON_OPERATIONAL, operational_status.OPERATIONAL),
        ),
    ],
    ids=["critical-temperature", "operational"],
)
def test_domain_mapping_protocol(
    domain_type: type[operational_domain | critical_temperature_domain],
    points: tuple[tuple[float, float], ...],
    values: tuple[Any, Any],
) -> None:
    """Both domain classes expose complete mapping views and missing-key errors."""
    domain = domain_type([sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF])
    key, new_key, missing_key = (parameter_point(list(point)) for point in points)
    value, new_value = values
    domain[key] = value
    assert domain[key] == value
    with pytest.raises(KeyError):
        _ = domain[missing_key]
    domain[new_key] = new_value
    assert domain[new_key] == new_value
    assert key in domain
    assert new_key in domain
    assert missing_key not in domain
    assert len(domain) == 2
    expected_keys = Counter([key, new_key])
    assert Counter(iter(domain)) == expected_keys
    assert Counter(domain.keys()) == expected_keys
    assert Counter(domain.values()) == Counter(values)
    assert Counter(domain.items()) == Counter([(key, value), (new_key, new_value)])


def test_operational_domain_two_bdl_pair_wire() -> None:
    bdl_wire = sidb_layout()

    bdl_wire.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.INPUT)
    bdl_wire.assign_sidb(lattice_site(2, 0, 0), sidb_dot_tag.INPUT)

    bdl_wire.assign_sidb(lattice_site(6, 0, 0), sidb_dot_tag.NORMAL)
    bdl_wire.assign_sidb(lattice_site(8, 0, 0), sidb_dot_tag.NORMAL)

    bdl_wire.assign_sidb(lattice_site(12, 0, 0), sidb_dot_tag.OUTPUT)
    bdl_wire.assign_sidb(lattice_site(14, 0, 0), sidb_dot_tag.OUTPUT)

    bdl_wire.assign_sidb(lattice_site(18, 0, 0), sidb_dot_tag.NORMAL)

    options: dict[str, Any] = {
        "sim_engine": sidb_simulation_engine.QUICKEXACT,
        "base": 2,
        "input_bdl_config": input_bdl_configuration.PERTURBER_DISTANCE_ENCODED,
    }

    options["sweep_dimensions"] = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 1.0, 10.0, 0.1),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 1.0, 10.0, 0.1),
    ]

    stats_grid = operational_domain_stats()
    op_domain = operational_domain_grid_search(bdl_wire, [create_id_tt()], stats=stats_grid, **options)

    assert len(op_domain) == 8281

    assert stats_grid.num_simulator_invocations == 10034
    assert stats_grid.num_evaluated_parameter_combinations == 8281
    assert stats_grid.num_operational_parameter_combinations == 0
    assert stats_grid.num_non_operational_parameter_combinations == 8281


@pytest.mark.parametrize("strategy", ["grid", "flood"])
def test_domain_reports_progress(resources_dir: Path, strategy: str) -> None:
    """The parameter points are reported as they are evaluated, ending at the grid size."""
    lyt = read_sqd_layout(str(resources_dir / "siqad_or_gate.sqd"))

    options: dict[str, Any] = {
        "sim_engine": sidb_simulation_engine.QUICKEXACT,
        "base": 2,
        "mu_minus": -0.28,
        "threshold_bdl_interdistance": 1.5,
        "op_condition": operational_condition.TOLERATE_KINKS,
        "sweep_dimensions": [
            operational_domain_value_range(sweep_parameter.EPSILON_R, 5.70, 5.80, 0.01),
            operational_domain_value_range(sweep_parameter.LAMBDA_TF, 3.00, 3.10, 0.01),
        ],
    }
    options["number_of_threads"] = 2

    reports = []
    options["on_progress"] = lambda task, done, total: reports.append((task, done, total))
    workers = []
    options["on_worker_progress"] = lambda *report: workers.append(report)

    stats = operational_domain_stats()
    if strategy == "grid":
        operational_domain_grid_search(lyt, [create_or_tt()], stats=stats, **options)
    else:
        operational_domain_flood_fill(lyt, [create_or_tt()], 1, stats=stats, **options)

    points = [(done, total) for task, done, total in reports if task == "parameter points"]
    assert points[0] == (0, 0)  # the total is unknown until the grid is set up
    assert points == sorted(points)
    assert points[-1][0] == stats.num_evaluated_parameter_combinations == 121
    assert points[-1][1] == (121 if strategy == "grid" else 0)

    finished = [(done, total) for worker, count, description, done, total, active in workers if not active]
    if strategy == "grid":
        assert len(finished) == options["number_of_threads"]
    assert sum(done for done, total in finished) == stats.num_evaluated_parameter_combinations
    assert all(total in (done, 0) for done, total in finished)
    if strategy == "flood":
        assert any("exploring" in description and total == 0 for _, _, description, _, total, _ in workers)


def test_single_point_keyword_domain(resources_dir: Path) -> None:
    """Nested physical and BDL options reach the one-point sweep and its statistics."""
    layout = read_sqd_layout(str(resources_dir / "siqad_or_gate.sqd"))
    stats = operational_domain_stats()
    domain = operational_domain_grid_search(
        layout,
        [create_or_tt()],
        base=2,
        mu_minus=-0.28,
        threshold_bdl_interdistance=1.5,
        sweep_dimensions=[
            operational_domain_value_range(sweep_parameter.EPSILON_R, 5.7, 5.7, 0.01),
            operational_domain_value_range(sweep_parameter.LAMBDA_TF, 3.0, 3.0, 0.01),
        ],
        number_of_threads=1,
        stats=stats,
    )
    assert domain[parameter_point([5.7, 3.0])] == operational_status.OPERATIONAL
    assert stats.num_evaluated_parameter_combinations == 1
    assert stats.num_operational_parameter_combinations == 1


@pytest.mark.parametrize(
    "ranges",
    [
        [(5.6, 5.6, 0.0)],
        [(5.6, 5.6, -0.0)],
        [(float("nan"), 5.6, 0.1)],
        [(5.6, float("inf"), 0.1)],
        [(5.6, 5.6, float("nan"))],
        [(1.0, 2.0, 2.2250738585072014e-308)],
        [(5.6, 5.6000002, 1e-7)],
        [(5.6000004, 5.6000022, 6e-7)],
        [(5.6, 5.7, 1e-18)],
    ],
)
def test_invalid_sweep_ranges(ranges: list[tuple[float, float, float]]) -> None:
    """Reject non-finite, overflowing, and identity-colliding grids before allocation or simulation."""
    dimensions = [sweep_parameter.EPSILON_R, sweep_parameter.LAMBDA_TF]
    with pytest.raises(ValueError):  # ruff: ignore[pytest-raises-too-broad] -- error type is the public contract
        operational_domain_grid_search(
            sidb_layout(),
            [create_id_tt()],
            sweep_dimensions=[
                operational_domain_value_range(dimension, *bounds)
                for dimension, bounds in zip(dimensions, ranges, strict=False)
            ],
        )


@pytest.mark.parametrize(
    "search",
    [
        operational_domain_random_sampling,
        operational_domain_flood_fill,
        operational_domain_contour_tracing,
        critical_temperature_domain_random_sampling,
        critical_temperature_domain_flood_fill,
        critical_temperature_domain_contour_tracing,
    ],
)
def test_colliding_sweeps_rejected_before_zero_samples(
    search: Callable[..., operational_domain | critical_temperature_domain],
) -> None:
    """Zero sampling still validates the grid for every sampling strategy."""
    with pytest.raises(ValueError):  # ruff: ignore[pytest-raises-too-broad] -- error type is the public contract
        search(
            sidb_layout(),
            [create_id_tt()],
            0,
            sweep_dimensions=[
                operational_domain_value_range(sweep_parameter.EPSILON_R, 5.6, 5.6000002, 1e-7),
                operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.0, 5.0, 0.1),
            ],
        )


@pytest.mark.parametrize(("maximum", "step", "count"), [(5.6, 1e-7, 1), (5.64, 0.07, 1), (5.600001, 6e-7, 2)])
def test_distinct_sweep_identities(resources_dir: Path, maximum: float, step: float, count: int) -> None:
    """Singleton and fine sweeps retain distinct points after trimming their last step."""
    layout = read_sqd_layout(str(resources_dir / "siqad_or_gate.sqd"))
    minimum = 5.6000004 if count == 2 else 5.6
    stats = operational_domain_stats()
    domain = operational_domain_grid_search(
        layout,
        [create_or_tt()],
        number_of_threads=1,
        stats=stats,
        mu_minus=-0.28,
        threshold_bdl_interdistance=1.5,
        sweep_dimensions=[
            operational_domain_value_range(sweep_parameter.EPSILON_R, minimum, maximum, step),
            operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.0, 5.0, 0.1),
        ],
    )
    assert len(domain) == count
    assert stats.num_evaluated_parameter_combinations == count


def test_flood_fill_rejects_one_dimension() -> None:
    """Flood fill requires at least two sweep dimensions, even without samples."""
    with pytest.raises(ValueError):  # ruff: ignore[pytest-raises-too-broad] -- error type is the public contract
        operational_domain_flood_fill(
            sidb_layout(),
            [create_id_tt()],
            0,
            sweep_dimensions=[operational_domain_value_range(sweep_parameter.EPSILON_R, 5.0, 5.1, 0.1)],
        )


def test_grid_product_overflow_preserves_statistics() -> None:
    """A product of small dimensions fails before the grid is allocated or statistics are published."""
    stats = operational_domain_stats()
    with pytest.raises(ValueError):  # ruff: ignore[pytest-raises-too-broad] -- error type is the public contract
        operational_domain_random_sampling(
            sidb_layout(),
            [create_id_tt()],
            0,
            stats=stats,
            sweep_dimensions=[
                operational_domain_value_range(sweep_parameter.EPSILON_R, 5.6, 5.7, 0.1) for _ in range(64)
            ],
        )
    assert stats.num_total_parameter_points == 0


@pytest.mark.parametrize("search", [operational_domain_random_sampling, critical_temperature_domain_random_sampling])
def test_zero_samples_produce_empty_domain(
    search: Callable[..., operational_domain | critical_temperature_domain], resources_dir: Path
) -> None:
    """Zero samples produce no classifications or simulator invocations."""
    layout = read_sqd_layout(str(resources_dir / "siqad_or_gate.sqd"))
    stats = operational_domain_stats()
    domain = search(
        layout,
        [create_or_tt()],
        0,
        stats=stats,
        sweep_dimensions=[
            operational_domain_value_range(sweep_parameter.EPSILON_R, 5.6, 5.7, 0.1),
            operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.0, 5.1, 0.1),
        ],
    )
    assert len(domain) == 0
    assert stats.num_evaluated_parameter_combinations == 0
    assert stats.num_simulator_invocations == 0


def test_random_sampling_stops_when_grid_is_exhausted(resources_dir: Path) -> None:
    """Requesting more samples than grid points yields the complete classification once."""
    layout = read_sqd_layout(str(resources_dir / "siqad_or_gate.sqd"))
    dimensions = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 5.7, 5.8, 0.1),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 3.0, 3.1, 0.1),
    ]
    stats = operational_domain_stats()
    domain = operational_domain_random_sampling(
        layout,
        [create_or_tt()],
        100,
        stats=stats,
        mu_minus=-0.28,
        threshold_bdl_interdistance=1.5,
        sweep_dimensions=dimensions,
        number_of_threads=2,
    )
    expected = {
        parameter_point([epsilon, screening]): operational_status.OPERATIONAL
        for epsilon in (5.7, 5.8)
        for screening in (3.0, 3.1)
    }
    assert dict(domain.items()) == expected
    assert stats.num_evaluated_parameter_combinations == 4
