import unittest

from mnt.pyfiction import *


class TestDisplacementRobustnessDomain(unittest.TestCase):

    def test_siqad_and_gate_100_lattice(self):
        layout = sidb_100_lattice((10, 10))

        layout.assign_cell_type((0, 1), sidb_technology.cell_type.INPUT)
        layout.assign_cell_type((2, 3), sidb_technology.cell_type.INPUT)

        layout.assign_cell_type((20, 1), sidb_technology.cell_type.INPUT)
        layout.assign_cell_type((18, 3), sidb_technology.cell_type.INPUT)

        layout.assign_cell_type((4, 5), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((6, 7), sidb_technology.cell_type.NORMAL)

        layout.assign_cell_type((14, 7), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((16, 5), sidb_technology.cell_type.NORMAL)

        layout.assign_cell_type((10, 12), sidb_technology.cell_type.OUTPUT)
        layout.assign_cell_type((10, 14), sidb_technology.cell_type.OUTPUT)

        layout.assign_cell_type((10, 19), sidb_technology.cell_type.NORMAL)

        params = displacement_robustness_domain_params()

        params.displacement_variations = [1, 1]
        params.operational_params.simulation_parameters = sidb_simulation_parameters(2, -0.28)

        params.operational_params.input_bdl_iterator_params.bdl_pairs_params.maximum_distance = 2.0
        params.operational_params.input_bdl_iterator_params.bdl_pairs_params.minimum_distance = 0.2

        # only offset_coordinate(4, 5) and offset_coordinate(10,12) are affected by displacement
        params.fixed_sidbs = {offset_coordinate(0, 1), offset_coordinate(2, 3), offset_coordinate(20, 1),
                              offset_coordinate(18, 3), offset_coordinate(4, 5), offset_coordinate(14, 7),
                              offset_coordinate(16, 5), offset_coordinate(10, 14), offset_coordinate(10, 19)}

        params.percentage_of_analyzed_displaced_layouts = 0.1
        params.dimer_policy = dimer_displacement_policy.ALLOW_OTHER_DIMER
        params.analysis_mode = displacement_analysis_mode.RANDOM

        stats = displacement_robustness_domain_stats()

        _ = determine_displacement_robustness_domain_100(layout, [create_and_tt()], params, stats)

        self.assertEqual(stats.num_non_operational_sidb_displacements + stats.num_operational_sidb_displacements, 8)


if __name__ == '__main__':
    unittest.main()
