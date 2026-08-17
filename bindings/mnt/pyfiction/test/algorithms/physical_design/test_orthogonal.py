from mnt.pyfiction import (
    eq_type,
    equivalence_checking,
    orthogonal,
    orthogonal_params,
    orthogonal_stats,
)


def test_orthogonal_default(mux21):
    layout = orthogonal(mux21)
    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_orthogonal_with_parameters(mux21):
    params = orthogonal_params()

    layout = orthogonal(mux21, params)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_orthogonal_with_stats(mux21):
    stats = orthogonal_stats()

    layout = orthogonal(mux21, statistics=stats)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG
