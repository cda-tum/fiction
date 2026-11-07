# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from collections import Counter
from itertools import starmap

import pytest

from mnt.pyfiction.sidb import lattice_site, sidb_layout
from mnt.pyfiction.sidb.model import charge_state_to_sign
from mnt.pyfiction.sidb.simulation import engines


@pytest.fixture(params=["quickexact", "exhaustive_ground_state_simulation", "clustercomplete"])
def exact_engine(request: pytest.FixtureRequest):
    """Return each compiled exact engine without expanding unrelated physical inputs."""
    if not hasattr(engines, request.param):
        pytest.skip("pyfiction was built without ALGLIB")
    return getattr(engines, request.param)


def test_exact_engine_empty_and_single_sidb(exact_engine) -> None:
    """Exact engines distinguish an empty layout from one isolated negative SiDB."""
    layout = sidb_layout()
    assert exact_engine(layout, base=2).charge_distributions == []
    layout.assign_sidb(lattice_site(1, 3))
    result = exact_engine(layout, base=2)
    assert result.layout == layout
    assert len(result.charge_distributions) == 1
    assert [charge_state_to_sign(state) for state in result.charge_distributions[0].charge_states()] == [-1]
    assert result.charge_distributions[0].energy() == 0.0


@pytest.mark.parametrize(
    ("mu_minus", "epsilon_r", "lambda_tf", "signs", "energy"),
    [
        (-0.28, 5.6, 5.0, (-1, -1, -1, -1, -1, 0, 0, 0), 0.4662582096),
        (-0.1, 5.6, 5.0, (-1, 0, -1, 0, -1, 0, 0, 0), 0.061037632),
        (-0.7, 5.6, 5.0, (-1, -1, -1, -1, -1, -1, -1, -1), 2.069954113),
        (-0.28, 5.6, 1.0, (-1, -1, -1, -1, -1, -1, -1, -1), 0.5432404075),
        (-0.28, 5.6, 10.0, (-1, 0, -1, 0, -1, -1, 0, 0), 0.2930574885),
        (-0.28, 10.0, 5.0, (-1, -1, -1, -1, -1, 0, 0, -1), 0.505173434),
    ],
)
def test_exact_engine_or_gate_physical_parameters(
    exact_engine, mu_minus: float, epsilon_r: float, lambda_tf: float, signs: tuple[int, ...], energy: float
) -> None:
    """Exact engines return the complete OR-gate configuration and Catch2 energy under each physical change."""
    coordinates = [(6, 2, 0), (12, 3, 0), (10, 8, 1), (10, 6, 1), (16, 1, 0), (10, 5, 0), (14, 2, 0), (8, 3, 0)]
    sites = list(starmap(lattice_site, coordinates))
    layout = sidb_layout()
    for site in sites:
        layout.assign_sidb(site)
    result = exact_engine(layout, base=2, mu_minus=mu_minus, epsilon_r=epsilon_r, lambda_tf=lambda_tf)
    assert Counter(
        tuple(charge_state_to_sign(cd.get_charge_state(site)) for site in sites) for cd in result.charge_distributions
    ) == Counter([signs])
    assert result.charge_distributions[0].energy() == pytest.approx(energy, abs=1e-6)


def test_exact_engine_degenerate_bdl_pair(exact_engine) -> None:
    """A symmetric BDL pair retains both degenerate charge configurations."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site(1, 3))
    layout.assign_sidb(lattice_site(3, 3))
    result = exact_engine(layout, base=3, mu_minus=-0.25)
    assert Counter(
        tuple(charge_state_to_sign(state) for state in cd.charge_states()) for cd in result.charge_distributions
    ) == Counter([(-1, 0), (0, -1)])
    assert [cd.energy() for cd in result.charge_distributions] == [0.0, 0.0]
