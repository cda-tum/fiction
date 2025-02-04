import unittest

from mnt.pyfiction import (
    aspect_ratio_offset,
    charge_distribution_surface_100,
    charge_distribution_surface_111,
    parameter_point,
    physically_valid_parameters,
    sidb_100_lattice,
    sidb_111_lattice,
    sidb_technology,
)


class TestDeterminePhysicallyValidParameters(unittest.TestCase):
    def test_one_DB_100_lattice(self):
        layout = sidb_100_lattice(aspect_ratio_offset((10, 10)))
        layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
        cds = charge_distribution_surface_100(layout)

        valid_parameters = physically_valid_parameters(cds)

        self.assertEqual(valid_parameters.get_excited_state_number_for_parameter(parameter_point([5, 5])), 0)

        self.assertEqual(valid_parameters.get_excited_state_number_for_parameter(parameter_point([5.1, 5.1])), 0)

        # Testing for an invalid parameter point that raises an exception
        with self.assertRaises(ValueError):
            valid_parameters.get_excited_state_number_for_parameter(parameter_point([15, 15]))

    def test_one_DB_111_lattice(self):
        layout = sidb_111_lattice(aspect_ratio_offset((10, 10)))
        layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
        cds = charge_distribution_surface_111(layout)

        valid_parameters = physically_valid_parameters(cds)

        self.assertEqual(valid_parameters.get_excited_state_number_for_parameter(parameter_point([5, 5])), 0)

        self.assertEqual(valid_parameters.get_excited_state_number_for_parameter(parameter_point([5.1, 5.1])), 0)

        # Testing for an invalid parameter point that raises an exception
        with self.assertRaises(ValueError):
            valid_parameters.get_excited_state_number_for_parameter(parameter_point([15, 15]))


if __name__ == "__main__":
    unittest.main()
