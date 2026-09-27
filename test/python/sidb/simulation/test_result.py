# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``sidb_simulation_result``."""

from __future__ import annotations

import pytest

from mnt.pyfiction.sidb import ChargeState, DotTag, Lattice, LatticeSite, SiDBLayout
from mnt.pyfiction.sidb.simulation import (
    PotentialLandscape,
    QuickExactParams,
    SimulationResult,
    check_simulation_results_for_equivalence,
    exhaustive_ground_state_simulation,
    is_ground_state,
    quickexact,
)


@pytest.mark.parametrize("lattice", [Lattice.si_100_2x1(), Lattice.si_111_1x1()])
def test_engine_results(lattice: Lattice) -> None:
    """Exact engines agree on evaluated states, ground states, and result metadata."""
    layout = SiDBLayout(lattice, "pair")
    layout.assign_sidb(LatticeSite(), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(1, 0), DotTag.NORMAL)
    result = quickexact(layout)
    exhaustive = exhaustive_ground_state_simulation(layout)
    assert isinstance(result, SimulationResult)
    assert result.algorithm_name
    assert result.simulation_runtime.total_seconds() >= 0
    assert result.layout == layout
    assert len(result) == len(result.charge_distributions) > 0
    assert list(result) == result.charge_distributions
    assert check_simulation_results_for_equivalence(result, exhaustive)
    assert is_ground_state(result, exhaustive)
    landscape = PotentialLandscape(layout, params=result.simulation_parameters)
    for i, charges in enumerate(result):
        assert charges.energy == pytest.approx(landscape.energy(charges))
        assert landscape.is_physically_valid(charges)
        assert result.charge_state(i, LatticeSite()) == charges[LatticeSite()]
    assert all(charges.energy == min(cd.energy for cd in result) for charges in result.ground_states())
    with pytest.raises(IndexError):
        result.charge_state(len(result), LatticeSite())
    assert result.charge_state(0, LatticeSite(99, 99)) == ChargeState.NONE


def test_result_snapshots() -> None:
    """Returned layouts, parameters, and collections cannot alter an engine result."""
    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(), DotTag.NORMAL)
    params = QuickExactParams()
    result = quickexact(layout, params=params)
    states = list(result)
    params.simulation_parameters.epsilon_r = 1.0
    result.simulation_parameters.epsilon_r = 2.0
    result.layout.assign_sidb(LatticeSite(), DotTag.EMPTY)
    layout.assign_sidb(LatticeSite(), DotTag.EMPTY)
    result.charge_distributions.clear()
    assert result.layout.num_dots() == 1
    assert result.simulation_parameters.epsilon_r == QuickExactParams().simulation_parameters.epsilon_r
    assert list(result) == states
    for name in ("layout", "simulation_parameters", "charge_distributions", "algorithm_name", "simulation_runtime"):
        with pytest.raises(AttributeError):
            setattr(result, name, None)


def test_empty_engine_result() -> None:
    """Empty layouts have no physically valid simulation states."""
    result = quickexact(SiDBLayout())
    assert len(result) == 0
    assert result.ground_states() == []
    assert not is_ground_state(result, result)
    with pytest.raises(IndexError):
        result.charge_state(0, LatticeSite())


def test_result_comparison_detects_different_states() -> None:
    """Changing external potential can change the exact state ensemble."""
    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(), DotTag.NORMAL)
    plain = quickexact(layout)
    params = QuickExactParams()
    params.global_potential = -0.5
    shifted = quickexact(layout, params=params)
    assert not check_simulation_results_for_equivalence(plain, shifted)
    assert not is_ground_state(plain, shifted)


def test_result_iterator_owns_distributions() -> None:
    """Iteration stays valid when the simulation result leaves scope."""
    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(), DotTag.NORMAL)
    charges = list(iter(quickexact(layout)))
    assert len(charges) == 1
    assert charges[0][LatticeSite()] == ChargeState.NEGATIVE
