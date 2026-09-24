# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction import (
    apply_bestagon_library,
    apply_qca_one_library,
    apply_sim7_mol_library,
    apply_topolinano_library,
    exact_hexagonal,
    exact_params,
    exact_shifted_cartesian,
    orthogonal,
    technology_constraints,
    technology_network,
)


def test_apply_qca_one_library(mux21: technology_network) -> None:
    """Map the library and report every processed source gate."""
    layout = orthogonal(mux21)

    reports: list[tuple[str, int, int]] = []
    apply_qca_one_library(layout, lambda task, done, total: reports.append((task, done, total)))
    assert reports[-1][1] == reports[-1][2] > 0


def test_apply_sim7_mol_library(mux21: technology_network) -> None:
    """Map the library and report every processed source gate."""
    layout = orthogonal(mux21)

    reports: list[tuple[str, int, int]] = []
    cell_layout = apply_sim7_mol_library(layout, lambda task, done, total: reports.append((task, done, total)))
    assert reports[-1][1] == reports[-1][2] > 0
    assert cell_layout is not None
    assert cell_layout.num_cells() > 0


def test_apply_bestagon_library(mux21: technology_network) -> None:
    """Map the library and report every processed source gate."""
    params = exact_params()
    params.scheme = "ROW"
    params.crossings = True
    params.border_io = True

    layout = exact_hexagonal(mux21, params)

    reports: list[tuple[str, int, int]] = []
    apply_bestagon_library(layout, lambda task, done, total: reports.append((task, done, total)))
    assert reports[-1][1] == reports[-1][2] > 0


def test_apply_topolinano_library(mux21: technology_network) -> None:
    """Map the library and report every processed source gate."""
    params = exact_params()
    params.scheme = "COLUMNAR3"
    params.crossings = True
    params.border_io = True
    params.technology_specifics = technology_constraints.TOPOLINANO

    layout = exact_shifted_cartesian(mux21, params)

    reports: list[tuple[str, int, int]] = []
    apply_topolinano_library(layout, lambda task, done, total: reports.append((task, done, total)))
    assert reports[-1][1] == reports[-1][2] > 0
