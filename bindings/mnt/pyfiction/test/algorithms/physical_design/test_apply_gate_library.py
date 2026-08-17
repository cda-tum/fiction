from __future__ import annotations

from mnt.pyfiction import (
    apply_bestagon_library,
    apply_qca_one_library,
    apply_topolinano_library,
    exact_hexagonal,
    exact_params,
    exact_shifted_cartesian,
    orthogonal,
    technology_constraints,
)


def test_apply_qca_one_library(mux21):
    layout = orthogonal(mux21)

    apply_qca_one_library(layout)


def test_apply_bestagon_library(mux21):
    params = exact_params()
    params.scheme = "ROW"
    params.crossings = True
    params.border_io = True

    layout = exact_hexagonal(mux21, params)

    apply_bestagon_library(layout)


def test_apply_topolinano_library(mux21):
    params = exact_params()
    params.scheme = "COLUMNAR3"
    params.crossings = True
    params.border_io = True
    params.technology_specifics = technology_constraints.TOPOLINANO

    layout = exact_shifted_cartesian(mux21, params)

    apply_topolinano_library(layout)
