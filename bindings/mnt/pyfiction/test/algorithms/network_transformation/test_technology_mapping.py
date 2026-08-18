import pytest

from mnt.pyfiction import (
    all_standard_2_input_functions,
    all_standard_3_input_functions,
    all_supported_standard_functions,
    and_or_not,
    and_or_not_maj,
    eq_type,
    equivalence_checking,
    technology_mapping,
    technology_mapping_stats,
)

GATE_LIBRARIES = [
    pytest.param(and_or_not, id="and_or_not"),
    pytest.param(and_or_not_maj, id="and_or_not_maj"),
    pytest.param(all_standard_2_input_functions, id="all_standard_2_input_functions"),
    pytest.param(all_standard_3_input_functions, id="all_standard_3_input_functions"),
    pytest.param(all_supported_standard_functions, id="all_supported_standard_functions"),
]


def test_mapping_default(mux21):
    assert mux21.num_gates() == 5

    mapped_network = technology_mapping(mux21)

    assert equivalence_checking(mux21, mapped_network) == eq_type.NO


@pytest.mark.parametrize("make_params", GATE_LIBRARIES)
def test_mapping_with_parameters(mux21, make_params):
    mapped_network = technology_mapping(mux21, make_params())

    assert equivalence_checking(mux21, mapped_network) == eq_type.STRONG


@pytest.mark.parametrize("make_params", GATE_LIBRARIES)
def test_mapping_with_stats(mux21, make_params):
    stats = technology_mapping_stats()
    mapped_network = technology_mapping(mux21, params=make_params(), stats=stats)

    assert equivalence_checking(mux21, mapped_network) == eq_type.STRONG
