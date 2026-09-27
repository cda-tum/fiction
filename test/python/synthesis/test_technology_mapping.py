# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from typing import TYPE_CHECKING

import pytest

from mnt.pyfiction.synthesis import (
    MissingRequiredGatesError,
    all_standard_2_input_functions,
    all_standard_3_input_functions,
    all_supported_standard_functions,
    and_or_not,
    and_or_not_maj,
    technology_mapping,
)
from mnt.pyfiction.verification import EquivalenceType, equivalence_checking

if TYPE_CHECKING:
    from mnt.pyfiction.networks import TechnologyNetwork

GATE_LIBRARIES = [
    pytest.param(and_or_not, id="and_or_not"),
    pytest.param(and_or_not_maj, id="and_or_not_maj"),
    pytest.param(all_standard_2_input_functions, id="all_standard_2_input_functions"),
    pytest.param(all_standard_3_input_functions, id="all_standard_3_input_functions"),
    pytest.param(all_supported_standard_functions, id="all_supported_standard_functions"),
]


def test_missing_gate_exception_export():
    """Users can catch the exception exposed by the technology mapper."""
    assert issubclass(MissingRequiredGatesError, RuntimeError)


def test_mapping_default(mux21: TechnologyNetwork) -> None:
    """An empty gate library is rejected before native mapping."""
    with pytest.raises(MissingRequiredGatesError, match="missing required gates"):
        technology_mapping(mux21)


@pytest.mark.parametrize("make_params", GATE_LIBRARIES)
def test_mapping_with_parameters(mux21, make_params):
    mapped_network = technology_mapping(mux21, params=make_params()).network

    assert equivalence_checking(mux21, mapped_network).eq == EquivalenceType.STRONG


@pytest.mark.parametrize("make_params", GATE_LIBRARIES)
def test_mapping_with_stats(mux21, make_params):
    result = technology_mapping(mux21, params=make_params())
    mapped_network = result.network
    assert not result.stats.mapper_stats.mapping_error

    assert equivalence_checking(mux21, mapped_network).eq == EquivalenceType.STRONG
