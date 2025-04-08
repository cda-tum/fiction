import unittest

from mnt.pyfiction import (
    charge_distribution_surface,
    offset_coordinate,
    sidb_charge_state,
    sidb_layout,
    sidb_technology,
)


class TestCalculateEnergyAndStateType(unittest.TestCase):
    def test_calculate_energy_and_state_type(self):
        layout_one = sidb_layout((10, 10))
        layout_one.assign_cell_type(offset_coordinate(0, 0), sidb_technology.cell_type.NORMAL)
        layout_one.assign_cell_type(offset_coordinate(1, 2), sidb_technology.cell_type.NORMAL)
        layout_one.assign_cell_type(offset_coordinate(5, 2), sidb_technology.cell_type.NORMAL)

        charge_lyt_one = charge_distribution_surface(layout_one)
        charge_lyt_one.assign_charge_state(offset_coordinate(0, 0), sidb_charge_state.NEUTRAL)
        charge_lyt_one.assign_charge_state(offset_coordinate(1, 2), sidb_charge_state.NEGATIVE)
        charge_lyt_one.assign_charge_state(offset_coordinate(5, 2), sidb_charge_state.NEGATIVE)
        charge_lyt_one.update_after_charge_change()

        layout_two = sidb_layout((10, 10))
        layout_two.assign_cell_type(offset_coordinate(0, 0), sidb_technology.cell_type.NORMAL)
        layout_two.assign_cell_type(offset_coordinate(1, 2), sidb_technology.cell_type.NORMAL)
        layout_two.assign_cell_type(offset_coordinate(5, 2), sidb_technology.cell_type.NORMAL)

        charge_lyt_two = charge_distribution_surface(layout_two)
        charge_lyt_two.assign_charge_state((0, 0), sidb_charge_state.NEGATIVE)
        charge_lyt_two.assign_charge_state((1, 2), sidb_charge_state.NEUTRAL)
        charge_lyt_two.assign_charge_state((5, 2), sidb_charge_state.NEGATIVE)
        charge_lyt_two.update_after_charge_change()

        [charge_distribution_surface(), charge_distribution_surface()]

        # TODO Tests has to be added


if __name__ == "__main__":
    unittest.main()
