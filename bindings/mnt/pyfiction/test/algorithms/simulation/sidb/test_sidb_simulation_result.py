import unittest

from mnt.pyfiction import (
    aspect_ratio_offset,
    charge_distribution_surface_100,
    charge_distribution_surface_111,
    groundstate_from_simulation_result,
    sidb_100_lattice,
    sidb_111_lattice,
    sidb_charge_state,
    sidb_simulation_parameters,
    sidb_simulation_result_100,
    sidb_simulation_result_111,
    sidb_technology,
)


class TestSiDBSimulationResult(unittest.TestCase):
    def test_negative_and_neutral_layout_100_lattice(self):
        # Use standard constructor.
        result = sidb_simulation_result_100()

        layout = sidb_100_lattice(aspect_ratio_offset((2, 3)))
        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((0, 3), sidb_technology.cell_type.NORMAL)

        cds_negative = charge_distribution_surface_100(layout)

        cds_neutral = charge_distribution_surface_100(layout, sidb_simulation_parameters(), sidb_charge_state.NEUTRAL)

        result.charge_distributions = [cds_negative, cds_neutral]

        groundstate = groundstate_from_simulation_result(result)

        self.assertEqual(len(groundstate), 1)
        self.assertEqual(groundstate[0].get_charge_state((0, 1)), sidb_charge_state.NEUTRAL)
        self.assertEqual(groundstate[0].get_charge_state((0, 3)), sidb_charge_state.NEUTRAL)

    def test_negative_and_neutral_layout_111_lattice(self):
        # Use standard constructor.
        result = sidb_simulation_result_111()

        layout = sidb_111_lattice(aspect_ratio_offset((2, 3)))
        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((0, 3), sidb_technology.cell_type.NORMAL)

        cds_negative = charge_distribution_surface_111(layout)

        cds_neutral = charge_distribution_surface_111(layout, sidb_simulation_parameters(), sidb_charge_state.NEUTRAL)

        result.charge_distributions = [cds_negative, cds_neutral]

        groundstate = groundstate_from_simulation_result(result)

        self.assertEqual(len(groundstate), 1)
        self.assertEqual(groundstate[0].get_charge_state((0, 1)), sidb_charge_state.NEUTRAL)
        self.assertEqual(groundstate[0].get_charge_state((0, 3)), sidb_charge_state.NEUTRAL)


if __name__ == "__main__":
    unittest.main()
