import unittest

from mnt.pyfiction import *


class TestDesignSiDBGates(unittest.TestCase):

    def test_siqad_and_gate_skeleton_100(self):
        layout_100 = sidb_100_lattice((20, 20))

        layout_100.assign_cell_type((0, 1), sidb_technology.cell_type.INPUT)
        layout_100.assign_cell_type((2, 3), sidb_technology.cell_type.INPUT)

        layout_100.assign_cell_type((20, 1), sidb_technology.cell_type.INPUT)
        layout_100.assign_cell_type((18, 3), sidb_technology.cell_type.INPUT)

        layout_100.assign_cell_type((4, 5), sidb_technology.cell_type.NORMAL)
        layout_100.assign_cell_type((6, 7), sidb_technology.cell_type.NORMAL)

        layout_100.assign_cell_type((14, 7), sidb_technology.cell_type.NORMAL)
        layout_100.assign_cell_type((16, 5), sidb_technology.cell_type.NORMAL)

        layout_100.assign_cell_type((10, 12), sidb_technology.cell_type.OUTPUT)
        layout_100.assign_cell_type((10, 14), sidb_technology.cell_type.OUTPUT)

        layout_100.assign_cell_type((10, 19), sidb_technology.cell_type.NORMAL)

        params = design_sidb_gates_params()
        params.simulation_parameters.base = 2
        params.simulation_parameters.mu_minus = -0.28
        params.design_mode = design_sidb_gates_mode.EXHAUSTIVE
        params.canvas = [(4, 8), (14, 11)]
        params.number_of_sidbs = 1
        params.sim_engine = sidb_simulation_engine.QUICKEXACT

        self.assertEqual(params.simulation_parameters.mu_minus, -0.28)
        self.assertEqual(params.number_of_sidbs, 1)
        self.assertEqual(params.canvas[0], (4, 8))
        self.assertEqual(params.canvas[1], (14, 11))

        designed_gates = design_sidb_gates(layout_100, [create_and_tt()], params)

        self.assertEqual(len(designed_gates), 23)

    def test_nor_gate_111(self):
        layout_111 = sidb_111_lattice((20, 20))

        layout_111.assign_cell_type((0, 0), sidb_technology.cell_type.INPUT)
        layout_111.assign_cell_type((25, 0), sidb_technology.cell_type.INPUT)

        layout_111.assign_cell_type((23, 3), sidb_technology.cell_type.INPUT)
        layout_111.assign_cell_type((1, 3), sidb_technology.cell_type.INPUT)

        layout_111.assign_cell_type((4, 8), sidb_technology.cell_type.NORMAL)
        layout_111.assign_cell_type((21, 4), sidb_technology.cell_type.NORMAL)

        layout_111.assign_cell_type((5, 11), sidb_technology.cell_type.NORMAL)
        layout_111.assign_cell_type((19, 11), sidb_technology.cell_type.NORMAL)

        layout_111.assign_cell_type((8, 16), sidb_technology.cell_type.NORMAL)
        layout_111.assign_cell_type((17, 16), sidb_technology.cell_type.NORMAL)

        layout_111.assign_cell_type((9, 19), sidb_technology.cell_type.NORMAL)
        layout_111.assign_cell_type((15, 19), sidb_technology.cell_type.NORMAL)

        layout_111.assign_cell_type((15, 43), sidb_technology.cell_type.NORMAL)
        layout_111.assign_cell_type((17, 46), sidb_technology.cell_type.NORMAL)

        layout_111.assign_cell_type((19, 51), sidb_technology.cell_type.OUTPUT)
        layout_111.assign_cell_type((21, 54), sidb_technology.cell_type.OUTPUT)

        layout_111.assign_cell_type((23, 59), sidb_technology.cell_type.NORMAL)

        params = design_sidb_gates_params()
        params.simulation_parameters.base = 2
        params.simulation_parameters.mu_minus = -0.32
        params.design_mode = design_sidb_gates_mode.EXHAUSTIVE
        params.canvas = [(10, 22), (14, 34)]
        params.number_of_sidbs = 3
        params.sim_engine = sidb_simulation_engine.QUICKEXACT

        self.assertEqual(params.simulation_parameters.mu_minus, -0.32)
        self.assertEqual(params.number_of_sidbs, 3)
        self.assertEqual(params.canvas[0], (10, 22))
        self.assertEqual(params.canvas[1], (14, 34))

        designed_gates = design_sidb_gates(layout_111, [create_nor_tt()], params)

        self.assertEqual(len(designed_gates), 206)


if __name__ == '__main__':
    unittest.main()
