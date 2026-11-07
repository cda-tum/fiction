# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import Any

import pytest

from mnt.pyfiction.sidb import lattice, lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.simulation.analysis import physical_population_stability, transition_type


def test_population_stability_documents_python_exception() -> None:
    """Document the exception raised for invalid analysis parameters."""
    documentation = physical_population_stability.__doc__
    assert documentation is not None
    assert "ValueError:" in documentation
    assert "std::invalid_argument" not in documentation
    with pytest.raises(ValueError, match="potential-to-distance conversion"):
        physical_population_stability(sidb_layout(), precision_for_distance_corresponding_to_potential=16)


def test_three_sidbs_100_lattice() -> None:
    """Check population stability on the Si(100) lattice."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(0, 1, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(1, 0, 1), sidb_dot_tag.NORMAL)
    options: dict[str, Any] = {
        "mu_minus": -0.25,
    }
    result = physical_population_stability(layout, **options)
    assert len(result) == 5
    assert result[0].critical_dot in layout.sidbs()
    assert result[0].system_energy <= result[1].system_energy
    assert result[1].system_energy <= result[2].system_energy

    options["mu_minus"] = -0.32
    result = physical_population_stability(layout, **options)
    assert len(result) == 1


def test_three_sidbs_111_lattice() -> None:
    """Check population stability on the Si(111) lattice."""
    layout = sidb_layout(lattice.si_111_1x1())
    layout.assign_sidb(lattice_site(0, 0, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(0, 1, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(1, 0, 1), sidb_dot_tag.NORMAL)
    options: dict[str, Any] = {
        "mu_minus": -0.25,
    }
    result = physical_population_stability(layout, **options)
    assert len(result) == 5
    assert result[0].critical_dot in layout.sidbs()
    assert result[0].system_energy <= result[1].system_energy
    assert result[1].system_energy <= result[2].system_energy

    options["mu_minus"] = -0.32
    result = physical_population_stability(layout, **options)
    assert len(result) == 2


def test_zero_transition_potential() -> None:
    """A charge transition at zero potential has infinite perturbing-charge distance."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0), sidb_dot_tag.NORMAL)
    results = physical_population_stability(layout, mu_minus=0.0)
    zero_transitions = [
        info.distance_corresponding_to_potential[transition]
        for info in results
        for transition, (_, potential) in info.transition_potentials.items()
        if potential == 0.0
    ]
    assert zero_transitions
    assert all(distance == float("inf") for distance in zero_transitions)


@pytest.mark.parametrize(
    ("precision", "screening", "distance"), [(2, 5.0, 0.77), (3, 5.0, 0.762), (2, float("inf"), 0.89)]
)
def test_single_sidb_transition(precision: int, screening: float, distance: float) -> None:
    """The negative-to-neutral margin maps to the specified distance precision and screening limit."""
    layout = sidb_layout()
    site = lattice_site(1, 1)
    layout.assign_sidb(site)
    results = physical_population_stability(
        layout,
        base=2,
        mu_minus=-0.29,
        lambda_tf=screening,
        precision_for_distance_corresponding_to_potential=precision,
    )
    assert len(results) == 1
    stability = results[0]
    assert stability.critical_dot == site
    assert stability.transition_potentials[transition_type.NEGATIVE_TO_NEUTRAL] == (site, 0.29)
    assert stability.distance_corresponding_to_potential[transition_type.NEGATIVE_TO_NEUTRAL] == pytest.approx(distance)
    for transition in (transition_type.NEUTRAL_TO_POSITIVE, transition_type.POSITIVE_TO_NEUTRAL):
        assert stability.transition_potentials[transition][1] == float("inf")
        assert stability.distance_corresponding_to_potential[transition] == float("inf")


def test_neutral_sidb_has_both_transition_margins() -> None:
    """A neutral SiDB records its margins to both neighboring charge states."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site())
    results = physical_population_stability(layout, base=3, mu_minus=0.5)
    assert len(results) == 1
    potentials = results[0].transition_potentials
    assert potentials[transition_type.NEUTRAL_TO_NEGATIVE][1] == pytest.approx(0.5, abs=1e-14)
    assert potentials[transition_type.NEUTRAL_TO_POSITIVE][1] == pytest.approx(0.09, abs=1e-14)
