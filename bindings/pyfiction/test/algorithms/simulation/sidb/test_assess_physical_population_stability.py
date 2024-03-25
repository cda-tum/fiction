from mnt.pyfiction import *
import unittest
import os


class TestAssessPhysicalPopulationStability(unittest.TestCase):

    def test_three_DBs(self):
        layout = sidb_layout((2, 3))
        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((0, 3), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((1, 1), sidb_technology.cell_type.NORMAL)
        params = assess_physical_population_stability_params()
        params.simulation_parameters.mu_minus = -0.25
        cds = charge_distribution_surface(layout)
        result = assess_physical_population_stability(cds, params)
        self.assertEqual(len(result), 5)
        self.assertLessEqual(result[0].system_energy, result[1].system_energy)
        self.assertLessEqual(result[1].system_energy, result[2].system_energy)

        params.simulation_parameters.mu_minus = -0.32
        result = assess_physical_population_stability(cds, params)
        self.assertEqual(len(result), 1)

if __name__ == '__main__':
    unittest.main()
