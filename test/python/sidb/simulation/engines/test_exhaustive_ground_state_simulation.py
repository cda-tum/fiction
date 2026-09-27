# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``exhaustive_ground_state_simulation``."""

from __future__ import annotations

from mnt.pyfiction.sidb import ChargeState, DotTag, Lattice, LatticeSite, SiDBLayout, SimulationParams
from mnt.pyfiction.sidb.simulation import exhaustive_ground_state_simulation


def test_perturber_and_sidb_pair() -> None:
    """ExGS finds the H-Si(100)-2x1 ground state."""

    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(0, 0, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(4, 0, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(6, 0, 1), DotTag.NORMAL)

    result = exhaustive_ground_state_simulation(layout, params=SimulationParams())

    assert result.algorithm_name == "ExGS"
    assert result.layout == layout
    assert len(result.charge_distributions) == 1

    groundstate = result.charge_distributions[0]

    assert groundstate.get_charge_state(LatticeSite(0, 0, 1)) == ChargeState.NEGATIVE
    assert groundstate.get_charge_state(LatticeSite(4, 0, 1)) == ChargeState.NEUTRAL
    assert groundstate.get_charge_state(LatticeSite(6, 0, 1)) == ChargeState.NEGATIVE


def test_perturber_and_sidb_pair_111() -> None:
    """ExGS finds the H-Si(111)-1x1 ground state."""

    layout = SiDBLayout(Lattice.si_111_1x1())
    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(1, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(2, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(3, 0, 0), DotTag.NORMAL)

    params = SimulationParams()
    params.mu_minus = -0.32
    params.base = 2

    result = exhaustive_ground_state_simulation(layout, params=params)

    assert result.algorithm_name == "ExGS"

    groundstate = result.ground_states()

    assert len(groundstate) == 1

    assert groundstate[0].get_charge_state(LatticeSite(0, 0, 0)) == ChargeState.NEGATIVE
    assert groundstate[0].get_charge_state(LatticeSite(1, 0, 0)) == ChargeState.NEUTRAL
    assert groundstate[0].get_charge_state(LatticeSite(2, 0, 0)) == ChargeState.NEUTRAL
    assert groundstate[0].get_charge_state(LatticeSite(3, 0, 0)) == ChargeState.NEGATIVE


def test_exgs_reports_progress() -> None:
    """Every charge configuration is counted, and the callback may be omitted or ``None``."""

    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(0, 0, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(4, 0, 1), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(6, 0, 1), DotTag.NORMAL)

    params = SimulationParams()
    params.base = 2

    reports: list[tuple[str, int, int]] = []

    result = exhaustive_ground_state_simulation(
        layout, params=params, on_progress=lambda task, done, total: reports.append((task, done, total))
    )

    assert len(result.charge_distributions) == 1
    # three SiDBs in base 2 have eight charge configurations
    assert reports[0] == ("charge configurations", 0, 8)
    assert reports[-1] == ("charge configurations", 8, 8)

    silent = exhaustive_ground_state_simulation(layout, params=params, on_progress=None)
    assert len(silent.charge_distributions) == 1
