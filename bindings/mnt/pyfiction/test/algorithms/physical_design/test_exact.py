from __future__ import annotations

from mnt.pyfiction import (
    eq_type,
    equivalence_checking,
    exact_cartesian,
    exact_hexagonal,
    exact_params,
    exact_stats,
)


def test_exact_cartesian_default(mux21):
    layout = exact_cartesian(mux21)
    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_exact_cartesian_with_parameters(mux21):
    params = exact_params()
    params.border_io = True
    params.crossings = True
    params.scheme = "ESR"

    layout = exact_cartesian(mux21, params)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_exact_cartesian_with_stats(mux21):
    stats = exact_stats()

    layout = exact_cartesian(mux21, statistics=stats)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_exact_hexagonal_default(mux21):
    layout = exact_hexagonal(mux21)
    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_exact_hexagonal_with_parameters(mux21):
    params = exact_params()
    params.border_io = True
    params.crossings = True
    params.scheme = "ESR"

    layout = exact_hexagonal(mux21, params)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_exact_hexagonal_with_stats(mux21):
    stats = exact_stats()

    layout = exact_hexagonal(mux21, statistics=stats)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG
