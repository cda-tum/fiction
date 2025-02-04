import unittest

from mnt.pyfiction import (
    charge_distribution_surface_100,
    charge_distribution_surface_111,
    minimum_energy,
    sidb_100_lattice,
    sidb_111_lattice,
    sidb_charge_state,
    sidb_simulation_parameters,
    sidb_technology,
    aspect_ratio_offset
)


class TestMinimumEnergy(unittest.TestCase):
    def test_three_DBs_100_lattice(self):
        layout = sidb_100_lattice(aspect_ratio_offset((10, 10)))

        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((4, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((6, 1), sidb_technology.cell_type.NORMAL)

        cds1 = charge_distribution_surface_100(layout)  # all negative
        cds2 = charge_distribution_surface_100(
            layout, sidb_simulation_parameters(), sidb_charge_state.NEUTRAL
        )  # all neutral
        cds3 = charge_distribution_surface_100(layout)
        cds3.assign_charge_state((6, 1), sidb_charge_state.NEUTRAL)  # only two SiDBs are negative

        cds4 = charge_distribution_surface_100(layout)
        cds4.assign_charge_state((0, 1), sidb_charge_state.NEUTRAL)
        cds4.assign_charge_state((4, 1), sidb_charge_state.POSITIVE)
        cds4.assign_charge_state((6, 1), sidb_charge_state.NEGATIVE)

        result = minimum_energy([cds1, cds2, cds3, cds4])
        self.assertLessEqual(result, 0)

    def test_three_DBs_111_lattice(self):
        layout = sidb_111_lattice(aspect_ratio_offset((10, 10)))

        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((4, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((6, 1), sidb_technology.cell_type.NORMAL)

        cds1 = charge_distribution_surface_111(layout)  # all negative
        cds2 = charge_distribution_surface_111(
            layout, sidb_simulation_parameters(), sidb_charge_state.NEUTRAL
        )  # all neutral
        cds3 = charge_distribution_surface_111(layout)
        cds3.assign_charge_state((6, 1), sidb_charge_state.NEUTRAL)  # only two SiDBs are negative

        cds4 = charge_distribution_surface_111(layout)
        cds4.assign_charge_state((0, 1), sidb_charge_state.NEUTRAL)
        cds4.assign_charge_state((4, 1), sidb_charge_state.POSITIVE)
        cds4.assign_charge_state((6, 1), sidb_charge_state.NEGATIVE)

        result = minimum_energy([cds1, cds2, cds3, cds4])
        self.assertLessEqual(result, 0)


if __name__ == "__main__":
    unittest.main()
