import os
import unittest

from mnt.pyfiction import *

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestDefectOperationalDomain(unittest.TestCase):

    def test_xor_gate_100_lattice(self):
        lyt = read_sqd_layout_100_cube(dir_path + "/../../../resources/hex_21_inputsdbp_xor_v1.sqd")

        params = defect_operational_domain_params()
        params.operational_params.simulation_parameters.base = 2
        params.operational_params.simulation_parameters.mu_minus = -0.32
        params.defect_influence_params.additional_scanning_area = (50, 50)

        params.defect_influence_params.defect = sidb_defect(sidb_defect_type.SI_VACANCY, -1, 10.6, 5.9)

        stats_grid = defect_operational_domain_stats()
        _ = defect_operational_domain_grid_search(lyt, [create_xor_tt()], 6, params, stats_grid)
        self.assertGreater(stats_grid.num_operational_defect_positions, 0)

        stats_random = defect_operational_domain_stats()
        _ = defect_operational_domain_random_sampling(lyt, [create_xor_tt()], 100, params, stats_random)
        self.assertGreater(stats_random.num_operational_defect_positions, 0)

        stats_quicktrace = defect_operational_domain_stats()
        _ = defect_operational_domain_quicktrace(lyt, [create_xor_tt()], 3, params, stats_quicktrace)
        self.assertGreater(stats_quicktrace.num_operational_defect_positions, 0)

if __name__ == '__main__':
    unittest.main()
