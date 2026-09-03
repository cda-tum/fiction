# Copyright (c) 2018 - 2023 Marcel Walter
# Copyright (c) 2023 - present Chair for Design Automation, Technical University of Munich
# All rights reserved.
#
# SPDX-License-Identifier: MIT
#
# Licensed under the MIT License

from __future__ import annotations

import pytest

from mnt.pyfiction import (
    charge_distribution,
    lattice,
    lattice_site,
    parameter_point,
    physically_valid_parameters,
    sidb_layout,
    sidb_technology,
)


def test_one_sidb_100_lattice():
    layout = sidb_layout()
    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)

    valid_parameters = physically_valid_parameters(layout, charge_distribution(layout))

    assert valid_parameters.get_excited_state_number_for_parameter(parameter_point([5, 5])) == 0

    assert valid_parameters.get_excited_state_number_for_parameter(parameter_point([5.1, 5.1])) == 0

    # Testing for an invalid parameter point that raises an exception
    with pytest.raises(ValueError, match="no excited state number available"):
        valid_parameters.get_excited_state_number_for_parameter(parameter_point([15, 15]))


def test_one_sidb_111_lattice():
    layout = sidb_layout(lattice.si_111_1x1())
    layout.assign_cell_type(lattice_site(0, 0, 0), sidb_technology.cell_type.NORMAL)

    valid_parameters = physically_valid_parameters(layout, charge_distribution(layout))

    assert valid_parameters.get_excited_state_number_for_parameter(parameter_point([5, 5])) == 0

    assert valid_parameters.get_excited_state_number_for_parameter(parameter_point([5.1, 5.1])) == 0

    # Testing for an invalid parameter point that raises an exception
    with pytest.raises(ValueError, match="no excited state number available"):
        valid_parameters.get_excited_state_number_for_parameter(parameter_point([15, 15]))
