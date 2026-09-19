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
from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction import (
    create_and_tt,
    operational_domain_params,
    operational_domain_ratio,
    operational_domain_ratio_params,
    operational_domain_value_range,
    parameter_point,
    read_sqd_layout,
    sidb_simulation_engine,
    sweep_parameter,
)

if TYPE_CHECKING:
    from pathlib import Path


def test_and_gate_100_lattice(resources_dir):
    lyt = read_sqd_layout(str(resources_dir / "21_hex_inputsdbp_and_v19.sqd"))

    params = operational_domain_params()
    params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
    params.operational_params.simulation_parameters.base = 2

    params.sweep_dimensions = [
        operational_domain_value_range(sweep_parameter.EPSILON_R, 5.00, 6.00, 0.1),
        operational_domain_value_range(sweep_parameter.LAMBDA_TF, 5.00, 6.00, 0.1),
    ]

    ratio_params = operational_domain_ratio_params()
    ratio_params.op_domain_params = params

    assert ratio_params.op_domain_params.operational_params.simulation_parameters.base == 2

    operational_domain_ratio_result = operational_domain_ratio(
        lyt,
        [create_and_tt()],
        parameter_point([5.6, 5.0]),
        ratio_params,
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
                import time
                from mnt import pyfiction as pf

                layout = pf.read_sqd_layout(sys.argv[1])
                params = pf.operational_domain_params()
                params.number_of_threads = 2
                params.operational_params.sim_engine = pf.sidb_simulation_engine.QUICKEXACT
                params.operational_params.simulation_parameters.base = 2
                params.sweep_dimensions = [
                    pf.operational_domain_value_range(pf.sweep_parameter.EPSILON_R, 5.5, 5.7, 0.01),
                    pf.operational_domain_value_range(pf.sweep_parameter.LAMBDA_TF, 5, 5.2, 0.01),
                ]
                reports = []

                def report(task: str, done: int, total: int) -> None:
                    'Record progress and exercise a slow callback.'
                    reports.append((task, done, total))
                    if done == 0:
                        time.sleep(0.2)

                params.on_progress = report
                ratio_params = pf.operational_domain_ratio_params()
                ratio_params.op_domain_params = params
                ratio = pf.operational_domain_ratio(
                    layout, [pf.create_and_tt()], pf.parameter_point([5.6, 5]), ratio_params
                )
                assert 0 <= ratio <= 1, ratio
                assert reports[-1][1] > 0, reports
            """),
            str(resources_dir / "21_hex_inputsdbp_and_v19.sqd"),
        ],
        check=True,
        capture_output=True,
        text=True,
        timeout=30,
    )
