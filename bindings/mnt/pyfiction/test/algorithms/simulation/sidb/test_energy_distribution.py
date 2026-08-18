import pytest

from mnt.pyfiction import (
    calculate_energy_distribution,
    charge_distribution_surface_100,
    charge_distribution_surface_111,
    sidb_100_lattice,
    sidb_111_lattice,
    sidb_charge_state,
    sidb_simulation_parameters,
    sidb_technology,
)


def test_three_sidbs_100_lattice():
    layout = sidb_100_lattice((10, 10))

    layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((4, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((6, 1), sidb_technology.cell_type.NORMAL)

    cds1 = charge_distribution_surface_100(layout)  # all negative
    cds2 = charge_distribution_surface_100(
        layout,
        sidb_simulation_parameters(),
        sidb_charge_state.NEUTRAL,
    )  # all neutral
    cds3 = charge_distribution_surface_100(layout)
    cds3.assign_charge_state((6, 1), sidb_charge_state.NEUTRAL)  # only two SiDBs are negative

    cds4 = charge_distribution_surface_100(layout)
    cds4.assign_charge_state((0, 1), sidb_charge_state.NEUTRAL)
    cds4.assign_charge_state((4, 1), sidb_charge_state.POSITIVE)
    cds4.assign_charge_state((6, 1), sidb_charge_state.NEGATIVE)

    charge_layouts = [cds1, cds2, cds3, cds4]

    assert cds1.get_electrostatic_potential_energy() == pytest.approx(0.48066663155586997, abs=1e-7)
    assert cds2.get_electrostatic_potential_energy() == pytest.approx(0.0, abs=1e-7)
    assert cds3.get_electrostatic_potential_energy() == pytest.approx(0.48066663155586997, abs=1e-7)
    assert cds4.get_electrostatic_potential_energy() == pytest.approx(0.48066663155586997, abs=1e-7)

    distribution = calculate_energy_distribution(charge_layouts)

    assert distribution.get_nth_state(0).electrostatic_potential_energy == pytest.approx(0.0, abs=1e-7)
    assert distribution.get_nth_state(1).electrostatic_potential_energy == pytest.approx(0.48066663155586997, abs=1e-7)
    assert distribution.get_nth_state(0).degeneracy <= 1
    assert distribution.get_nth_state(1).degeneracy <= 3


def test_three_sidbs_111_lattice():
    layout = sidb_111_lattice((10, 10))

    layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((4, 1), sidb_technology.cell_type.NORMAL)
    layout.assign_cell_type((6, 1), sidb_technology.cell_type.NORMAL)

    cds1 = charge_distribution_surface_111(layout)  # all negative
    cds2 = charge_distribution_surface_111(
        layout,
        sidb_simulation_parameters(),
        sidb_charge_state.NEUTRAL,
    )  # all neutral
    cds3 = charge_distribution_surface_111(layout)
    cds3.assign_charge_state((6, 1), sidb_charge_state.NEUTRAL)  # only two SiDBs are negative

    cds4 = charge_distribution_surface_111(layout)
    cds4.assign_charge_state((0, 1), sidb_charge_state.NEUTRAL)
    cds4.assign_charge_state((4, 1), sidb_charge_state.POSITIVE)
    cds4.assign_charge_state((6, 1), sidb_charge_state.NEGATIVE)

    charge_layouts = [cds1, cds2, cds3, cds4]

    assert cds1.get_electrostatic_potential_energy() == pytest.approx(0.233980661373219, abs=1e-7)
    assert cds2.get_electrostatic_potential_energy() == pytest.approx(0.0, abs=1e-7)
    assert cds3.get_electrostatic_potential_energy() == pytest.approx(0.233980661373219, abs=1e-7)
    assert cds4.get_electrostatic_potential_energy() == pytest.approx(0.233980661373219, abs=1e-7)

    distribution = calculate_energy_distribution(charge_layouts)

    assert distribution.get_nth_state(0).degeneracy <= 1
    assert distribution.get_nth_state(1).degeneracy <= 3
