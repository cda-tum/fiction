# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING, Any

import pytest

from mnt.pyfiction.sidb import lattice, lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.io import read_sqd_layout
from mnt.pyfiction.sidb.simulation import sidb_simulation_engine
from mnt.pyfiction.sidb.simulation.analysis import (
    critical_temperature_gate_based,
    critical_temperature_non_gate_based,
    critical_temperature_stats,
)
from mnt.pyfiction.sidb.simulation.logic import (
    bdl_wire_selection,
    detect_bdl_pairs,
    detect_bdl_wires,
    generate_bdl_input_pattern_layouts,
)
from mnt.pyfiction.synthesis import create_and_tt, create_not_tt, create_xor_tt

if TYPE_CHECKING:
    from pathlib import Path


@pytest.mark.parametrize(
    "lat",
    [pytest.param(lattice.si_100_2x1(), id="100"), pytest.param(lattice.si_111_1x1(), id="111")],
)
def test_perturber_and_sidb_pair(lat: lattice) -> None:
    """Check non-gate critical temperature on both supported lattices.

    Args:
        lat: Lattice used for the test layout.
    """
    layout = sidb_layout(lat)
    layout.assign_sidb(lattice_site(0, 0, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(4, 0, 1), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(6, 0, 1), sidb_dot_tag.NORMAL)

    options: dict[str, Any] = {}

    options["sim_engine"] = sidb_simulation_engine.QUICKEXACT

    stats = critical_temperature_stats()

    assert critical_temperature_non_gate_based(layout, stats=stats, **options) == 400

    assert stats.algorithm_name == "QuickExact"
    assert stats.num_valid_lyt == 1


def test_gate_based_simulation(resources_dir: Path) -> None:
    """Check gate-based critical temperature for an XOR gate.

    Args:
        resources_dir: Directory that contains the test layout.
    """
    layout = read_sqd_layout(str(resources_dir / "hex_21_inputsdbp_xor_v1.sqd"), "xor_gate")
    options: dict[str, Any] = {}

    options["base"] = 2

    options["sim_engine"] = sidb_simulation_engine.QUICKEXACT

    stats = critical_temperature_stats()
    spec = [create_xor_tt()]

    assert critical_temperature_gate_based(layout, spec, stats=stats, **options) <= 200

    assert stats.algorithm_name == "QuickExact"


def test_bestagon_inv(resources_dir: Path) -> None:
    """Check a Bestagon inverter with QuickSim.

    Args:
        resources_dir: Directory that contains the test layout.
    """
    layout = read_sqd_layout(
        str(resources_dir / "hex_11_inputsdbp_inv_straight_v0_manual.sqd"),
        "inverter_input_0",
    )

    options: dict[str, Any] = {}

    options["sim_engine"] = sidb_simulation_engine.QUICKSIM
    options["base"] = 2  # QuickSim simulates two charge states only

    stats = critical_temperature_stats()
    spec = [create_not_tt()]

    assert critical_temperature_gate_based(layout, spec, stats=stats, **options) <= 400

    assert stats.algorithm_name == "QuickSim"
    assert stats.num_valid_lyt > 1


def test_bestagon_inv_with_different_mu(resources_dir: Path) -> None:
    """Check a Bestagon inverter at a different chemical potential.

    Args:
        resources_dir: Directory that contains the test layout.
    """
    layout = read_sqd_layout(
        str(resources_dir / "hex_11_inputsdbp_inv_straight_v0_manual.sqd"),
        "inverter_input_0",
    )

    options: dict[str, Any] = {
        "base": 2,
        "mu_minus": -0.2,
    }

    options["sim_engine"] = sidb_simulation_engine.QUICKEXACT

    stats = critical_temperature_stats()
    spec = [create_not_tt()]

    assert critical_temperature_gate_based(layout, spec, stats=stats, **options) <= 5

    assert stats.algorithm_name == "QuickExact"


def test_critical_temperature_with_input_pattern_layouts() -> None:
    """Compare pre-generated input layouts with the layout-based overload."""
    lyt = sidb_layout()

    lyt.assign_sidb(lattice_site(0, 0, 1), sidb_dot_tag.INPUT)
    lyt.assign_sidb(lattice_site(2, 1, 1), sidb_dot_tag.INPUT)

    lyt.assign_sidb(lattice_site(20, 0, 1), sidb_dot_tag.INPUT)
    lyt.assign_sidb(lattice_site(19, 1, 1), sidb_dot_tag.INPUT)

    lyt.assign_sidb(lattice_site(4, 2, 1), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(6, 3, 1), sidb_dot_tag.NORMAL)

    lyt.assign_sidb(lattice_site(14, 3, 1), sidb_dot_tag.NORMAL)
    lyt.assign_sidb(lattice_site(16, 2, 1), sidb_dot_tag.NORMAL)

    lyt.assign_sidb(lattice_site(10, 6, 0), sidb_dot_tag.OUTPUT)
    lyt.assign_sidb(lattice_site(10, 7, 0), sidb_dot_tag.OUTPUT)

    lyt.assign_sidb(lattice_site(10, 9, 1), sidb_dot_tag.NORMAL)

    options: dict[str, Any] = {
        "sim_engine": sidb_simulation_engine.QUICKEXACT,
    }
    options.update({"base": 2, "mu_minus": -0.28, "epsilon_r": 5.6, "lambda_tf": 5.0})

    input_bdl_wires = detect_bdl_wires(lyt, wire_selection=bdl_wire_selection.INPUT)
    output_bdl_wires = detect_bdl_wires(lyt, wire_selection=bdl_wire_selection.OUTPUT)
    output_bdl_pairs = detect_bdl_pairs(lyt, sidb_dot_tag.OUTPUT)

    input_pattern_layouts = generate_bdl_input_pattern_layouts(lyt, input_bdl_wires)

    # a 2-input gate has 4 input patterns
    assert len(input_pattern_layouts) == 4

    reference_stats = critical_temperature_stats()
    reference_ct = critical_temperature_gate_based(lyt, [create_and_tt()], stats=reference_stats, **options)

    stats = critical_temperature_stats()
    ct = critical_temperature_gate_based(
        input_pattern_layouts,
        [create_and_tt()],
        output_bdl_pairs,
        input_bdl_wires,
        output_bdl_wires,
        stats=stats,
        **options,
    )

    # the two overloads run the same computation, so the results must be identical
    assert ct == reference_ct
    assert stats.num_valid_lyt == reference_stats.num_valid_lyt
    assert stats.algorithm_name == reference_stats.algorithm_name

    # a layout list that does not match the specification is rejected
    with pytest.raises(ValueError, match="expected 4 input pattern layouts"):
        critical_temperature_gate_based(
            input_pattern_layouts[:1], [create_and_tt()], output_bdl_pairs, input_bdl_wires, output_bdl_wires, **options
        )

    # more output BDL pairs than truth tables is rejected
    with pytest.raises(ValueError, match="expected 1 output BDL pairs"):
        critical_temperature_gate_based(
            input_pattern_layouts,
            [create_and_tt()],
            [*output_bdl_pairs, output_bdl_pairs[0]],
            input_bdl_wires,
            output_bdl_wires,
            **options,
        )
