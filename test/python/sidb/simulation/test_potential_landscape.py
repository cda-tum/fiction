# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``potential_landscape``."""

from __future__ import annotations

import pytest

from mnt.pyfiction.sidb import (
    ChargeState,
    Defect,
    DefectType,
    DotTag,
    LatticeSite,
    SiDBLayout,
    SimulationParams,
)
from mnt.pyfiction.sidb.simulation import ChargeTransitionThresholdBounds, PotentialLandscape


def three_sidbs() -> SiDBLayout:
    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(5, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(7, 0, 0), DotTag.NORMAL)
    return layout


def test_geometry_and_potentials():
    layout = three_sidbs()
    landscape = PotentialLandscape(layout, params=SimulationParams(2, -0.32))

    assert landscape.num_sidbs() == 3
    assert landscape.layout == layout
    assert landscape.params.base == 2
    assert landscape.defects() == []

    assert landscape.nm_distance(0, 1) == pytest.approx(1.92, abs=1e-5)
    assert landscape.nm_distance(1, 2) == pytest.approx(0.768, abs=1e-5)
    assert landscape.nm_distance(1, 1) == 0.0
    assert landscape.chargeless_potential(1, 1) == 0.0
    assert landscape.chargeless_potential(0, 1) == landscape.chargeless_potential(1, 0)
    assert landscape.chargeless_potential(1, 2) > landscape.chargeless_potential(0, 1)
    assert landscape.chargeless_potential(1, 2) == pytest.approx(landscape.chargeless_potential_at_distance(0.768))
    assert landscape.local_external_potential(0) == 0.0
    assert landscape.local_potential_caused_by_defects(0) == 0.0

    thresholds = landscape.effective_charge_transition_thresholds(0)
    assert thresholds[ChargeTransitionThresholdBounds.NEGATIVE_UPPER_BOUND.value] == pytest.approx(0.32, abs=1e-5)
    assert thresholds[ChargeTransitionThresholdBounds.POSITIVE_LOWER_BOUND.value] == pytest.approx(0.91, abs=1e-5)


def test_energies_and_validity():
    layout = three_sidbs()
    landscape = PotentialLandscape(layout, params=SimulationParams(2, -0.32))

    all_negative = landscape.evaluate([ChargeState.NEGATIVE] * layout.num_dots())
    all_neutral = landscape.evaluate([ChargeState.NEUTRAL] * layout.num_dots())

    assert all_neutral.energy == 0.0
    assert all_negative.energy > 0.0
    assert landscape.energy(all_negative) == pytest.approx(all_negative.energy)
    assert not landscape.is_physically_valid(all_negative)  # the two close SiDBs cannot both be negative
    assert not landscape.is_physically_valid(all_neutral)  # the isolated SiDB has to be negative

    ground = landscape.evaluate([ChargeState.NEGATIVE, ChargeState.NEUTRAL, ChargeState.NEGATIVE])

    assert landscape.is_physically_valid(ground)
    assert 0.0 < ground.energy < all_negative.energy

    local_potentials = landscape.local_potentials(ground)
    assert len(local_potentials) == 3
    assert local_potentials[1] < 0.0  # the neutral SiDB sits between two negative ones

    internal = landscape.local_internal_potentials(ground)
    assert len(internal) == 3
    assert internal == pytest.approx(local_potentials)  # no external potential applied


def test_external_potentials():
    layout = three_sidbs()
    params = SimulationParams(2, -0.32)
    plain = PotentialLandscape(layout, params=params)
    shifted = PotentialLandscape(
        layout, params=params, local_external_potential={LatticeSite(0, 0, 0): 0.1}, global_external_potential=0.2
    )

    assert shifted.local_external_potential(0) == pytest.approx(0.3)
    assert shifted.local_external_potential(1) == pytest.approx(0.2)

    all_negative = plain.evaluate([ChargeState.NEGATIVE] * layout.num_dots())
    assert shifted.energy(all_negative) < plain.energy(all_negative)


