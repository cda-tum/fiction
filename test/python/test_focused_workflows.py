# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""User-facing contracts shared by the focused FCN workflows."""

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction import inml, mol_qca, qca, sidb
from mnt.pyfiction.networks import AigNetwork, TechnologyNetwork, simulate_outputs
from mnt.pyfiction.networks.io import read_network, write_verilog
from mnt.pyfiction.physical_design import orthogonal
from mnt.pyfiction.synthesis import and_or_not, convert_network, technology_mapping
from mnt.pyfiction.verification import EquivalenceType, equivalence_checking, gate_level_drvs

if TYPE_CHECKING:
    from pathlib import Path


def test_network_workflow_preserves_source(mux21: TechnologyNetwork, tmp_path: Path) -> None:
    """Path inputs, representation selection, and mapping compose without modifying the source."""
    path = tmp_path / "network.v"
    write_verilog(mux21, path)
    aig = read_network(path, network_type=AigNetwork)
    assert isinstance(aig, AigNetwork)
    assert simulate_outputs(aig) == simulate_outputs(mux21)
    copy = convert_network(mux21)
    copy.name = "independent"
    assert mux21.name != copy.name
    mapped = technology_mapping(aig, params=and_or_not())
    assert not mapped.stats.mapper_stats.mapping_error
    result = equivalence_checking(mux21, mapped.network)
    assert result.eq == EquivalenceType.STRONG
    assert result.counter_example == []


def test_network_selectors_reject_unsupported_types(mux21: TechnologyNetwork, tmp_path: Path) -> None:
    """Unsupported representations fail before native I/O or conversion."""
    with pytest.raises(ValueError, match="unsupported network type"):
        read_network(tmp_path / "missing.v", network_type=str)  # ty: ignore[invalid-argument-type]  # unsupported selector
    with pytest.raises(ValueError, match="unsupported network type"):
        convert_network(mux21, network_type=str)  # ty: ignore[invalid-argument-type]  # unsupported selector


def test_design_rules_return_a_report(mux21: TechnologyNetwork, capsys: pytest.CaptureFixture[str]) -> None:
    """A check returns its report without printing or requiring an output object."""
    result = gate_level_drvs(orthogonal(mux21).layout)
    assert result.drvs == 0
    assert result.warnings == 0
    assert result.report
    assert not capsys.readouterr().out


@pytest.mark.parametrize("dimension", [-1.0, float("inf"), float("nan")])
def test_area_rejects_invalid_dimensions(dimension: float) -> None:
    """Every technology rejects dimensions that cannot describe physical cells."""
    with pytest.raises(ValueError, match="finite and nonnegative"):
        qca.area(qca.QCALayout(), width=dimension)
    with pytest.raises(ValueError, match="finite and nonnegative"):
        mol_qca.area(mol_qca.MolecularQCALayout(), height=dimension)
    with pytest.raises(ValueError, match="finite and nonnegative"):
        inml.area(inml.INMLLayout(), hspace=dimension)
    with pytest.raises(ValueError, match="finite and nonnegative"):
        sidb.area(sidb.SiDBLayout(), vspace=dimension)


def test_sidb_io_accepts_paths(tmp_path: Path) -> None:
    """SQD paths round-trip named layouts and SVG paths accept optional charges."""
    layout = sidb.SiDBLayout()
    layout.assign_sidb(sidb.LatticeSite(0, 0), sidb.DotTag.NORMAL)
    path = tmp_path / "sample.sqd"
    sidb.io.write_sqd_layout(layout, path)
    restored = sidb.io.read_sqd_layout(path, name="sample")
    assert restored.name == "sample"
    assert restored.num_dots() == 1
    charges = sidb.simulation.PotentialLandscape(restored).evaluate([sidb.ChargeState.NEGATIVE])
    svg = tmp_path / "sample.svg"
    sidb.io.write_sidb_layout_svg(restored, svg, charges=charges)
    assert svg.read_text(encoding="utf-8") == sidb.io.write_sidb_layout_svg_to_string(restored, charges=charges)
