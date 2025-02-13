import os
import unittest

from mnt.pyfiction import (
    bdl_wire_selection,
    create_and_tt,
    detect_bdl_wires_100,
    detect_bdl_wires_params,
    is_kink_induced_non_operational,
    is_operational,
    is_operational_params,
    kink_induced_non_operational_input_patterns,
    operational_analysis_strategy,
    operational_condition_kinks,
    operational_input_patterns,
    operational_status,
    read_sqd_layout_100,
    read_sqd_layout_111,
    sidb_100_lattice,
    sidb_simulation_parameters,
    sidb_technology,
)

dir_path = os.path.dirname(os.path.realpath(__file__))


class TestIsOperational(unittest.TestCase):
    def test_is_operational(self):
        lyt = sidb_100_lattice()

        lyt.assign_cell_type((0, 1), sidb_technology.cell_type.INPUT)
        lyt.assign_cell_type((2, 3), sidb_technology.cell_type.INPUT)

        lyt.assign_cell_type((20, 1), sidb_technology.cell_type.INPUT)
        lyt.assign_cell_type((19, 3), sidb_technology.cell_type.INPUT)

        lyt.assign_cell_type((4, 5), sidb_technology.cell_type.NORMAL)
        lyt.assign_cell_type((6, 7), sidb_technology.cell_type.NORMAL)

        lyt.assign_cell_type((14, 7), sidb_technology.cell_type.NORMAL)
        lyt.assign_cell_type((16, 5), sidb_technology.cell_type.NORMAL)

        lyt.assign_cell_type((10, 12, 0), sidb_technology.cell_type.OUTPUT)
        lyt.assign_cell_type((10, 14, 0), sidb_technology.cell_type.OUTPUT)

        lyt.assign_cell_type((10, 19), sidb_technology.cell_type.NORMAL)

        params = is_operational_params()
        params.simulation_parameters = sidb_simulation_parameters(2, -0.28)

        assessment_results = is_operational(lyt, [create_and_tt()], params)

        self.assertEqual(assessment_results.status, operational_status.OPERATIONAL)

        params.simulation_parameters = sidb_simulation_parameters(2, -0.1)

        assessment_results = is_operational(lyt, [create_and_tt()], params)

        self.assertEqual(assessment_results.status, operational_status.NON_OPERATIONAL)

        # pre-determined I/O pins
        output_bdl_wires = detect_bdl_wires_100(lyt, detect_bdl_wires_params(), bdl_wire_selection.OUTPUT)
        input_bdl_wires = detect_bdl_wires_100(lyt, detect_bdl_wires_params(), bdl_wire_selection.INPUT)
        assessment_results = is_operational(lyt, [create_and_tt()], params, input_bdl_wires, output_bdl_wires)
        self.assertEqual(assessment_results.status, operational_status.NON_OPERATIONAL)

        # pre-determined I/O pins and canvas layout
        canvas_lyt = sidb_100_lattice()
        canvas_lyt.assign_cell_type((4, 5), sidb_technology.cell_type.LOGIC)
        canvas_lyt.assign_cell_type((6, 7), sidb_technology.cell_type.LOGIC)
        assessment_results = is_operational(lyt, [create_and_tt()], params, input_bdl_wires, output_bdl_wires)
        self.assertEqual(assessment_results.status, operational_status.NON_OPERATIONAL)

    def test_and_gate_kinks(self):
        lyt = read_sqd_layout_100(dir_path + "/../../../resources/AND_mu_032_kinks.sqd")

        params = is_operational_params()
        params.simulation_parameters = sidb_simulation_parameters(2, -0.32)

        assessment_results = is_operational(lyt, [create_and_tt()], params)

        self.assertEqual(assessment_results.status, operational_status.OPERATIONAL)

        params.op_condition_kinks = operational_condition_kinks.REJECT_KINKS

        assessment_results = is_operational(lyt, [create_and_tt()], params)

        self.assertEqual(assessment_results.status, operational_status.NON_OPERATIONAL)

    def test_and_gate_non_operational_due_to_kinks(self):
        lyt = read_sqd_layout_100(dir_path + "/../../../resources/AND_mu_032_kinks.sqd")

        params = is_operational_params()
        params.simulation_parameters = sidb_simulation_parameters(2, -0.32)

        result = is_kink_induced_non_operational(lyt, [create_and_tt()], params)

        self.assertTrue(result)

    def test_and_gate_non_operational_input_patterns_due_to_kinks(self):
        lyt = read_sqd_layout_100(dir_path + "/../../../resources/AND_mu_032_kinks.sqd")

        params = is_operational_params()
        params.simulation_parameters = sidb_simulation_parameters(2, -0.32)

        non_operational_pattern_kinks = kink_induced_non_operational_input_patterns(lyt, [create_and_tt()], params)

        self.assertEqual(non_operational_pattern_kinks, {1, 2})

    def test_and_gate_111_lattice_11_input_pattern(self):
        lyt = read_sqd_layout_111(dir_path + "/../../../resources/AND_mu_032_111_surface.sqd")

        params = is_operational_params()
        params.simulation_parameters = sidb_simulation_parameters(2, -0.32)

        assessment_results = is_operational(lyt, [create_and_tt()], params)

        self.assertEqual(assessment_results.status, operational_status.OPERATIONAL)

        params.simulation_parameters = sidb_simulation_parameters(2, -0.1)

        self.assertEqual(params.simulation_parameters.mu_minus, -0.1)

        assessment_results = is_operational(lyt, [create_and_tt()], params)

        self.assertEqual(assessment_results.status, operational_status.NON_OPERATIONAL)

        # filer only
        params.strategy_to_analyze_operational_status = operational_analysis_strategy.FILTER_ONLY
        self.assertEqual(params.strategy_to_analyze_operational_status, operational_analysis_strategy.FILTER_ONLY)
        assessment_results = is_operational(lyt, [create_and_tt()], params)
        self.assertEqual(assessment_results.status, operational_status.NON_OPERATIONAL)

        # filer then simulation
        params.strategy_to_analyze_operational_status = operational_analysis_strategy.FILTER_THEN_SIMULATION
        self.assertEqual(
            params.strategy_to_analyze_operational_status, operational_analysis_strategy.FILTER_THEN_SIMULATION
        )
        assessment_results = is_operational(lyt, [create_and_tt()], params)
        self.assertEqual(assessment_results.status, operational_status.NON_OPERATIONAL)

    def test_and_gate_111_lattice_operational_input_pattern(self):
        lyt = read_sqd_layout_111(dir_path + "/../../../resources/AND_mu_032_111_surface.sqd")

        params = is_operational_params()
        params.simulation_parameters = sidb_simulation_parameters(2, -0.30)

        operational_patterns = operational_input_patterns(lyt, [create_and_tt()], params)

        print(operational_patterns)
        self.assertEqual(len(operational_patterns), 2)

        self.assertEqual(operational_patterns, {0, 3})


if __name__ == "__main__":
    unittest.main()
