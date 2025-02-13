import unittest

from mnt.pyfiction import (
    create_and_tt,
    create_nor_tt,
    design_sidb_gates,
    design_sidb_gates_mode_100,
    design_sidb_gates_mode_111,
    design_sidb_gates_params_100,
    design_sidb_gates_params_111,
    operational_condition,
    sidb_100_lattice,
    sidb_111_lattice,
    sidb_simulation_engine,
    sidb_technology,
)


class TestDesignSiDBGates(unittest.TestCase):
    def test_siqad_and_gate_skeleton_100(self):
        layout = sidb_100_lattice((20, 20))

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

        params = design_sidb_gates_params_100()
        params.operational_params.simulation_parameters.base = 2
        params.operational_params.simulation_parameters.mu_minus = -0.28
        params.design_mode = design_sidb_gates_mode_100.EXHAUSTIVE_GATE_DESIGNER
        params.canvas = [(4, 8), (14, 11)]
        params.number_of_sidbs = 1
        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT

        self.assertEqual(params.operational_params.simulation_parameters.mu_minus, -0.28)
        self.assertEqual(params.number_of_sidbs, 1)
        self.assertEqual(params.canvas[0], (4, 8, 0))
        self.assertEqual(params.canvas[1], (14, 11))

        designed_gates = design_sidb_gates(layout, [create_and_tt()], params)

        self.assertEqual(len(designed_gates), 23)

    def test_nor_gate_111(self):
        layout = sidb_111_lattice((30, 30))

        layout.assign_cell_type((0, 0), sidb_technology.cell_type.INPUT)
        layout.assign_cell_type((1, 3), sidb_technology.cell_type.INPUT)

        layout.assign_cell_type((25, 0), sidb_technology.cell_type.INPUT)
        layout.assign_cell_type((23, 3), sidb_technology.cell_type.INPUT)

        layout.assign_cell_type((4, 8), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((21, 8), sidb_technology.cell_type.NORMAL)

        layout.assign_cell_type((5, 11), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((19, 11), sidb_technology.cell_type.NORMAL)

        layout.assign_cell_type((8, 16), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((17, 16), sidb_technology.cell_type.NORMAL)

        layout.assign_cell_type((9, 19), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((15, 19), sidb_technology.cell_type.NORMAL)

        layout.assign_cell_type((15, 43), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((17, 46), sidb_technology.cell_type.NORMAL)

        layout.assign_cell_type((19, 51), sidb_technology.cell_type.OUTPUT)
        layout.assign_cell_type((21, 54), sidb_technology.cell_type.OUTPUT)

        layout.assign_cell_type((23, 59), sidb_technology.cell_type.NORMAL)

        params = design_sidb_gates_params_111()
        params.operational_params.simulation_parameters.base = 2
        params.operational_params.simulation_parameters.mu_minus = -0.32
        params.design_mode = design_sidb_gates_mode_111.EXHAUSTIVE_GATE_DESIGNER
        params.canvas = [(10, 22), (14, 34)]
        params.number_of_sidbs = 3
        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT
        params.operational_params.op_condition = operational_condition.REJECT_KINKS

        self.assertEqual(params.operational_params.simulation_parameters.mu_minus, -0.32)
        self.assertEqual(params.number_of_sidbs, 3)
        self.assertEqual(params.canvas[0], (10, 22, 0))
        self.assertEqual(params.canvas[1], (14, 34))

        designed_gates = design_sidb_gates(layout, [create_nor_tt()], params)
        self.assertEqual(len(designed_gates), 44)

        # tolerate kink states
        params.operational_params.op_condition = operational_condition.TOLERATE_KINKS
        designed_gates = design_sidb_gates(layout, [create_nor_tt()], params)
        self.assertEqual(len(designed_gates), 175)

    def test_nor_gate_111_quickcell(self):
        layout = sidb_111_lattice((30, 30))

        layout.assign_cell_type((0, 0), sidb_technology.cell_type.INPUT)
        layout.assign_cell_type((1, 3), sidb_technology.cell_type.INPUT)

        layout.assign_cell_type((25, 0), sidb_technology.cell_type.INPUT)
        layout.assign_cell_type((23, 3), sidb_technology.cell_type.INPUT)

        layout.assign_cell_type((4, 8), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((21, 8), sidb_technology.cell_type.NORMAL)

        layout.assign_cell_type((5, 11), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((19, 11), sidb_technology.cell_type.NORMAL)

        layout.assign_cell_type((8, 16), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((17, 16), sidb_technology.cell_type.NORMAL)

        layout.assign_cell_type((9, 19), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((15, 19), sidb_technology.cell_type.NORMAL)

        layout.assign_cell_type((15, 43), sidb_technology.cell_type.NORMAL)
        layout.assign_cell_type((17, 46), sidb_technology.cell_type.NORMAL)

        layout.assign_cell_type((19, 51), sidb_technology.cell_type.OUTPUT)
        layout.assign_cell_type((21, 54), sidb_technology.cell_type.OUTPUT)

        layout.assign_cell_type((23, 59), sidb_technology.cell_type.NORMAL)

        params = design_sidb_gates_params_111()
        params.operational_params.simulation_parameters.base = 2
        params.operational_params.simulation_parameters.mu_minus = -0.32
        params.design_mode = design_sidb_gates_mode_111.EXHAUSTIVE_GATE_DESIGNER

        params.canvas = [(10, 26), (14, 34)]
        params.number_of_sidbs = 3
        params.operational_params.sim_engine = sidb_simulation_engine.QUICKEXACT

        self.assertEqual(params.operational_params.simulation_parameters.mu_minus, -0.32)
        self.assertEqual(params.number_of_sidbs, 3)
        self.assertEqual(params.canvas[0], (10, 26, 0))
        self.assertEqual(params.canvas[1], (14, 34))

        designed_gates = design_sidb_gates(layout, [create_nor_tt()], params)
        self.assertEqual(len(designed_gates), 14)


if __name__ == "__main__":
    unittest.main()
