import unittest

from mnt.pyfiction import (sidb_100_lattice, sidb_technology, assess_physical_population_stability_params,
                           assess_physical_population_stability_100, assess_physical_population_stability_111, sidb_111_lattice, population_stability_information_100, population_stability_information_111)

class TestAssessPhysicalPopulationStability(unittest.TestCase):

    def test_three_DBs_100_lattice(self):
        layout = sidb_100_lattice((2, 3))
        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((0, 3), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((1, 1), sidb_technology.cell_type.NORMAL)
        params = assess_physical_population_stability_params()
        params.simulation_parameters.mu_minus = -0.25
        cds = charge_distribution_surface_100(layout)
        result = assess_physical_population_stability_100(layout, params)
        self.assertEqual(len(result), 5)
        self.assertLessEqual(result[0].system_energy, result[1].system_energy)
        self.assertLessEqual(result[1].system_energy, result[2].system_energy)

        params.simulation_parameters.mu_minus = -0.32
        result = assess_physical_population_stability_100(cds, params)
        self.assertEqual(len(result), 1)

    def test_three_DBs_111_lattice(self):
        layout = sidb_111_lattice((2, 3))
        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((0, 3), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((1, 1), sidb_technology.cell_type.NORMAL)
        params = assess_physical_population_stability_params()
        params.simulation_parameters.mu_minus = -0.25
        cds = charge_distribution_surface_111(layout)
        result = assess_physical_population_stability_111(layout, params)
        self.assertEqual(len(result), 5)
        self.assertLessEqual(result[0].system_energy, result[1].system_energy)
        self.assertLessEqual(result[1].system_energy, result[2].system_energy)

        params.simulation_parameters.mu_minus = -0.32
        result = assess_physical_population_stability_111(cds, params)
        self.assertEqual(len(result), 2)

if __name__ == '__main__':
    unittest.main()
