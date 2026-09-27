# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``charge_distribution``."""

from __future__ import annotations

import pytest

from mnt.pyfiction.sidb import ChargeDistribution, ChargeState, DotTag, LatticeSite, SiDBLayout
from mnt.pyfiction.sidb.simulation import PotentialLandscape


def test_evaluated_distribution() -> None:
    """Evaluated distributions expose raster-ordered states and consistent energy."""
    layout = SiDBLayout()
    sites = [LatticeSite(0, 0), LatticeSite(3, 0), LatticeSite(5, 0)]
    for site in reversed(sites):
        layout.assign_sidb(site, DotTag.NORMAL)
    landscape = PotentialLandscape(layout)
    states = [ChargeState.NEGATIVE, ChargeState.NEUTRAL, ChargeState.POSITIVE]
    charges = landscape.evaluate(states)
    assert isinstance(charges, ChargeDistribution)
    assert len(charges) == 3
    assert list(charges) == states
    assert charges.sites == sites
    assert charges.charge_states == states
    assert charges.energy == landscape.energy(charges)
    assert charges[sites[1]] == ChargeState.NEUTRAL
    assert charges.num_negative_sidbs() == charges.num_neutral_sidbs() == charges.num_positive_sidbs() == 1
    assert charges.index_of(sites[2]) == 2
    assert charges.index_of(LatticeSite(99, 99)) is None
    with pytest.raises(KeyError):
        _ = charges[LatticeSite(99, 99)]


def test_distribution_is_read_only() -> None:
    """State and site snapshots cannot change a distribution's cached energy."""
    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(), DotTag.NORMAL)
    charges = PotentialLandscape(layout).evaluate([ChargeState.NEGATIVE])
    states = charges.charge_states
    states[0] = ChargeState.POSITIVE
    charges.sites.clear()
    assert list(charges) == [ChargeState.NEGATIVE]
    assert len(charges.sites) == 1
    for name in ("energy", "charge_states", "sites"):
        with pytest.raises(AttributeError):
            setattr(charges, name, None)
    for name in (
        "assign_charge_state",
        "assign_charge_state_by_index",
        "assign_all_charge_states",
        "assign_energy",
        "charge_index",
    ):
        assert not hasattr(charges, name)
    with pytest.raises(TypeError):
        hash(charges)


def test_distribution_iterator_owns_states() -> None:
    """Iterators remain valid after their distribution leaves scope."""
    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(), DotTag.NORMAL)
    states = iter(PotentialLandscape(layout).evaluate([ChargeState.NEUTRAL]))
    assert list(states) == [ChargeState.NEUTRAL]
