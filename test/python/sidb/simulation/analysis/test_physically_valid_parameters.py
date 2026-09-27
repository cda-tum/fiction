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

from mnt.pyfiction.sidb import ChargeState, DotTag, Lattice, LatticeSite, SiDBLayout
from mnt.pyfiction.sidb.analysis import ParameterPoint, physically_valid_parameters
from mnt.pyfiction.sidb.simulation import PotentialLandscape


def test_one_sidb_100_lattice() -> None:
    """Check physical parameter validity on the Si(100) lattice."""
    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)

    valid_parameters = physically_valid_parameters(
        layout, PotentialLandscape(layout).evaluate([ChargeState.NEGATIVE] * layout.num_dots())
    )

    assert valid_parameters.get_excited_state_number_for_parameter(ParameterPoint([5, 5])) == 0

    assert valid_parameters.get_excited_state_number_for_parameter(ParameterPoint([5.1, 5.1])) == 0

    # Testing for an invalid parameter point that raises an exception
    with pytest.raises(ValueError, match="no excited state number available"):
        valid_parameters.get_excited_state_number_for_parameter(ParameterPoint([15, 15]))


def test_one_sidb_111_lattice() -> None:
    """Check physical parameter validity on the Si(111) lattice."""
    layout = SiDBLayout(Lattice.si_111_1x1())
    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)

    valid_parameters = physically_valid_parameters(
        layout, PotentialLandscape(layout).evaluate([ChargeState.NEGATIVE] * layout.num_dots())
    )

    assert valid_parameters.get_excited_state_number_for_parameter(ParameterPoint([5, 5])) == 0

    assert valid_parameters.get_excited_state_number_for_parameter(ParameterPoint([5.1, 5.1])) == 0

    # Testing for an invalid parameter point that raises an exception
    with pytest.raises(ValueError, match="no excited state number available"):
        valid_parameters.get_excited_state_number_for_parameter(ParameterPoint([15, 15]))


def test_progress_callback_completes() -> None:
    """Worker callbacks complete without blocking on the Python interpreter lock."""
    subprocess.run(  # ruff: ignore[subprocess-without-shell-equals-true] -- fixed interpreter and test input
        [
            sys.executable,
            "-c",
            textwrap.dedent("""
                import time
                from mnt.pyfiction.sidb import ChargeState, LatticeSite, DotTag, SiDBLayout
                from mnt.pyfiction.sidb.simulation import PotentialLandscape
                from mnt.pyfiction.sidb.analysis import physically_valid_parameters
                from mnt.pyfiction.sidb.analysis import (
                    OperationalDomainParams,
                    SweepRange,
                    SweepParameter,
                )

                layout = SiDBLayout()
                layout.assign_sidb(LatticeSite(0, 0), DotTag.NORMAL)
                params = OperationalDomainParams()
                params.number_of_threads = 2
                params.sweep_dimensions = [
                    SweepRange(SweepParameter.EPSILON_R, 5, 6, 0.5)
                ]
                reports = []

                def report(task: str, done: int, total: int) -> None:
                    'Record progress and exercise a slow callback.'
                    reports.append((task, done, total))
                    if done == 0:
                        time.sleep(0.2)

                params.on_progress = report
                charges = PotentialLandscape(layout).evaluate([ChargeState.NEGATIVE])
                physically_valid_parameters(layout, charges, params)
                assert reports[-1] == ("parameter points", 3, 3), reports
            """),
        ],
        check=True,
        capture_output=True,
        text=True,
        timeout=30,
    )
