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

from mnt.pyfiction.networks import simulate_outputs
from mnt.pyfiction.networks.io import read_network
from mnt.pyfiction.synthesis import (
    all_standard_2_input_functions,
    all_standard_3_input_functions,
    all_supported_standard_functions,
    and_or_not,
    and_or_not_maj,
    missing_required_gates_exception,
    technology_mapping,
    technology_mapping_stats,
)
from mnt.pyfiction.verification import count_gate_types, eq_type, equivalence_checking

if TYPE_CHECKING:
    from pathlib import Path

    from mnt.pyfiction.networks import technology_network


@pytest.mark.parametrize(
    ("gate", "expression"),
    [("lt2", "a & ~b"), ("gt2", "~a & b"), ("le2", "~(a & ~b)"), ("ge2", "~(~a & b)")],
)
def test_mapping_respects_asymmetric_gate_selection(tmp_path: Path, gate: str, expression: str) -> None:
    """The mapped network uses the selected gate and preserves the truth table."""
    path = tmp_path / "gate.v"
    path.write_text(
        f"module top(a,b,f);\ninput a,b;\noutput f;\nassign f = {expression};\nendmodule\n", encoding="utf-8"
    )
    network = read_network(str(path))
    options: dict[str, Any] = {"inv": True, gate: True}
    mapped = technology_mapping(network, **options)
    counts = count_gate_types(mapped)
    assert getattr(counts, f"num_{gate}") == 1
    assert simulate_outputs(mapped) == simulate_outputs(network)


GATE_LIBRARIES = [
    pytest.param(and_or_not, id="and_or_not"),
    pytest.param(and_or_not_maj, id="and_or_not_maj"),
    pytest.param(all_standard_2_input_functions, id="all_standard_2_input_functions"),
    pytest.param(all_standard_3_input_functions, id="all_standard_3_input_functions"),
    pytest.param(all_supported_standard_functions, id="all_supported_standard_functions"),
]


def test_missing_gate_exception_export():
    """Users can catch the exception exposed by the technology mapper."""
    assert issubclass(missing_required_gates_exception, RuntimeError)


def test_mapping_default(mux21: technology_network) -> None:
    """An empty gate library is rejected before native mapping."""
    with pytest.raises(missing_required_gates_exception, match="missing required gates"):
        technology_mapping(mux21)


@pytest.mark.parametrize("make_options", GATE_LIBRARIES)
def test_mapping_with_parameters(mux21, make_options):
    mapped_network = technology_mapping(mux21, **make_options())

    assert equivalence_checking(mux21, mapped_network) == eq_type.STRONG


@pytest.mark.parametrize("make_options", GATE_LIBRARIES)
def test_mapping_with_stats(mux21, make_options):
    stats = technology_mapping_stats()
    mapped_network = technology_mapping(mux21, stats=stats, **make_options())

    assert equivalence_checking(mux21, mapped_network) == eq_type.STRONG
