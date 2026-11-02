# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Tests for ``write_sqd_sim_result``."""

from __future__ import annotations

from typing import TYPE_CHECKING
from xml.etree import ElementTree as ET  # ruff: ignore[suspicious-xml-etree-import] - Only local, fixed test data is parsed.

import pytest

from mnt.pyfiction.sidb import lattice_site, sidb_dot_tag, sidb_layout
from mnt.pyfiction.sidb.simulation import sidb_simulation_result
from mnt.pyfiction.sidb.simulation.engines import quickexact
from mnt.pyfiction.sidb.simulation.io import write_sqd_sim_result

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
