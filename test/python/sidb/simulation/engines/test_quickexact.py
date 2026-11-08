# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``quickexact``."""

from __future__ import annotations

from collections import Counter
from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction.sidb import lattice, lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.io import read_sqd_layout
from mnt.pyfiction.sidb.model import sidb_charge_state, sidb_defect, sidb_defect_type
from mnt.pyfiction.sidb.simulation.engines import (
    automatic_base_number_detection,
    exhaustive_ground_state_simulation,
    quickexact,
)

if TYPE_CHECKING:
    from pathlib import Path


def test_three_sidbs() -> None:
    """QuickExact simulates a three-SiDB layout."""

    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(1, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(2, 0, 0), sidb_dot_tag.NORMAL)

    options: dict[str, Any] = {
        "base": 2,
        "mu_minus": -0.25,
        "epsilon_r": 5.6,
        "lambda_tf": 5.0,
        "base_number_detection": automatic_base_number_detection.OFF,
    }

    def check_distributions(result: Any, base: int, *, expect_positive: bool = False) -> None:
        """Compare engine and exhaustive charge configurations with derived literals."""
        reference = exhaustive_ground_state_simulation(
            layout, base=base, **{key: options[key] for key in ("mu_minus", "epsilon_r", "lambda_tf")}
        )
        sites = [lattice_site(x, 0, 0) for x in range(3)]
        expected = Counter(tuple(cd.get_charge_state(site) for site in sites) for cd in reference.charge_distributions)
        actual = Counter(tuple(cd.get_charge_state(site) for site in sites) for cd in result.charge_distributions)
        assert expected
        negative, neutral, positive = sidb_charge_state.NEGATIVE, sidb_charge_state.NEUTRAL, sidb_charge_state.POSITIVE
        singles = [(negative, neutral, neutral), (neutral, negative, neutral), (neutral, neutral, negative)]
        if base == 2:
            known = singles
        elif options["epsilon_r"] == 5.6:
            known = [*singles, (negative, positive, negative)]
        else:
            known = [(negative, positive, negative), (positive, negative, positive)]
        assert expected == Counter(known)
        assert any(sidb_charge_state.POSITIVE in state for state in expected) is expect_positive
        assert actual == expected

    result = quickexact(layout, **options)

    assert result.algorithm_name == "QuickExact"
    assert result.layout == layout
    assert len(result.charge_distributions) <= 3
    check_distributions(result, 2)

    options["base_number_detection"] = automatic_base_number_detection.ON

    result = quickexact(layout, **options)
    assert len(result.charge_distributions) <= 4
    check_distributions(result, 3, expect_positive=True)

    options["epsilon_r"] = 2
    options["lambda_tf"] = 2
    result = quickexact(layout, **options)
    assert len(result.charge_distributions) <= 2
    check_distributions(result, 3, expect_positive=True)


