# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``write_sqd_sim_result``."""

from __future__ import annotations

import math
import time
from collections import Counter
from datetime import datetime, timedelta
from typing import TYPE_CHECKING
from xml.etree import ElementTree as ET  # ruff: ignore[suspicious-xml-etree-import] - Only local, fixed test data is parsed.

import pytest

from mnt.pyfiction.sidb import charge_distribution, lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.model import sidb_charge_state, sidb_simulation_parameters
from mnt.pyfiction.sidb.simulation import sidb_simulation_result
from mnt.pyfiction.sidb.simulation.engines import exhaustive_ground_state_simulation, quickexact
from mnt.pyfiction.sidb.simulation.io import write_sqd_sim_result
from mnt.pyfiction.utils import __repo__, __version__

if TYPE_CHECKING:
    from pathlib import Path


def test_write_sqd_sim_result(tmp_path: Path) -> None:
    """The writer emits a SiQAD simulation result.

    Args:
        tmp_path: Temporary output directory.
    """

    layout = sidb_layout()
    layout.assign_sidb(lattice_site(0, 0, 0), sidb_dot_tag.NORMAL)
    layout.assign_sidb(lattice_site(2, 0, 0), sidb_dot_tag.NORMAL)

    result = quickexact(layout)
    assert result.charge_distributions
    result.algorithm_name = "a & <simulation> \"quoted\" 'name'\t\n\rend é 中 𐀀"

    filename = tmp_path / "result.xml"
    write_sqd_sim_result(result, str(filename))

    text = filename.read_text(encoding="utf-8")
    assert text.startswith("<?xml")
    assert "<sim_out>" in text
    assert "<dbdot" in text
    # Parse only local writer output built from fixed test data.
    assert ET.fromstring(text).findtext("eng_info/engine") == result.algorithm_name  # ruff: ignore[suspicious-xml-element-tree-usage]


@pytest.mark.parametrize("control", ["\0", "\x01", "\x1f", "\ufffe", "\uffff"])
def test_invalid_engine_name_preserves_destination(tmp_path: Path, control: str) -> None:
    """Illegal XML characters raise ValueError before replacing an existing file.

    Args:
        tmp_path: Temporary output directory.
        control: Illegal XML character.
    """

    result = sidb_simulation_result()
    result.algorithm_name = control
    filename = tmp_path / "result.xml"
    filename.write_text("original")

    # The API promises the exception type, not diagnostic wording.
    with pytest.raises(ValueError):  # ruff: ignore[pytest-raises-too-broad]
        write_sqd_sim_result(result, str(filename))

    assert filename.read_text() == "original"
    assert list(tmp_path.iterdir()) == [filename]


def test_empty_simulation_xml_metadata(tmp_path: Path) -> None:
    """Empty results preserve physical metadata and emit a canonical timestamp for the write."""
    result = sidb_simulation_result()
    result.algorithm_name = "TestSim"
    result.simulation_runtime = timedelta(seconds=42)
    result.simulation_parameters = sidb_simulation_parameters(
        base_number=2, mu_minus=-0.25, relative_permittivity=4.2, screening_distance=6.0
    )
    path = tmp_path / "empty.xml"
    before = math.floor(time.time())
    write_sqd_sim_result(result, str(path))
    after = math.floor(time.time())
    root = ET.fromstring(path.read_text(encoding="utf-8"))  # ruff: ignore[suspicious-xml-element-tree-usage] -- local writer output
    assert root.findtext("eng_info/engine") == "TestSim"
    assert root.findtext("eng_info/version") == __version__
    assert root.findtext("eng_info/repo") == __repo__
    assert root.findtext("eng_info/return_code") == "0"
    assert root.findtext("eng_info/time_elapsed_s") == "42"
    assert root.findtext("sim_params/debye_length") == "6"
    assert root.findtext("sim_params/eps_r") == "4.2"
    assert root.findtext("sim_params/muzm") == "-0.25"
    for container in ("physloc", "elec_dist"):
        element = root.find(container)
        assert element is not None
        assert list(element) == []
    timestamp = root.findtext("eng_info/timestamp")
    assert timestamp is not None
    parsed = datetime.fromisoformat(timestamp)
    assert parsed.strftime("%Y-%m-%d %H:%M:%S") == timestamp
    assert any(before <= parsed.replace(fold=fold).timestamp() <= after for fold in (0, 1))


