from mnt.pyfiction import (sidb_100_lattice, sidb_111_lattice, sidb_technology, sidb_simulation_result_100,
                           charge_distribution_surface_100, check_simulation_results_for_equivalence,
                           sidb_simulation_result_111, charge_distribution_surface_111)
import unittest


class TestCheckSimulationResultsForEquivalence(unittest.TestCase):

    def test_cds_with_two_sidbs_100_lattice(self):
        # Use standard constructor.
        lattice_100 = sidb_100_lattice()
        lattice_100.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        lattice_100.assign_cell_type((0, 3), sidb_technology.cell_type.NORMAL)

        first_result_100 = sidb_simulation_result_100()
        second_result_100 = sidb_simulation_result_100()

        first_result_100.charge_distributions = [charge_distribution_surface_100(lattice_100)]
        second_result_100.charge_distributions = [charge_distribution_surface_100(lattice_100)]

        equivalent = check_simulation_results_for_equivalence(first_result_100, second_result_100)

        self.assertTrue(equivalent)

    def test_cds_with_two_sidbs_111_lattice(self):
        # Use standard constructor.
        lattice_111 = sidb_111_lattice()
        lattice_111.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        lattice_111.assign_cell_type((0, 3), sidb_technology.cell_type.NORMAL)

        first_result_111 = sidb_simulation_result_111()
        second_result_111 = sidb_simulation_result_111()

        first_result_111.charge_distributions = [charge_distribution_surface_111(lattice_111)]
        second_result_111.charge_distributions = [charge_distribution_surface_111(lattice_111)]

        equivalent = check_simulation_results_for_equivalence(first_result_111, second_result_111)

        self.assertTrue(equivalent)


if __name__ == '__main__':
    unittest.main()
