# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction import (
    charge_distribution_surface_100,
    charge_distribution_surface_111,
    generate_multiple_random_sidb_layouts,
    generate_random_sidb_layout,
    generate_random_sidb_layout_params,
    sidb_100_lattice,
    sidb_111_lattice,
    sidb_cell_level_layout,
)


def test_area_with_one_sidb_100_lattice():
    params = generate_random_sidb_layout_params()
    params.number_of_sidbs = 1
    params.coordinate_pair = ((10, 10), (10, 10))
    result_lyt = generate_random_sidb_layout(params, sidb_100_lattice())
    assert result_lyt.num_cells() == 1
    cell = (result_lyt.cells())[0]
    assert cell.x == 10
    assert cell.y == 10


def test_area_with_five_sidb_layout() -> None:
    """The generator places five SiDBs in the requested Cartesian area."""
    params = generate_random_sidb_layout_params()
    params.number_of_sidbs = 5
    params.coordinate_pair = ((0, 0), (10, 10))
    result_lyt = generate_random_sidb_layout(params, sidb_cell_level_layout())
    assert result_lyt.num_cells() == 5


def test_area_with_five_sidb_100_lattice() -> None:
    """The generator places five SiDBs on an H-Si(100)-2x1 lattice."""
    params = generate_random_sidb_layout_params()
    params.number_of_sidbs = 5
    params.coordinate_pair = ((0, 0), (10, 10))
    result_lyt = generate_random_sidb_layout(params, sidb_100_lattice())
    assert result_lyt.num_cells() == 5


def test_area_with_five_sidbs_cds_100() -> None:
    """The generator populates an H-Si(100)-2x1 charge-distribution surface."""
    params = generate_random_sidb_layout_params()
    params.number_of_sidbs = 5
    params.coordinate_pair = ((0, 0), (10, 10))
    result_lyt = generate_random_sidb_layout(params, charge_distribution_surface_100())
    assert result_lyt.num_cells() == 5


def test_area_with_five_sidbs_cds_111() -> None:
    """The generator populates an H-Si(111)-1x1 charge-distribution surface."""
    params = generate_random_sidb_layout_params()
    params.number_of_sidbs = 5
    params.coordinate_pair = ((0, 0), (10, 10))
    result_lyt = generate_random_sidb_layout(params, charge_distribution_surface_111())
    assert result_lyt.num_cells() == 5


def test_area_with_one_coordinate_111_lattice():
    params = generate_random_sidb_layout_params()
    params.number_of_sidbs = 1
    params.coordinate_pair = ((10, 10), (10, 10))
    result_lyt = generate_random_sidb_layout(params, sidb_111_lattice())
    assert result_lyt.num_cells() == 1
    cell = (result_lyt.cells())[0]
    assert cell.x == 10
    assert cell.y == 10


def test_impossible_design_of_single_layout():
    params = generate_random_sidb_layout_params()
    params.number_of_sidbs = 2
    result_lyt = generate_random_sidb_layout(params, sidb_cell_level_layout())
    assert result_lyt is None


def test_impossible_design_of_multiple_layouts():
    params = generate_random_sidb_layout_params()
    params.maximal_attempts_for_multiple_layouts = 5
    params.number_of_sidbs = 2
    result_lyt = generate_multiple_random_sidb_layouts(params, sidb_cell_level_layout())
    assert result_lyt is None
