import unittest

from mnt.pyfiction import (
    charge_distribution_surface_100,
    charge_distribution_surface_111,
    determine_groundstate_from_simulation_results,
    sidb_100_lattice,
    sidb_111_lattice,
    sidb_charge_state,
    sidb_simulation_parameters,
    sidb_simulation_result_100,
    sidb_simulation_result_111,
    sidb_technology,
)


class TestDetermineGroundstateFromSimulationResults(unittest.TestCase):
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

        results = sidb_simulation_result_100()
        results.charge_distributions = [cds1, cds2, cds3]

        result = determine_groundstate_from_simulation_results(results)
        self.assertEqual(len(result), 1)

        groundstate = result[0]
        self.assertEqual(groundstate.get_charge_state((0, 1)), sidb_charge_state.NEUTRAL)
        self.assertEqual(groundstate.get_charge_state((4, 1)), sidb_charge_state.NEUTRAL)
        self.assertEqual(groundstate.get_charge_state((6, 1)), sidb_charge_state.NEUTRAL)

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

        results = sidb_simulation_result_111()
        results.charge_distributions = [cds1, cds2, cds3]

        result = determine_groundstate_from_simulation_results(results)
        self.assertEqual(len(result), 1)

        groundstate = result[0]
        self.assertEqual(groundstate.get_charge_state((0, 1)), sidb_charge_state.NEUTRAL)
        self.assertEqual(groundstate.get_charge_state((4, 1)), sidb_charge_state.NEUTRAL)
        self.assertEqual(groundstate.get_charge_state((6, 1)), sidb_charge_state.NEUTRAL)


if __name__ == "__main__":
    unittest.main()
