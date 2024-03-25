import unittest
from mnt.pyfiction import *

class TestDesignSiDBGates(unittest.TestCase):

    def test_siqad_and_gate_skeleton(self):
        layout = sidb_layout((2, 1))
        layout.assign_cell_type((0, 0, 1), sidb_technology.cell_type.INPUT)
        layout.assign_cell_type((2, 1, 1), sidb_technology.cell_type.INPUT)
        layout.assign_cell_type((20, 0, 1), sidb_technology.cell_type.INPUT)
        layout.assign_cell_type((18, 1, 1), sidb_technology.cell_type.INPUT)

        layout.assign_cell_type((4, 2, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((6, 3, 1), sidb_technology.cell_type.NORMAL)

        layout.assign_cell_type((14, 3, 1), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((16, 2, 1), sidb_technology.cell_type.NORMAL)

        layout.assign_cell_type((10, 6, 0), sidb_technology.cell_type.OUTPUT)
        layout.assign_cell_type((10, 7, 0), sidb_technology.cell_type.OUTPUT)

        layout.assign_cell_type((10, 9, 1), sidb_technology.cell_type.NORMAL)

        params = design_sidb_gates_params()
        params.simulation_parameters.base = 2
        params.simulation_parameters.mu_minus = -0.28
        params.design_mode = design_sidb_gates_mode.EXHAUSTIVE
        params.canvas = [(4, 4, 0), (14, 5, 1)]
        params.sim_engine = sidb_simulation_engine.QUICKEXACT
        self.assertEqual(params.simulation_parameters.mu_minus, -0.28)

        designed_gates = design_sidb_gates(layout, [create_and_tt()], params)

        self.assertEqual(len(designed_gates), 23)


if __name__ == '__main__':
    unittest.main()