def test_charged_defect():
    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(0, 0, 0), DotTag.NORMAL)
    layout.assign_sidb(LatticeSite(3, 0, 0), DotTag.NORMAL)
    layout.assign_defect(LatticeSite(1, 1, 0), Defect(DefectType.SI_VACANCY, -1, 5.6, 5.0))
    layout.assign_defect(LatticeSite(6, 1, 0), Defect(DefectType.SILOXANE, 0))  # neutral, does not enter

    landscape = PotentialLandscape(layout, params=SimulationParams(2, -0.32))

    assert len(landscape.defects()) == 1
    assert landscape.local_potential_caused_by_defects(0) < 0.0
    assert landscape.local_potential_caused_by_defects(0) < landscape.local_potential_caused_by_defects(1)

    neutral = landscape.evaluate([ChargeState.NEUTRAL] * layout.num_dots())
    assert landscape.local_internal_potentials(neutral)[0] == pytest.approx(
        landscape.local_potential_caused_by_defects(0)
    )

    # a negative defect makes the neutral distribution more attractive than the negative one
    assert landscape.energy(landscape.evaluate([ChargeState.NEGATIVE] * layout.num_dots())) > landscape.energy(neutral)


def test_input_boundaries() -> None:
    """Landscape queries reject missing indices and distributions over different sites."""
    for landscape in (PotentialLandscape(SiDBLayout()), PotentialLandscape(three_sidbs())):
        index = landscape.num_sidbs()
        with pytest.raises(IndexError):
            landscape.nm_distance(0, index)
        with pytest.raises(IndexError):
            landscape.nm_distance(index, 0)
        with pytest.raises(IndexError):
            landscape.chargeless_potential(0, index)
        with pytest.raises(IndexError):
            landscape.local_external_potential(index)
        with pytest.raises(IndexError):
            landscape.local_potential_caused_by_defects(index)
        with pytest.raises(IndexError):
            landscape.effective_charge_transition_thresholds(index)
    landscape = PotentialLandscape(three_sidbs())
    other = three_sidbs()
    other.assign_sidb(LatticeSite(7, 0, 0), DotTag.EMPTY)
    other.assign_sidb(LatticeSite(8, 0, 0), DotTag.NORMAL)
    for cd in (
        PotentialLandscape(SiDBLayout()).evaluate([]),
        PotentialLandscape(other).evaluate([ChargeState.NEGATIVE] * other.num_dots()),
    ):
        with pytest.raises(ValueError, match="SiDB sites"):
            landscape.local_internal_potentials(cd)
        with pytest.raises(ValueError, match="SiDB sites"):
            landscape.local_potentials(cd)
        with pytest.raises(ValueError, match="SiDB sites"):
            landscape.energy(cd)
        with pytest.raises(ValueError, match="SiDB sites"):
            landscape.is_physically_valid(cd)


def test_evaluation_boundaries() -> None:
    """Each site needs a physical charge state; validity remains an explicit query."""
    landscape = PotentialLandscape(three_sidbs())
    for states in ([], [ChargeState.NEGATIVE] * 4, [ChargeState.NEGATIVE, ChargeState.NONE, ChargeState.NEUTRAL]):
        with pytest.raises(ValueError, match="charge state"):
            landscape.evaluate(states)
    assert len(PotentialLandscape(SiDBLayout()).evaluate([])) == 0


def test_landscape_snapshots() -> None:
    """Editing input and returned model data cannot invalidate cached potentials."""
    layout = three_sidbs()
    params = SimulationParams()
    landscape = PotentialLandscape(layout, params=params)
    distance = landscape.nm_distance(0, 1)
    layout.assign_sidb(LatticeSite(), DotTag.EMPTY)
    copied = landscape.layout
    copied.assign_sidb(LatticeSite(), DotTag.EMPTY)
    params.epsilon_r = 1.0
    landscape.params.epsilon_r = 2.0
    assert landscape.num_sidbs() == 3
    assert landscape.layout.num_dots() == 3
    assert landscape.params.epsilon_r == SimulationParams().epsilon_r
    assert landscape.nm_distance(0, 1) == distance
