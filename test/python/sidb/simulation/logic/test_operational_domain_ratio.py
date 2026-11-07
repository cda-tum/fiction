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

from mnt.pyfiction.sidb import sidb_layout
from mnt.pyfiction.sidb.io import read_sqd_layout
from mnt.pyfiction.sidb.simulation import sidb_simulation_engine
from mnt.pyfiction.sidb.simulation.logic import (
    operational_domain_ratio,
    operational_domain_value_range,
    parameter_point,
    sweep_parameter,
)
from mnt.pyfiction.synthesis import create_and_tt

if TYPE_CHECKING:
    from pathlib import Path


def test_and_gate_100_lattice(resources_dir):
    lyt = read_sqd_layout(str(resources_dir / "21_hex_inputsdbp_and_v19.sqd"))

    options: dict[str, Any] = {
        "sim_engine": sidb_simulation_engine.QUICKEXACT,
        "base": 2,
    }

    options["sweep_dimensions"] = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 5.00, 6.00, 0.1),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.00, 6.00, 0.1),
    ]

    ratio_options = options.copy()

    operational_domain_ratio_result = operational_domain_ratio(
        lyt, [create_and_tt()], parameter_point([5.6, 5.0]), **ratio_options
    )

    assert operational_domain_ratio_result == pytest.approx(23 / 121, abs=1e-5)


def test_progress_callback_completes(resources_dir: Path) -> None:
    """Nested operational-domain callbacks finish while flood-fill workers run."""
    subprocess.run(  # ruff: ignore[subprocess-without-shell-equals-true] -- fixed interpreter and test input
        [
            sys.executable,
            "-c",
            textwrap.dedent("""

                import sys
                import threading
                import time
                from mnt.pyfiction.sidb.io import read_sqd_layout
                from mnt.pyfiction.sidb.simulation import sidb_simulation_engine
                from mnt.pyfiction.sidb.simulation.logic import (
                    operational_domain_ratio,
                    operational_domain_value_range,
                    parameter_point,
                    sweep_parameter,
                )
                from mnt.pyfiction.synthesis import create_and_tt

                layout = read_sqd_layout(sys.argv[1])
                options = {}
                options['number_of_threads'] = 2
                options['sim_engine'] = sidb_simulation_engine.QUICKEXACT
                options['base'] = 2
                options['sweep_dimensions'] = [
                    operational_domain_value_range(sweep_parameter.EPSILON_R, 5.5, 5.7, 0.02),
                    operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5, 5.2, 0.02),
                ]
                reports = []
                threads = set()

                def report(task: str, done: int, total: int) -> None:
                    'Record progress and exercise a slow callback.'
                    reports.append((task, done, total))
                    threads.add(threading.get_ident())
                    if done == 0:
                        time.sleep(0.2)

                options['on_progress'] = report
                ratio_options = options.copy()
                ratio = operational_domain_ratio(layout, [create_and_tt()], parameter_point([5.6, 5]), **ratio_options)
                assert 0 <= ratio <= 1, ratio
                assert reports[-1][1] > 0, reports
                assert threads - {threading.get_ident()}, threads
            """),
            str(resources_dir / "21_hex_inputsdbp_and_v19.sqd"),
        ],
        check=True,
        capture_output=True,
        text=True,
        timeout=30,
    )


@pytest.mark.parametrize(
    "seed",
    [
        [],
        [5.0],
        [5.0, 5.0, 5.0],
        [float("nan"), 5.0],
        [5.0, float("inf")],
        [-float("inf"), 5.0],
        [4.9, 5.0],
        [5.0, 6.1],
    ],
)
def test_ratio_rejects_malformed_seed(seed: list[float]) -> None:
    """Seeds must match the dimensions and lie inside finite sweep bounds."""
    with pytest.raises(ValueError):  # ruff: ignore[pytest-raises-too-broad] -- error type is the public contract
        operational_domain_ratio(
            sidb_layout(),
            [create_and_tt()],
            parameter_point(seed),
            sweep_dimensions=[
                operational_domain_value_range(sweep_parameter.EPSILON_R, 5.0, 6.0, 1.0),
                operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.0, 6.0, 1.0),
            ],
        )


@pytest.mark.parametrize(("maximum", "step"), [(5.1, 0.0), (5.0000002, 1e-7)])
def test_ratio_rejects_invalid_sweep(maximum: float, step: float) -> None:
    """Ratios reject invalid steps and coordinate identities before sampling."""
    with pytest.raises(ValueError):  # ruff: ignore[pytest-raises-too-broad] -- error type is the public contract
        operational_domain_ratio(
            sidb_layout(),
            [create_and_tt()],
            parameter_point([5.0, 5.0]),
            sweep_dimensions=[
                operational_domain_value_range(sweep_parameter.EPSILON_R, 5.0, maximum, step),
                operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.0, 5.0, 0.1),
            ],
        )
