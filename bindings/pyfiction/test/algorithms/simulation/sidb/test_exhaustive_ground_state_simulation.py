from mnt.pyfiction import *
import unittest
import os


class TestExhaustiveGroundStateSimulation(unittest.TestCase):

    def test_perturber_and_DB_pair(self):
        layout = sidb_layout((10, 10))
        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((4, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((6, 1), sidb_technology.cell_type.NORMAL)

        params = sidb_simulation_parameters()

        cds = charge_distribution_surface(layout)

        result = exhaustive_ground_state_simulation(cds, params)

        self.assertEqual(result.algorithm_name, "ExGS")
        self.assertEqual(len(result.charge_distributions), 1)

        groundstate = result.charge_distributions[0]

        self.assertEqual(groundstate.get_charge_state((0, 1)), sidb_charge_state.NEGATIVE)
        self.assertEqual(groundstate.get_charge_state((4, 1)), sidb_charge_state.NEUTRAL)
        self.assertEqual(groundstate.get_charge_state((6, 1)), sidb_charge_state.NEGATIVE)


if __name__ == '__main__':
    unittest.main()
