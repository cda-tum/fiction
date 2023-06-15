from fiction.pyfiction import *
import unittest
import os


class CalculateEnergyAndStateTypeUnitTest(unittest.TestCase):

    def test_calculate_energy_and_state_type(self):
        energy_distribution = [(0.1, True), (0.3, False), (0.5, True)]
        layout_one = sidb_layout((4, 4))
        layout_one.assign_cell_type((0, 2), sidb_technology.cell_type.NORMAL)
        layout_one.assign_cell_type((2, 4), sidb_technology.cell_type.INPUT)

        layout_two = sidb_layout((4, 4))
        layout_two.assign_cell_type((0, 2), sidb_technology.cell_type.NORMAL)
        layout_two.assign_cell_type((4, 4), sidb_technology.cell_type.INPUT)

        charge_lyt_one = charge_distribution_surface()
        charge_lyt_one.assign_charge_state((0, 0), sidb_charge_state.NEGATIVE)
        charge_lyt_one.assign_charge_state((3, 2), sidb_charge_state.NEGATIVE)
        charge_lyt_one.assign_charge_state((10, 2), sidb_charge_state.NEUTRAL)
        charge_state = charge_lyt_one.get_charge_state((10, 2, 1))
        charge_lyt_one.update_after_charge_change()
        size = charge_lyt_one.x
        charge_index = charge_lyt_one.get_charge_index()
        energy = charge_lyt_one.get_system_energy()

        charge_lyt_second = charge_distribution_surface()
        charge_lyt_second.assign_charge_state(siqad_coordinate(1, 5, 1), sidb_charge_state.NEGATIVE)
        charge_lyt_second.assign_charge_state(siqad_coordinate(1, 2, 1), sidb_charge_state.NEGATIVE)
        charge_lyt_one.assign_charge_state(siqad_coordinate(10, 2, 1), sidb_charge_state.NEUTRAL)
        charge_lyt_second.update_after_charge_change()

        valid_lyts = [charge_lyt_one, charge_lyt_second]

        output_cells = [siqad_coordinate(10, 2, 1)]
        output_bits = [False]

    #     params = sidb_simulation_parameters()
    #
    #     # Check if it is initialized correctly.
    #     self.assertEqual(params.lat_a, 3.84)
    #     self.assertEqual(params.lat_b, 7.68)
    #     self.assertEqual(params.lat_c, 2.25)
    #     self.assertEqual(params.epsilon_r, 5.6)
    #     self.assertEqual(params.lambda_tf, 5)
    #     self.assertEqual(params.mu, -0.32)
    #     self.assertEqual(params.base, 3)
    #
    # def test_custom_initialization(self):
    #     params = sidb_simulation_parameters(
    #         2, -0.4, 7.1, 10.0, 6.84, 7.68, 2.8)
    #
    #     # Check if it is initialized correctly.
    #     self.assertEqual(params.lat_a, 6.84)
    #     self.assertEqual(params.lat_b, 7.68)
    #     self.assertEqual(params.lat_c, 2.8)
    #     self.assertEqual(params.epsilon_r, 7.1)
    #     self.assertEqual(params.lambda_tf, 10.0)
    #     self.assertEqual(params.mu, -0.4)
    #     self.assertEqual(params.base, 2)


if __name__ == '__main__':
    unittest.main()
