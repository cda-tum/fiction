# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

"""Public SiDB workflow contracts."""

from __future__ import annotations

import pytest

from mnt.pyfiction.sidb import DotTag, LatticeSite, SiDBLayout
from mnt.pyfiction.sidb.analysis import (
    CriticalTemperatureResult,
    OperationalResult,
    OperationalStatus,
    critical_temperature_gate_based,
    critical_temperature_non_gate_based,
    is_operational,
)


def test_temperature_result() -> None:
    """Thermal analysis returns a named value and read-only statistics."""
    layout = SiDBLayout()
    layout.assign_sidb(LatticeSite(), DotTag.NORMAL)
    result = critical_temperature_non_gate_based(layout)
    assert isinstance(result, CriticalTemperatureResult)
    assert result.temperature >= 0.0
    assert result.stats.num_valid_lyt == 1
    for field in ("temperature", "stats"):
        with pytest.raises(AttributeError):
            setattr(result, field, None)


def test_explicit_analysis_wiring() -> None:
    """Explicit patterns and wiring must form a complete analysis input."""
    with pytest.raises(ValueError, match="both input and output wires"):
        is_operational([], [])
    with pytest.raises(ValueError, match="both input and output wires"):
        is_operational(SiDBLayout(), [], input_wires=[])
    with pytest.raises(ValueError, match="output pairs and input and output wires"):
        critical_temperature_gate_based([], [])
    with pytest.raises(ValueError, match="one layout per input pattern"):
        critical_temperature_gate_based(SiDBLayout(), [], output_bdl_pairs=[])


@pytest.mark.parametrize("status", list(OperationalStatus))
def test_operational_result_truth_value(status: OperationalStatus) -> None:
    """Truth testing an operational check reflects its logical status."""
    result = OperationalResult(status, 1)
    assert bool(result) == (status == OperationalStatus.OPERATIONAL)
    assert result.simulator_invocations == 1
