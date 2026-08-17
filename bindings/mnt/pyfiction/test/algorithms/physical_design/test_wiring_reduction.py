import pytest

from mnt.pyfiction import (
    eq_type,
    equivalence_checking,
    orthogonal,
    wiring_reduction,
    wiring_reduction_params,
    wiring_reduction_stats,
)


def test_wiring_reduction_default(mux21):
    layout = orthogonal(mux21)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG

    wiring_reduction(layout)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_wiring_reduction_with_parameters(mux21):
    layout = orthogonal(mux21)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG

    params = wiring_reduction_params()
    wiring_reduction(layout, params)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG


def test_wiring_reduction_with_stats(mux21):
    layout = orthogonal(mux21)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG

    stats = wiring_reduction_stats()
    wiring_reduction(layout, statistics=stats)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    assert stats.x_size_before == 6
    assert stats.y_size_before == 8
    assert stats.x_size_after == 6
    assert stats.y_size_after == 5
    assert stats.num_wires_before == 21
    assert stats.num_wires_after == 15
    assert stats.wiring_improvement == pytest.approx(28.57)
    assert stats.area_improvement == pytest.approx(37.5)


def test_wiring_reduction_with_stats_and_parameters(mux21):
    layout = orthogonal(mux21)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG

    params = wiring_reduction_params()
    params.timeout = 1000000

    stats = wiring_reduction_stats()
    wiring_reduction(layout, params, stats)

    assert equivalence_checking(mux21, layout) == eq_type.STRONG
    assert stats.time_total.total_seconds() > 0
    assert stats.x_size_before == 6
    assert stats.y_size_before == 8
    assert stats.x_size_after == 6
    assert stats.y_size_after == 5
    assert stats.num_wires_before == 21
    assert stats.num_wires_after == 15
    assert stats.wiring_improvement == pytest.approx(28.57)
    assert stats.area_improvement == pytest.approx(37.5)
