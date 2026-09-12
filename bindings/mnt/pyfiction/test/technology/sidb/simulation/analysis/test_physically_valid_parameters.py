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

import pytest

from mnt.pyfiction import (
    charge_distribution,
    lattice,
    lattice_site,
    parameter_point,
    physically_valid_parameters,
    sidb_dot_tag,
    sidb_layout,
)


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


def test_progress_callback_completes() -> None:
    """Worker callbacks complete without blocking on the Python interpreter lock."""
    subprocess.run(  # ruff: ignore[subprocess-without-shell-equals-true] -- fixed interpreter and test input
        [
            sys.executable,
            "-c",
            textwrap.dedent("""
                import time
                from mnt import pyfiction as pf

                layout = pf.sidb_layout()
                layout.assign_sidb(pf.lattice_site(0, 0), pf.sidb_dot_tag.NORMAL)
                params = pf.operational_domain_params()
                params.number_of_threads = 2
                params.sweep_dimensions = [
                    pf.operational_domain_value_range(pf.sweep_parameter.EPSILON_R, 5, 6, 0.5)
                ]
                reports = []

                def report(task: str, done: int, total: int) -> None:
                    'Record progress and exercise a slow callback.'
                    reports.append((task, done, total))
                    if done == 0:
                        time.sleep(0.2)

                params.on_progress = report
                pf.physically_valid_parameters(layout, pf.charge_distribution(layout), params)
                assert reports[-1] == ("parameter points", 3, 3), reports
            """),
        ],
        check=True,
        capture_output=True,
        text=True,
        timeout=30,
    )
