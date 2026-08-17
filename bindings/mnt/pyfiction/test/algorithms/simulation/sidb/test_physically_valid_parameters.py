from __future__ import annotations

import pytest

from mnt.pyfiction import (
    charge_distribution_surface_100,
    charge_distribution_surface_111,
    parameter_point,
    physically_valid_parameters,
    sidb_100_lattice,
    sidb_111_lattice,
    sidb_technology,
)


def test_one_sidb_100_lattice():
    layout = sidb_100_lattice((10, 10))
    layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
    cds = charge_distribution_surface_100(layout)

    valid_parameters = physically_valid_parameters(cds)

    assert valid_parameters.get_excited_state_number_for_parameter(parameter_point([5, 5])) == 0

    assert valid_parameters.get_excited_state_number_for_parameter(parameter_point([5.1, 5.1])) == 0

    # Testing for an invalid parameter point that raises an exception
    with pytest.raises(ValueError, match="no excited state number available"):
        valid_parameters.get_excited_state_number_for_parameter(parameter_point([15, 15]))


def test_one_sidb_111_lattice():
    layout = sidb_111_lattice((10, 10))
    layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
    cds = charge_distribution_surface_111(layout)

    valid_parameters = physically_valid_parameters(cds)

    assert valid_parameters.get_excited_state_number_for_parameter(parameter_point([5, 5])) == 0

    assert valid_parameters.get_excited_state_number_for_parameter(parameter_point([5.1, 5.1])) == 0

    # Testing for an invalid parameter point that raises an exception
    with pytest.raises(ValueError, match="no excited state number available"):
        valid_parameters.get_excited_state_number_for_parameter(parameter_point([15, 15]))