def test_perturber_and_sidb_pair_111() -> None:
    """QuickExact finds the H-Si(111)-1x1 ground state."""

    layout = sidb_layout(lattice.si_111_1x1())
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(1, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(2, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(3, 0, 0), sidb_dot_tag.NORMAL)

    result = quickexact(layout, base=2, mu_minus=-0.32, base_number_detection=automatic_base_number_detection.OFF)

    assert result.algorithm_name == "QuickExact"

    groundstate = result.groundstates()

    assert len(groundstate) == 1

    assert groundstate[0].get_charge_state(lattice_site(0, 0, 0)) == sidb_charge_state.NEGATIVE
    assert groundstate[0].get_charge_state(lattice_site(1, 0, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state(lattice_site(2, 0, 0)) == sidb_charge_state.NEUTRAL
    assert groundstate[0].get_charge_state(lattice_site(3, 0, 0)) == sidb_charge_state.NEGATIVE

    # the result offers the same lookup by distribution index and site
    assert result.charge_state(0, lattice_site(0, 0, 0)) == result.charge_distributions[0].get_charge_state(
        lattice_site(0, 0, 0)
    )


def test_simulate_all_inputs_of_and_gate(resources_dir: Path) -> None:
    """QuickExact finds a charge distribution for every AND-gate input.

    Args:
        resources_dir: Directory that contains the test layout.
    """

    and_gate = read_sqd_layout(str(resources_dir / "Bestagon_AND_mu_025_v0.sqd"))
    physical_parameters: dict[str, Any] = {"base": 3, "mu_minus": -0.32, "epsilon_r": 5.6, "lambda_tf": 5.0}
    physical_parameters["base"] = 2
    physical_parameters["epsilon_r"] = 5.6
    physical_parameters["lambda_tf"] = 5.0  # (nm)
    physical_parameters["mu_minus"] = -0.25  # (eV)
    quickexact_parameter = physical_parameters.copy()

    left_a, left_b = lattice_site(0, 0, 0), lattice_site(2, 1, 0)
    right_a, right_b = lattice_site(24, 1, 0), lattice_site(26, 0, 0)

    for left, right in [(left_b, right_a), (left_b, right_b), (left_a, right_a), (left_a, right_b)]:
        # delete one SiDB of each input BDL pair to set the input pattern
        and_gate.assign_sidb(left, sidb_dot_tag.EMPTY)
        and_gate.assign_sidb(right, sidb_dot_tag.EMPTY)

        assert len(quickexact(and_gate, **quickexact_parameter).charge_distributions) > 0

        # restore the original layout
        and_gate.assign_sidb(left, sidb_dot_tag.INPUT)
        and_gate.assign_sidb(right, sidb_dot_tag.INPUT)


@pytest.mark.parametrize(
    ("potential", "expected"),
    [(-0.26, sidb_charge_state.NEUTRAL), (-1.0, sidb_charge_state.POSITIVE), (1.0, sidb_charge_state.NEGATIVE)],
)
@pytest.mark.parametrize("scope", ["local", "global"])
def test_external_potential_changes_single_charge(scope: str, potential: float, expected: sidb_charge_state) -> None:
    """Local and global applied potentials select the expected charge state and energy."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site())
    options: dict[str, Any] = (
        {"local_external_potential": {lattice_site(): potential}}
        if scope == "local"
        else {"global_potential": potential}
    )
    result = quickexact(layout, base=3, mu_minus=-0.25, **options)
    assert len(result.charge_distributions) == 1
    cd = result.charge_distributions[0]
    assert cd.charge_states() == [expected]
    sign = {sidb_charge_state.NEGATIVE: -1, sidb_charge_state.NEUTRAL: 0, sidb_charge_state.POSITIVE: 1}[expected]
    assert cd.energy() == pytest.approx(sign * potential)
    if scope == "global":
        assert result.additional_simulation_parameters["global_potential"] == potential


@pytest.mark.parametrize(
    ("position", "charge", "epsilon_r", "lambda_tf", "expected"),
    [
        ((1, 2), -1, 5.6, 5.0, sidb_charge_state.NEUTRAL),
        ((1, 2), -1, 5.6, 2.0, sidb_charge_state.NEGATIVE),
        ((1, 6), -1, 0.3, 5.0, sidb_charge_state.POSITIVE),
        ((1, 2), -10, 5.6, 5.0, sidb_charge_state.POSITIVE),
        ((1, 2), -10, 5.6, 5e-4, sidb_charge_state.NEGATIVE),
    ],
)
def test_defect_parameters_change_single_charge(
    position: tuple[int, int], charge: int, epsilon_r: float, lambda_tf: float, expected: sidb_charge_state
) -> None:
    """Defect charge, permittivity, and screening enter the physical model independently."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site(1, 3))
    layout.assign_defect(lattice_site(*position), sidb_defect(sidb_defect_type.UNKNOWN, charge, epsilon_r, lambda_tf))
    result = quickexact(layout, base=2, mu_minus=-0.25)
    assert result.layout == layout
    assert len(result.charge_distributions) == 1
    assert result.charge_distributions[0].charge_states() == [expected]
