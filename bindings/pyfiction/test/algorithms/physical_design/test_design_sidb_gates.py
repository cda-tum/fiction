import unittest
from mnt.pyfiction import *

class TestDesignSiDBGates(unittest.TestCase):

    def test_siqad_and_gate_skeleton(self):
        layout = sidb_layout((20, 20))

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

        params = design_sidb_gates_params()
        params.simulation_parameters.base = 2
        params.simulation_parameters.mu_minus = -0.28
        params.design_mode = design_sidb_gates_mode.EXHAUSTIVE
        params.canvas = [(4, 8), (14, 11)]
        params.number_of_sidbs = 1
        params.sim_engine = sidb_simulation_engine.QUICKEXACT

        self.assertEqual(params.simulation_parameters.mu_minus, -0.28)
        self.assertEqual(params.number_of_sidbs, 1)
        self.assertEqual(params.canvas[0], (4, 8, 0))
        self.assertEqual(params.canvas[1], (14, 11))

        designed_gates = design_sidb_gates(layout, [create_and_tt()], params)

        self.assertEqual(len(designed_gates), 23)


if __name__ == '__main__':
    unittest.main()
