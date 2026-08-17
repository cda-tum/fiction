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


def test_mapping_default(mux21):
    assert mux21.num_gates() == 5

    mapped_network = technology_mapping(mux21)

    assert equivalence_checking(mux21, mapped_network) == eq_type.NO


def test_mapping_with_parameters(mux21):
    for params in [
        and_or_not(),
        and_or_not_maj(),
        all_standard_2_input_functions(),
        all_standard_3_input_functions(),
        all_supported_standard_functions(),
    ]:
        mapped_network = technology_mapping(mux21, params)

        assert equivalence_checking(mux21, mapped_network) == eq_type.STRONG


def test_mapping_with_stats(mux21):
    for params in [
        and_or_not(),
        and_or_not_maj(),
        all_standard_2_input_functions(),
        all_standard_3_input_functions(),
        all_supported_standard_functions(),
    ]:
        stats = technology_mapping_stats()
        mapped_network = technology_mapping(mux21, params=params, stats=stats)

        assert equivalence_checking(mux21, mapped_network) == eq_type.STRONG
