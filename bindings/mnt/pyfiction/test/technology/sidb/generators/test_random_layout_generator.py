# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

from mnt.pyfiction import (
    generate_multiple_random_sidb_layouts,
    generate_random_sidb_layout,
    generate_random_sidb_layout_params,
    lattice,
    lattice_site,
    sidb_layout,
)


def test_area_with_one_sidb_100_lattice() -> None:
    """A one-site H-Si(100)-2x1 area fixes the generated SiDB position."""
    params = generate_random_sidb_layout_params()
    params.number_of_sidbs = 1
    params.coordinate_pair = (lattice_site(10, 5, 0), lattice_site(10, 5, 0))
    result_lyt = generate_random_sidb_layout(params, sidb_layout())
    assert result_lyt.num_cells() == 1
    assert result_lyt.sidbs() == [lattice_site(10, 5, 0)]


def test_area_with_five_sidb_100_lattice() -> None:
    """The generator places five SiDBs on an H-Si(100)-2x1 lattice."""
    params = generate_random_sidb_layout_params()
    params.number_of_sidbs = 5
    params.coordinate_pair = (lattice_site(0, 0, 0), lattice_site(10, 5, 0))
    result_lyt = generate_random_sidb_layout(params, sidb_layout())
    assert result_lyt.num_cells() == 5


def test_area_with_one_coordinate_111_lattice() -> None:
    """A one-site H-Si(111)-1x1 area fixes the generated SiDB position."""
    params = generate_random_sidb_layout_params()
    params.number_of_sidbs = 1
    params.coordinate_pair = (lattice_site(10, 5, 0), lattice_site(10, 5, 0))
    result_lyt = generate_random_sidb_layout(params, sidb_layout(lattice.si_111_1x1()))
    assert result_lyt.num_cells() == 1
    assert result_lyt.sidbs() == [lattice_site(10, 5, 0)]


def test_impossible_design_of_single_layout() -> None:
    """The generator returns None when the requested SiDBs do not fit."""
    params = generate_random_sidb_layout_params()
    params.number_of_sidbs = 2
    result_lyt = generate_random_sidb_layout(params, sidb_layout())
    assert result_lyt is None


def test_impossible_design_of_multiple_layouts() -> None:
    """Repeated generation returns None when the requested SiDBs do not fit."""
    params = generate_random_sidb_layout_params()
    params.maximal_attempts_for_multiple_layouts = 5
    params.number_of_sidbs = 2
    result_lyt = generate_multiple_random_sidb_layouts(params, sidb_layout())
    assert result_lyt is None
