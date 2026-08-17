import unittest

from mnt.pyfiction import (
    physical_population_stability_100,
    physical_population_stability_111,
    physical_population_stability_params,
    sidb_100_lattice,
    sidb_111_lattice,
    sidb_technology,
)


class TestAssessPhysicalPopulationStability(unittest.TestCase):
    def test_three_DBs_100_lattice(self):
        layout = sidb_100_lattice((2, 3))
        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((0, 3), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((1, 1), sidb_technology.cell_type.NORMAL)
        params = physical_population_stability_params()
        params.simulation_parameters.mu_minus = -0.25
        result = physical_population_stability_100(layout, params)
        assert len(result) == 5
        assert result[0].system_energy <= result[1].system_energy
        assert result[1].system_energy <= result[2].system_energy

        params.simulation_parameters.mu_minus = -0.32
        result = physical_population_stability_100(layout, params)
        assert len(result) == 1

    def test_three_DBs_111_lattice(self):
        layout = sidb_111_lattice((2, 3))
        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((0, 3), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((1, 1), sidb_technology.cell_type.NORMAL)
        params = physical_population_stability_params()
        params.simulation_parameters.mu_minus = -0.25
        result = physical_population_stability_111(layout, params)
        assert len(result) == 5
        assert result[0].system_energy <= result[1].system_energy
        assert result[1].system_energy <= result[2].system_energy

        params.simulation_parameters.mu_minus = -0.32
        result = physical_population_stability_111(layout, params)
        assert len(result) == 2


if __name__ == "__main__":
    unittest.main()
