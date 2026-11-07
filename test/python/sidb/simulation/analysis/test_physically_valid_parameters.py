# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import subprocess  # ruff: ignore[suspicious-subprocess-import] -- bounded native deadlock regression
import sys
import textwrap
from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction.sidb import charge_distribution, lattice, lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.io import read_sqd_layout
from mnt.pyfiction.sidb.model import sidb_charge_state
from mnt.pyfiction.sidb.simulation import sidb_simulation_engine
from mnt.pyfiction.sidb.simulation.analysis import physically_valid_parameters
from mnt.pyfiction.sidb.simulation.engines import quickexact
from mnt.pyfiction.sidb.simulation.logic import (
    input_bdl_configuration,
    operational_analysis_strategy,
    operational_condition,
    operational_domain_value_range,
    parameter_point,
    sweep_parameter,
)

if TYPE_CHECKING:
    from pathlib import Path


def test_excited_state_number_counts_lower_energy_levels(resources_dir: Path) -> None:
    """Physical-parameter analysis reports energy-level rank rather than state degeneracy.

    Args:
        resources_dir: Directory containing the reference SiDB layouts.
    """
    layout = read_sqd_layout(str(resources_dir / "Bestagon_AND_mu_025_v0.sqd"))
    result = quickexact(layout, base=2)
    highest = max(result.charge_distributions, key=lambda cd: cd.energy())
    parameters = physically_valid_parameters(
        layout,
        highest,
        base=2,
        number_of_threads=1,
        sweep_dimensions=[operational_domain_value_range(sweep_parameter.EPSILON_R, 5.6, 5.6, 0.1)],
    )
    assert parameters.get_excited_state_number_for_parameter(parameter_point([5.6])) == 2


def test_three_dimensional_physical_validity_ranks(resources_dir: Path) -> None:
    """A third-axis transition-energy sweep distinguishes ground-state and excited-state ranks."""
    layout = read_sqd_layout(str(resources_dir / "21_hex_inputsdbp_and_v19.sqd"))
    layout.assign_sidb(lattice_site(36, 1), sidb_dot_tag.EMPTY)
    layout.assign_sidb(lattice_site(), sidb_dot_tag.EMPTY)
    distribution = charge_distribution(layout)
    for coordinates in (
        (6, 2, 0),
        (30, 3, 0),
        (12, 4, 0),
        (24, 5, 0),
        (19, 8, 0),
        (18, 11, 1),
        (20, 14, 0),
        (26, 16, 0),
        (32, 18, 0),
    ):
        distribution.assign_charge_state(lattice_site(*coordinates), sidb_charge_state.NEUTRAL)
    parameters = physically_valid_parameters(
        layout,
        distribution,
        base=2,
        number_of_threads=1,
        sweep_dimensions=[
            operational_domain_value_range(sweep_parameter.EPSILON_R, 5.6, 5.6, 0.1),
            operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.0, 5.0, 0.1),
            operational_domain_value_range(sweep_parameter.MU_MINUS, -0.33, -0.31, 0.01),
        ],
    )
    assert [
        parameters.get_excited_state_number_for_parameter(parameter_point([5.6, 5.0, mu_minus]))
        for mu_minus in (-0.33, -0.32, -0.31)
    ] == [0, 0, 1]