@pytest.mark.parametrize("base", [2, 3])
def test_simulation_xml_preserves_charges_and_physical_locations(tmp_path: Path, base: int) -> None:
    """SiQAD XML preserves charge configurations and positions with state count three for both simulation bases."""
    layout = sidb_layout()
    for x in (5, 6, 7):
        layout.assign_sidb(lattice_site(x, 0))
    result = exhaustive_ground_state_simulation(layout, base=base)
    path = tmp_path / "states.xml"
    write_sqd_sim_result(result, str(path))
    root = ET.fromstring(path.read_text(encoding="utf-8"))  # ruff: ignore[suspicious-xml-element-tree-usage] -- local writer output
    assert root.tag == "sim_out"
    assert root.findtext("eng_info/engine") == "ExGS"
    assert [(float(dot.attrib["x"]), float(dot.attrib["y"])) for dot in root.findall("physloc/dbdot")] == [
        (19.2, 0.0),
        (23.04, 0.0),
        (26.88, 0.0),
    ]
    distributions = root.findall("elec_dist/dist")
    assert Counter(dist.text for dist in distributions) == Counter(["-+-", "0-0"] if base == 3 else ["0-0"])
    assert [float(dist.attrib["energy"]) for dist in distributions] == pytest.approx(
        [-0.953108, 0.0] if base == 3 else [0.0], abs=1e-6, rel=0
    )
    assert all(
        dist.attrib["physically_valid"] == "1" and dist.attrib["count"] == "1" and dist.attrib["state_count"] == "3"
        for dist in distributions
    )


def test_simulation_xml_rejects_shifted_sites(tmp_path: Path) -> None:
    """The XML writer validates complete distribution geometry before replacing the destination."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site())
    shifted = sidb_layout()
    shifted.assign_sidb(lattice_site(1, 0))
    result = sidb_simulation_result()
    result.layout = layout
    result.charge_distributions = [charge_distribution(shifted)]
    path = tmp_path / "preserved.xml"
    path.write_bytes(b"original")
    with pytest.raises(ValueError):  # ruff: ignore[pytest-raises-too-broad] -- exception type is the contract
        write_sqd_sim_result(result, str(path))
    assert path.read_bytes() == b"original"
    assert list(tmp_path.iterdir()) == [path]


def test_xml_orders_finite_and_nonfinite_energies(tmp_path: Path) -> None:
    """The XML writer preserves every configuration while ordering infinities and NaN around finite energies."""
    layout = sidb_layout()
    layout.assign_sidb(lattice_site())
    layout.assign_sidb(lattice_site(2, 0))
    states = {"-": sidb_charge_state.NEGATIVE, "0": sidb_charge_state.NEUTRAL, "+": sidb_charge_state.POSITIVE}
    result = sidb_simulation_result()
    result.layout = layout
    distributions = []
    for configuration, energy in [("--", math.nan), ("-0", 2.0), ("00", -math.inf), ("0+", math.inf), ("++", -3.0)]:
        distribution = charge_distribution(layout)
        for index, state in enumerate(configuration):
            distribution.assign_charge_state_by_index(index, states[state])
        distribution.assign_energy(energy)
        distributions.append(distribution)
    result.charge_distributions = distributions
    path = tmp_path / "energies.xml"
    write_sqd_sim_result(result, str(path))
    root = ET.fromstring(path.read_text(encoding="utf-8"))  # ruff: ignore[suspicious-xml-element-tree-usage] -- local writer output
    nodes = root.findall("elec_dist/dist")
    assert [node.text for node in nodes] == ["00", "++", "-0", "0+", "--"]
    energies = [float(node.attrib["energy"]) for node in nodes]
    assert energies[:-1] == [-math.inf, -3.0, 2.0, math.inf]
    assert math.isnan(energies[-1])
