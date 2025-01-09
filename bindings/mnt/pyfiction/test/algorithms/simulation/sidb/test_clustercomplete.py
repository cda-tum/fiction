from mnt.pyfiction import *
from dotenv import load_dotenv
import unittest
import os

dir_path = os.path.dirname(os.path.realpath(__file__))
load_dotenv()
alglib = os.environ.get("alglib", "OFF")


class TestClusterComplete(unittest.TestCase):
    @unittest.skipIf(alglib == "OFF", "ALGLIB not enabled")
    def test_three_sidbs(self):
        layout = sidb_100_lattice((2, 1))
        layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((1, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((2, 0), sidb_technology.cell_type.NORMAL)

        params = clustercomplete_params()
        params.simulation_parameters.base = 2
        params.simulation_parameters.mu_minus = -0.25
        params.validity_witness_partitioning_max_cluster_size_gss = 15
        params.num_overlapping_witnesses_limit_gss = 8
        params.available_threads = 4
        params.report_gss_stats = ground_state_space_reporting.ENABLED
        self.assertEqual(params.simulation_parameters.base, 2)
        self.assertEqual(params.simulation_parameters.mu_minus, -0.25)
        self.assertEqual(params.validity_witness_partitioning_max_cluster_size_gss, 15)
        self.assertEqual(params.num_overlapping_witnesses_limit_gss, 8)
        self.assertEqual(params.available_threads, 4)
        self.assertEqual(params.report_gss_stats, ground_state_space_reporting.ENABLED)

        cds = charge_distribution_surface_100(layout)

        result = clustercomplete(cds, params)

        self.assertEqual(result.algorithm_name, "ClusterComplete")
        self.assertLessEqual(len(result.charge_distributions), 3)

        params.simulation_parameters.base = 3
        self.assertEqual(params.simulation_parameters.base, 3)

        result = clustercomplete(cds, params)
        self.assertLessEqual(len(result.charge_distributions), 4)

        params.simulation_parameters.epsilon_r = 2
        params.simulation_parameters.lambda_tf = 2
        result = clustercomplete(cds, params)
        self.assertLessEqual(len(result.charge_distributions), 2)

    @unittest.skipIf(alglib == "OFF", "ALGLIB not enabled")
    def test_perturber_and_sidb_pair_111(self):
        layout = sidb_111_lattice((4, 1))
        layout.assign_cell_type((0, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((1, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((2, 0), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((3, 0), sidb_technology.cell_type.NORMAL)

        params = clustercomplete_params()
        params.simulation_parameters.base = 2
        params.simulation_parameters.mu_minus = -0.32
        self.assertEqual(params.simulation_parameters.mu_minus, -0.32)

        cds = charge_distribution_surface_111(layout)

        result = clustercomplete(cds, params)

        self.assertEqual(result.algorithm_name, "ClusterComplete")

        groundstate = determine_groundstate_from_simulation_results(result)

        self.assertEqual(len(groundstate), 1)

        self.assertEqual(groundstate[0].get_charge_state((0, 0)), sidb_charge_state.NEGATIVE)
        self.assertEqual(groundstate[0].get_charge_state((1, 0)), sidb_charge_state.NEUTRAL)
        self.assertEqual(groundstate[0].get_charge_state((2, 0)), sidb_charge_state.NEUTRAL)
        self.assertEqual(groundstate[0].get_charge_state((3, 0)), sidb_charge_state.NEGATIVE)


if __name__ == '__main__':
    unittest.main()
