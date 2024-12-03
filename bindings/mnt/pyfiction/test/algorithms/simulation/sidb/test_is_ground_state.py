import unittest

from mnt.pyfiction import (
    sidb_100_lattice,
    sidb_technology,
    charge_distribution_surface_100,
    sidb_simulation_parameters,
    sidb_charge_state,
    sidb_simulation_result_100,
    is_ground_state,
    sidb_111_lattice,
    charge_distribution_surface_111,
    sidb_simulation_result_111,
)


class TestIsGroundState(unittest.TestCase):
    def test_three_DBs_100_lattice(self):
        layout = sidb_100_lattice((10, 10))

        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((4, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((6, 1), sidb_technology.cell_type.NORMAL)

        cds1 = charge_distribution_surface_100(layout)  # all negative
        cds2 = charge_distribution_surface_100(
            layout, sidb_simulation_parameters(), sidb_charge_state.NEUTRAL
        )  # all neutral
        cds3 = charge_distribution_surface_100(layout)
        cds3.assign_charge_state((6, 1), sidb_charge_state.NEUTRAL)  # only two SiDBs are negative
        cds3.update_after_charge_change()

        cds4 = charge_distribution_surface_100(layout)
        cds4.assign_charge_state((0, 1), sidb_charge_state.NEUTRAL)
        cds4.assign_charge_state((4, 1), sidb_charge_state.POSITIVE)
        cds4.assign_charge_state((6, 1), sidb_charge_state.NEGATIVE)
        cds4.update_after_charge_change()

        exact_results = sidb_simulation_result_100()
        exact_results.charge_distributions = [cds1, cds2, cds3, cds4]

        heuristic_results = sidb_simulation_result_100()
        heuristic_results.charge_distributions = [cds4]

        self.assertTrue(is_ground_state(heuristic_results, exact_results))

    def test_three_DBs_111_lattice(self):
        layout = sidb_111_lattice((10, 10))

        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((4, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((6, 1), sidb_technology.cell_type.NORMAL)

        cds1 = charge_distribution_surface_111(layout)  # all negative
        cds2 = charge_distribution_surface_111(
            layout, sidb_simulation_parameters(), sidb_charge_state.NEUTRAL
        )  # all neutral
        cds3 = charge_distribution_surface_111(layout)
        cds3.assign_charge_state((6, 1), sidb_charge_state.NEUTRAL)  # only two SiDBs are negative
        cds3.update_after_charge_change()

        cds4 = charge_distribution_surface_111(layout)
        cds4.assign_charge_state((0, 1), sidb_charge_state.NEUTRAL)
        cds4.assign_charge_state((4, 1), sidb_charge_state.POSITIVE)
        cds4.assign_charge_state((6, 1), sidb_charge_state.NEGATIVE)
        cds4.update_after_charge_change()

        exact_results = sidb_simulation_result_111()
        exact_results.charge_distributions = [cds1, cds2, cds3, cds4]

        heuristic_results = sidb_simulation_result_111()
        heuristic_results.charge_distributions = [cds4]

        self.assertTrue(is_ground_state(heuristic_results, exact_results))


if __name__ == "__main__":
    unittest.main()
