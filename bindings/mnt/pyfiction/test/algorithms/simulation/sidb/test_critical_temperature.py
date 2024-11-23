import os
import unittest

from mnt.pyfiction import *

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestCriticalTemperature(unittest.TestCase):

    def test_perturber_and_DB_pair_100(self):
        layout = sidb_100_lattice((10, 10))
        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((4, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((6, 1), sidb_technology.cell_type.NORMAL)

        params = critical_temperature_params()

        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT

        stats = critical_temperature_stats()

        cds = charge_distribution_surface_100(layout)

        self.assertEqual(critical_temperature_non_gate_based(cds, params, stats), 400)

        self.assertEqual(stats.algorithm_name, "QuickExact")
        self.assertEqual(stats.num_valid_lyt, 1)

    def test_perturber_and_DB_pair_111(self):
        layout = sidb_111_lattice((10, 10))
        layout.assign_cell_type((0, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((4, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((6, 1), sidb_technology.cell_type.NORMAL)

        params = critical_temperature_params()

        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT

        stats = critical_temperature_stats()

        cds = charge_distribution_surface_111(layout)

        self.assertEqual(critical_temperature_non_gate_based(cds, params, stats), 400)

        self.assertEqual(stats.algorithm_name, "QuickExact")
        self.assertEqual(stats.num_valid_lyt, 1)

    def test_gate_based_simulation(self):
        layout = read_sqd_layout_100(dir_path + "/../../../resources/hex_21_inputsdbp_xor_v1.sqd", "xor_gate")
        params = critical_temperature_params()

        params.operational_params.simulation_parameters.base = 2

        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT

        stats = critical_temperature_stats()

        cds = charge_distribution_surface_100(layout)
        spec = [create_xor_tt()]

        self.assertLessEqual(critical_temperature_gate_based(cds, spec, params, stats), 200)

        self.assertEqual(stats.algorithm_name, "QuickExact")

    def test_bestagon_inv(self):
        layout = read_sqd_layout_100(dir_path + "/../../../resources/hex_11_inputsdbp_inv_straight_v0_manual.sqd",
                                     "inverter_input_0")

        params = critical_temperature_params()

        params.operational_params.sim_engine = sidb_simulation_engine.QUICKSIM

        stats = critical_temperature_stats()

        cds = charge_distribution_surface_100(layout)
        spec = [create_not_tt()]

        self.assertLessEqual(critical_temperature_gate_based(cds, spec, params, stats), 400)

        self.assertEqual(stats.algorithm_name, "QuickSim")
        self.assertGreater(stats.num_valid_lyt, 1)

    def test_bestagon_inv_with_different_mu(self):
        layout = read_sqd_layout_100(dir_path + "/../../../resources/hex_11_inputsdbp_inv_straight_v0_manual.sqd",
                                     "inverter_input_0")

        params = critical_temperature_params()
        params.operational_params.simulation_parameters.base = 2
        params.operational_params.simulation_parameters.mu_minus = -0.2

        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT

        stats = critical_temperature_stats()

        cds = charge_distribution_surface_100(layout)
        spec = [create_not_tt()]

        self.assertLessEqual(critical_temperature_gate_based(cds, spec, params, stats), 0)

        self.assertEqual(stats.algorithm_name, "QuickExact")


if __name__ == '__main__':
    unittest.main()