def test_one_sidb_100_lattice() -> None:
    """Check physical parameter validity on the Si(100) lattice."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)

    valid_parameters = physically_valid_parameters(layout, charge_distribution(layout))

    assert valid_parameters.get_excited_state_number_for_parameter(parameter_point([5, 5])) == 0

    assert valid_parameters.get_excited_state_number_for_parameter(parameter_point([5.1, 5.1])) == 0

    # Testing for an invalid parameter point that raises an exception
    with pytest.raises(ValueError, match="no excited state number available"):
        valid_parameters.get_excited_state_number_for_parameter(parameter_point([15, 15]))


def test_one_sidb_111_lattice() -> None:
    """Check physical parameter validity on the Si(111) lattice."""
    layout = sidb_layout(lattice.si_111_1x1())
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)

    valid_parameters = physically_valid_parameters(layout, charge_distribution(layout))

    assert valid_parameters.get_excited_state_number_for_parameter(parameter_point([5, 5])) == 0

    assert valid_parameters.get_excited_state_number_for_parameter(parameter_point([5.1, 5.1])) == 0

    # Testing for an invalid parameter point that raises an exception
    with pytest.raises(ValueError, match="no excited state number available"):
        valid_parameters.get_excited_state_number_for_parameter(parameter_point([15, 15]))


@pytest.mark.parametrize(
    ("name", "value"),
    [
        ("threshold_bdl_interdistance", 1.0),
        ("minimum_distance", 1.0),
        ("maximum_distance", 1.0),
        ("input_bdl_config", input_bdl_configuration.PERTURBER_ABSENCE_ENCODED),
        ("op_condition", operational_condition.REJECT_KINKS),
        ("strategy_to_analyze_operational_status", operational_analysis_strategy.SIMULATION_ONLY),
    ],
)
def test_physical_validity_rejects_gate_options(name: str, value: object) -> None:
    """Physical-validity sweeps expose physical and sweep options without gate detection."""
    layout = sidb_layout()
    options: dict[str, Any] = {name: value}
    with pytest.raises(TypeError):
        physically_valid_parameters(layout, charge_distribution(layout), **options)


@pytest.mark.skipif(not hasattr(sidb_simulation_engine, "CLUSTERCOMPLETE"), reason="ALGLIB is not available")
def test_physical_validity_rejects_unsupported_engine() -> None:
    """Physical-validity sweeps reject engines that cannot count excited states."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0), sidb_dot_tag.NORMAL)
    with pytest.raises(ValueError, match="unsupported simulation engine"):
        physically_valid_parameters(
            layout, charge_distribution(layout), sim_engine=sidb_simulation_engine.CLUSTERCOMPLETE
        )


def test_progress_callback_completes() -> None:
    """Worker callbacks complete without blocking on the Python interpreter lock."""
    subprocess.run(  # ruff: ignore[subprocess-without-shell-equals-true] -- fixed interpreter and test input
        [
            sys.executable,
            "-c",
            textwrap.dedent("""

                import time
                from mnt.pyfiction.sidb import charge_distribution, lattice_site, sidb_dot_tag, sidb_layout
                from mnt.pyfiction.sidb.simulation.analysis import physically_valid_parameters
                from mnt.pyfiction.sidb.simulation.logic import operational_domain_value_range, sweep_parameter

                layout = sidb_layout()
                layout.assign_sidb(lattice_site(0, 0), sidb_dot_tag.NORMAL)
                options = {}
                options['number_of_threads'] = 2
                options['sweep_dimensions'] = [
                    operational_domain_value_range(sweep_parameter.EPSILON_R, 5, 6, 0.5)
                ]
                reports = []

                def report(task: str, done: int, total: int) -> None:
                    'Record progress and exercise a slow callback.'
                    reports.append((task, done, total))
                    if done == 0:
                        time.sleep(0.2)

                options['on_progress'] = report
                physically_valid_parameters(layout, charge_distribution(layout), **options)
                assert reports[-1] == ("parameter points", 3, 3), reports
            """),
        ],
        check=True,
        capture_output=True,
        text=True,
        timeout=30,
    )


@pytest.mark.parametrize(
    "bounds",
    [
        (5.0, 6.0, 0.0),
        (5.0, 6.0, -1.0),
        (6.0, 5.0, 1.0),
        (float("nan"), 6.0, 0.1),
        (5.0, float("inf"), 0.1),
        (5.0, 6.0, float("nan")),
        (5.0, 6.0, float("inf")),
        (5.0, 6.0, 2.2250738585072014e-308),
        (5.6, 5.6000002, 1e-7),
    ],
)
def test_physical_validity_sweep_boundaries(bounds: tuple[float, float, float]) -> None:
    """Matching configurations validate grid bounds; mismatched configurations yield no domain."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site())
    dimensions = [operational_domain_value_range(sweep_parameter.EPSILON_R, *bounds)]
    with pytest.raises(ValueError):  # ruff: ignore[pytest-raises-too-broad] -- error type is the public contract
        physically_valid_parameters(layout, charge_distribution(layout), sweep_dimensions=dimensions)
    empty_domain = physically_valid_parameters(layout, charge_distribution(), sweep_dimensions=dimensions)
    with pytest.raises(ValueError, match="no excited state number available"):
        empty_domain.get_excited_state_number_for_parameter(parameter_point([5.6]))
