import os
import unittest

from mnt.pyfiction import *

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestOperationalDomain(unittest.TestCase):

    def test_xor_gate_100_lattice(self):
        lyt = read_sqd_layout_100_cube(dir_path + "/../../../resources/hex_21_inputsdbp_xor_v1.sqd")

        params = defect_influence_operational_domain_params()
        params.defect_influence_params.additional_scanning_area = (20, 20)

        stats_grid = defect_influence_operational_domain_stats()
        _ = defect_influence_operational_domain_grid_search(lyt, [create_xor_tt()], params, 3, stats_grid)
        self.assertGreater(stats_grid.num_operational_parameter_combinations, 0)

        stats_random = defect_influence_operational_domain_stats()
        _ = defect_influence_operational_domain_grid_search(lyt, [create_xor_tt()], params, 100, stats_random)
        self.assertGreater(stats_random.num_operational_parameter_combinations, 0)

        stats_contour_tracing = defect_influence_operational_domain_stats()
        _ = defect_influence_operational_domain_contour_tracing(lyt, [create_xor_tt()], params, 20, stats_contour_tracing)
        self.assertGreater(stats_contour_tracing.num_operational_parameter_combinations, 0)

if __name__ == '__main__':
    unittest.main()
