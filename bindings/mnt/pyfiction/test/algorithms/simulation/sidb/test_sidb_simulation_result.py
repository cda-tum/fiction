import unittest

from mnt.pyfiction import (
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
        """Test ground state calculation for a negative and neutral layout on the 100 lattice."""
        # Initialize the result object.
        result = sidb_simulation_result_100()

        # Create a 2x3 lattice and assign NORMAL cell types.
        layout = sidb_100_lattice((2, 3))
        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((0, 3), sidb_technology.cell_type.NORMAL)

        # Generate charge distributions for negative and neutral states.
        cds_negative = charge_distribution_surface_100(layout)
        cds_neutral = charge_distribution_surface_100(layout, sidb_simulation_parameters(), sidb_charge_state.NEUTRAL)

        # Assign charge distributions to the result.
        result.charge_distributions = [cds_negative, cds_neutral]

        # Calculate the ground state from the simulation result.
        groundstate = groundstate_from_simulation_result(result)

        # Validate ground state results.
        self.assertEqual(len(groundstate), 1, "Expected exactly one ground state.")
        self.assertEqual(
            groundstate[0].get_charge_state((0, 1)),
            sidb_charge_state.NEUTRAL,
            "Cell (0, 1) should have a NEUTRAL charge state.",
        )
        self.assertEqual(
            groundstate[0].get_charge_state((0, 3)),
            sidb_charge_state.NEUTRAL,
            "Cell (0, 3) should have a NEUTRAL charge state.",
        )

    def test_negative_and_neutral_layout_111_lattice(self):
        """Test ground state calculation for a negative and neutral layout on the 111 lattice."""
        # Initialize the result object.
        result = sidb_simulation_result_111()

        # Create a 2x3 lattice and assign NORMAL cell types.
        layout = sidb_111_lattice((2, 3))
        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((0, 3), sidb_technology.cell_type.NORMAL)

        # Generate charge distributions for negative and neutral states.
        cds_negative = charge_distribution_surface_111(layout)
        cds_neutral = charge_distribution_surface_111(layout, sidb_simulation_parameters(), sidb_charge_state.NEUTRAL)

        # Assign charge distributions to the result.
        result.charge_distributions = [cds_negative, cds_neutral]

        # Calculate the ground state from the simulation result.
        groundstate = groundstate_from_simulation_result(result)

        # Validate ground state results.
        self.assertEqual(len(groundstate), 1, "Expected exactly one ground state.")
        self.assertEqual(
            groundstate[0].get_charge_state((0, 1)),
            sidb_charge_state.NEUTRAL,
            "Cell (0, 1) should have a NEUTRAL charge state.",
        )
        self.assertEqual(
            groundstate[0].get_charge_state((0, 3)),
            sidb_charge_state.NEUTRAL,
            "Cell (0, 3) should have a NEUTRAL charge state.",
        )

    def test_empty_layout_100_lattice(self):
        """Test handling of an empty layout on the 100 lattice."""
        result = sidb_simulation_result_100()

        layout = sidb_100_lattice((0, 0))  # Empty layout

        # Attempting to generate charge distributions should not throw errors.
        cds_empty = charge_distribution_surface_100(layout)

        # Assign the empty distribution and check ground state handling.
        result.charge_distributions = [cds_empty]
        groundstate = groundstate_from_simulation_result(result)

        self.assertEqual(len(groundstate), 1)

    def test_empty_layout_111_lattice(self):
        """Test handling of an empty layout on the 111 lattice."""
        result = sidb_simulation_result_111()

        layout = sidb_111_lattice((0, 0))  # Empty layout

        # Attempting to generate charge distributions should not throw errors.
        cds_empty = charge_distribution_surface_111(layout)

        # Assign the empty distribution and check ground state handling.
        result.charge_distributions = [cds_empty]
        groundstate = groundstate_from_simulation_result(result)

        self.assertEqual(len(groundstate), 1)


if __name__ == "__main__":
    unittest.main()
