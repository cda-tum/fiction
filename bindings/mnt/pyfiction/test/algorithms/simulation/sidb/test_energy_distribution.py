import unittest

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


class TestEnergyDistribution(unittest.TestCase):
    def test_three_DBs_100_lattice(self):
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

        self.assertAlmostEqual(cds1.get_electrostatic_potential_energy(), 0.48066663155586997)
        self.assertAlmostEqual(cds2.get_electrostatic_potential_energy(), 0.0)
        self.assertAlmostEqual(cds3.get_electrostatic_potential_energy(), 0.48066663155586997)
        self.assertAlmostEqual(cds4.get_electrostatic_potential_energy(), 0.48066663155586997)

        distribution = calculate_energy_distribution(charge_layouts)

        self.assertAlmostEqual(distribution.get_nth_state(0).electrostatic_potential_energy, 0.0)
        self.assertAlmostEqual(distribution.get_nth_state(1).electrostatic_potential_energy, 0.48066663155586997)
        self.assertLessEqual(distribution.get_nth_state(0).degeneracy, 1)
        self.assertLessEqual(distribution.get_nth_state(1).degeneracy, 3)

    def test_three_DBs_111_lattice(self):
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

        self.assertAlmostEqual(cds1.get_electrostatic_potential_energy(), 0.233980661373219)
        self.assertAlmostEqual(cds2.get_electrostatic_potential_energy(), 0.0)
        self.assertAlmostEqual(cds3.get_electrostatic_potential_energy(), 0.233980661373219)
        self.assertAlmostEqual(cds4.get_electrostatic_potential_energy(), 0.233980661373219)

        distribution = calculate_energy_distribution(charge_layouts)

        self.assertLessEqual(distribution.get_nth_state(0).degeneracy, 1)
        self.assertLessEqual(distribution.get_nth_state(1).degeneracy, 3)


if __name__ == "__main__":
    unittest.main()
